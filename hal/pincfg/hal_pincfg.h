#ifndef _HAL_PINCFG_H_
#define _HAL_PINCFG_H_

void hal_pincfg_in(GPIO_TypeDef *port, uint pin_idx);
void hal_pincfg_highz(GPIO_TypeDef *port, uint pin_idx);
void hal_pincfg_out(GPIO_TypeDef *port, uint pin_idx);
void hal_pincfg_od(GPIO_TypeDef *port, uint pin_idx);
void hal_pincfg_out_alt(GPIO_TypeDef *port, uint pin_idx);
void hal_pincfg_od_alt(GPIO_TypeDef *port, uint pin_idx);

#endif
