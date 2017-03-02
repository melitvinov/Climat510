#include "syntax.h"
#include "stm32f10x.h"
#include "hal_pincfg.h"

static const u32 output_speed_val = 0x03;  // up to 50 MHz


void hal_pincfg_in(GPIO_TypeDef *port, uint pin_idx)
{
    volatile u32 *reg;
    if (pin_idx < 8)
    {
        reg = &port->CRL;
    }
    else
    {
        reg = &port->CRH;
        pin_idx -= 8;
    }

    uint idx = pin_idx * 4;
    *reg = (*reg & ~(0x0F << idx)) | ((0x08 | 0x00) << idx);
}


void hal_pincfg_highz(GPIO_TypeDef *port, uint pin_idx)
{
    volatile u32 *reg;
    if (pin_idx < 8)
    {
        reg = &port->CRL;
    }
    else
    {
        reg = &port->CRH;
        pin_idx -= 8;
    }

    uint idx = pin_idx * 4;
    *reg = (*reg & ~(0x0F << idx)) | ((0x04 | 0x00) << idx);
}


void hal_pincfg_out(GPIO_TypeDef *port, uint pin_idx)
{
    volatile u32 *reg;
    if (pin_idx < 8)
    {
        reg = &port->CRL;
    }
    else
    {
        reg = &port->CRH;
        pin_idx -= 8;
    }

    uint idx = pin_idx * 4;
    *reg = (*reg & ~(0x0F << idx)) | ((0x00 | output_speed_val) << idx);
}


void hal_pincfg_od(GPIO_TypeDef *port, uint pin_idx)
{
    volatile u32 *reg;
    if (pin_idx < 8)
    {
        reg = &port->CRL;
    }
    else
    {
        reg = &port->CRH;
        pin_idx -= 8;
    }

    uint idx = pin_idx * 4;
    *reg = (*reg & ~(0x0F << idx)) | ((0x04 | output_speed_val) << idx);
}


void hal_pincfg_out_alt(GPIO_TypeDef *port, uint pin_idx)
{
    volatile u32 *reg;
    if (pin_idx < 8)
    {
        reg = &port->CRL;
    }
    else
    {
        reg = &port->CRH;
        pin_idx -= 8;
    }

    uint idx = pin_idx * 4;
    *reg = (*reg & ~(0x0F << idx)) | ((0x08 | output_speed_val) << idx);
}


void hal_pincfg_od_alt(GPIO_TypeDef *port, uint pin_idx)
{
    volatile u32 *reg;
    if (pin_idx < 8)
    {
        reg = &port->CRL;
    }
    else
    {
        reg = &port->CRH;
        pin_idx -= 8;
    }

    uint idx = pin_idx * 4;
    *reg = (*reg & ~(0x0F << idx)) | ((0x0C | output_speed_val) << idx);
}
