//---------------------- Подпрограммы для функционирования STM32 -------------------------------


//#define PORT_KEYB_IN	GPIOA
//#define PORT_KEYB_OUT	GPIOC
//#define KEYB_IN		0x000f
//#define KEYB_OUT	0x000f
//#define KEYB_STARTUP RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE)

void Init_IWDG(uint16_t* fIWDG_reset);

void Check_IWDG(void);

void CheckWithoutPC(void);


void Init_STM32(void);

void InitMainTimer(void);

void InitIPCTimer(void);

void process_legacy_timers(void);

void w1Init(void);
void w1_check(void);
int16_t w1reset(void);
void w1_wr(void);
void w1_rd(void);
void CrcCalc(void);
void Reg48ToI2C(void);
void OutReg(void);

void TestMem(uchar);

void Measure(void);
void CheckInputConfig(void);


// XXX: isolation
void stm32f10x_Rootines_reset_NMinPCOut(void);

void GetRTC(uint16_t *time, uint16_t *date, uint8_t *year, u8 *day_of_week);
