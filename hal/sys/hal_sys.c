#include "syntax.h"
#include "stm32f10x.h"

#include "hal_debug.h"
#include "hal_sys.h"

//-- linker symbols
extern u32 __data;
extern u32 __data_end;
extern u32 __data_load;
extern u32 __bss;
extern u32 __bss_end;
extern u32 __stack_bottom;
extern u32 __stack_top;

extern void main(void) __noreturn;

__naked static void reset_handler(void);
__naked static void nmi_handler(void);

typedef struct __packed
{
    u32 *stack;
    void (*reset)(void);
    void (*exceptions[14])(void);
    void (*isr_vectors[68])(void);
} vector_table_t;


extern void UART4_IRQHandler(void);

__weak void timer4_isr(void)    {    HAL_assert("unhandled timer4 isr");}
__weak void timer6_isr(void)    {    HAL_assert("unhandled timer6 isr");}
__weak void timer7_isr(void)    {    HAL_assert("unhandled timer7 isr");}
__weak void usart1_isr(void)    {    HAL_assert("unhandled usart1 isr");}

static void unhandled_isr(void)
{
    HAL_assert("unhandled isr");
}

// flash-based vector table
static const __attribute__ ((section(".initvectors"), used)) vector_table_t initvects =
{
    .stack = &__stack_top,
    .reset = reset_handler,
    .exceptions =
    {
        [0 ... 13] = nmi_handler,
    },
    .isr_vectors =
    {
        [0 ... 67] = unhandled_isr,
        [TIM4_IRQn] = timer4_isr,
        [TIM6_IRQn] = timer6_isr,
        [TIM7_IRQn] = timer7_isr,
        [USART1_IRQn] = usart1_isr,
        [UART4_IRQn] = UART4_IRQHandler,
    }
};

__naked static void nmi_handler(void)
{
    u32 *sp;
    u32 pc;

    // XXX: this calc may be wrong
    __asm__ volatile ("mrs %0, msp" : "=r" (sp));
    pc = *(sp + 5) - 4;

    hal_exception(pc, sp);
}

static void init_sys(void)
{
    RCC->CR |= RCC_CR_HSION;    // enable rc (just to be safe)

    // switch to rc
    RCC->CFGR &= ~(RCC_CFGR_SW);
    while ((RCC->CFGR & RCC_CFGR_SWS_HSI) != RCC_CFGR_SWS_HSI);

    // disable clock security
    RCC->CR &= ~RCC_CR_CSSON;

    // reset clocks leaving reserved bits as is, wait for pll to be disconnected
    RCC->CR &= 0xC0F0FFFF;
    while (RCC->CR & (RCC_CR_PLL3RDY | RCC_CR_PLL2RDY | RCC_CR_PLLRDY));

    // reset cfg leaving reserved bits as is
    RCC->CFGR &= 0xF0800000;

    // crear interrupt flags
    RCC->CIR |= 0x00FF0000;

    // clear cfg2, except the reserved bits
    // AHB, APB1, APB2 prescalers are == 1 now
    RCC->CFGR2 &= 0xFFF80000;


    // init system clocks

    // flash memory: prefetch buffer, two waitstates
    FLASH->ACR = (FLASH->ACR & 0xFFFFFFC0) | FLASH_ACR_PRFTBE | FLASH_ACR_LATENCY_1;

    // pll: 8MHz/2 * 9 = 36MHz
    RCC->CFGR |= RCC_CFGR_PLLMULL9;
    RCC->CR |= RCC_CR_PLLON;
    while (! (RCC->CR & RCC_CR_PLLRDY));

    // switch sys clock to pll
    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);

    // don't touch the AHB/APB1/APB2 clocks, leave them as is.
    // but enable port and alternate function clocks
    RCC->APB2ENR |= RCC_APB2ENR_IOPDEN | RCC_APB2ENR_IOPCEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPAEN | RCC_APB2ENR_AFIOEN;

    // leave jtag running for now, but without njreset and trace pins.
    // this will free pa15, pb3, pb4
    AFIO->MAPR |= (AFIO->MAPR & ~AFIO_MAPR_SWJ_CFG) | AFIO_MAPR_SWJ_CFG_JTAGDISABLE;
}

__naked static void reset_handler(void)
{
    __disable_irq();

    init_sys();

    // Relocate .data section
    for (u32 *from = &__data_load, *to = &__data; to < &__data_end; from++, to++)
        *to = *from;

    // Clear .bss
    for (u32 *to = &__bss; to < &__bss_end; to++)
        *to = 0;

    // Fill stack with funky pattern
    for (u32 *to = &__stack_bottom; to < &__stack_top; to++)
        *to = 0x55AA55AAUL;

    SCB->VTOR = (u32)&initvects;

    __enable_irq();

    main();
}
