void CheckWithoutPC(void);


void Init_STM32(void);
void TestMem(uchar);

// XXX: isolation
void stm32f10x_Rootines_reset_NMinPCOut(void);

void GetRTC(uint16_t *time, uint16_t *date, uint8_t *year, u8 *day_of_week);
