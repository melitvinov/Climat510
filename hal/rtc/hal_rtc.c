#include "syntax.h"
#include "stm32f10x.h"
#include "hal_rtc.h"

#include "debug.h"

#define RTC_OK_MAGIC    0xCAFE

static void wait_for_write_completion(void)
{
    while (! (RTC->CRL & RTC_CRL_RTOFF));
}

static void reset_clock(u32 timestamp)
{
    PWR->CR |= PWR_CR_DBP;  // allow access to RTC and backup registers

    wait_for_write_completion();
    RTC->CRL |= RTC_CRL_CNF;    // enter config mode
    wait_for_write_completion();

    RCC->BDCR |= RCC_BDCR_BDRST;    // start resetting backup domain
    RCC->BDCR &= ~RCC_BDCR_BDRST;   // stop resetting backup domain :-)
    RCC->BDCR |= RCC_BDCR_LSEON;     // start xtal

    #warning "xtal may fail. we could switch to rc as a graceful degradation"
    while (! (RCC->BDCR & RCC_BDCR_LSERDY));

    RCC->BDCR |= RCC_BDCR_RTCSEL_LSE;
    RCC->BDCR |= RCC_BDCR_RTCEN;

    while (! (RTC->CRL & RTC_CRL_RSF));

    RTC->PRLL = 0x7FFF;
    RTC->PRLH = 0;
    wait_for_write_completion();

    RTC->CNTL = timestamp;
    RTC->CNTH = timestamp >> 16;
    wait_for_write_completion();

    RTC->CRL &= ~RTC_CRL_CNF;    // exit config mode
    wait_for_write_completion();

    BKP->DR1 = RTC_OK_MAGIC;

    PWR->CR &= ~PWR_CR_DBP;  // forbid access to backup registers
}

void HAL_rtc_init(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_PWREN | RCC_APB1ENR_BKPEN;

    if (BKP->DR1 != RTC_OK_MAGIC || !(RCC->BDCR & RCC_BDCR_RTCEN))
    {
        WARN("magic = %d, resetting rtc clock", BKP->DR1);
        reset_clock(365 * 40 * 24 * 60 * 60);
    }
}

static u32 read_counter(void)
{
    // avoid wrong result if read is coincident the low hword overflow
    while (1)
    {
        u32 high = RTC->CNTH;
        u32 low = RTC->CNTL;
        if (RTC->CNTH == high)
            return (high << 16) | low;
    }
}

u32 HAL_rtc_get_timestamp(void)
{
    return read_counter();
}


void HAL_rtc_set_timestamp(u32 timestamp)
{
    PWR->CR |= PWR_CR_DBP;

    wait_for_write_completion();
    RTC->CRL |= RTC_CRL_CNF;
    wait_for_write_completion();

    RTC->CNTL = timestamp;
    RTC->CNTH = timestamp >> 16;

    wait_for_write_completion();

    RTC->CRL &= ~RTC_CRL_CNF;
    wait_for_write_completion();

    PWR->CR &= ~PWR_CR_DBP;
}
