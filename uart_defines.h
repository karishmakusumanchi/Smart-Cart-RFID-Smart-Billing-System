#define FOSC 12000000
#define CCLK (FOSC*5)
#define PCLK (CCLK/4)
#define BAUD 9600
#define DIVISOR (PCLK/(16*BAUD))

//pin connect block defines
#define TxD0_PIN_FUNC 0x00000001
#define RxD0_PIN_FUNC 0x00000004

#define TxD1_PIN_FUNC 0x00010000
#define RxD1_PIN_FUNC 0x00040000

//defines for UXLCR
#define _8BIT 3
#define WOR_LEN_SEL_ _8BIT
#define DLAB_BIT 7

//defines for UXLSR
#define DR_BIT 0
#define TEMT_BIT 6


