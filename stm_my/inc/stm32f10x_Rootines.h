//---------------------- Подпрограммы для функционирования STM32 -------------------------------


#define PORT_KEYB_IN	GPIOA
#define PORT_KEYB_OUT	GPIOC
#define KEYB_IN		0x000f
#define KEYB_OUT	0x000f
#define KEYB_STARTUP RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE)

void Init_IWDG(uint16_t* fIWDG_reset);

void Check_IWDG(void);

void CheckWithoutPC(void);


void Init_STM32(void);

void InitMainTimer(void);

void InitIPCTimer(void);

void InitRTC(void);

void Keyboard_Init(void);

char CheckKeyboardSTM();

void WriteDateTime();

char ReadDateTime();

void SendBlockFRAM(uint16_t fStartAddr,uint8_t* AdrBlock,uint16_t sizeBlock);

void RecvBlockFRAM(uint16_t fStartAddr,uint8_t* AdrBlock,uint16_t sizeBlock);

void WriteToFRAM();

void ReadFromFRAM();

void CopyEEP();
void CalcEEPSum();
void w1Init();
void w1_check();
int16_t w1reset();
void w1_wr();
void w1_rd();
void CrcCalc();
void Reg48ToI2C();
void OutReg();

void Measure();
void CheckInputConfig();
