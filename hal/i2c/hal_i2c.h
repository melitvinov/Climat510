#ifndef _HAL_I2C_H_
#define _HAL_I2C_H_

void hal_i2c_init(void);
bool hal_i2c_read(uint addr, const void *preamble, uint preamble_len, void *data, uint data_len);
bool hal_i2c_write(uint addr, const void *preamble, uint preamble_len, const void *data, uint data_len);

#ifdef _HAL_I2C_C_
// private

#define I2C_TRANSACTION_RETRY   32

#endif
#endif
