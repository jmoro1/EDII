typedef int            int32_t;
typedef short          int16_t;
typedef char           int8_t;
typedef unsigned int   uint32_t;
typedef unsigned short uint16_t;
typedef unsigned char  uint8_t;

//RCC AND APB2ENR ADRESS AND OFFSET
#define RCC_BASE_ADDRESS ((uint32_t) 0x40021000) //RESET AND CLOCK CONTROL BASE ADDRESS
#define RCC_APB2ENR_OFFSET ((uint32_t) 0x18) //PERIPHERAL CLOCK ENABLE REGISTER OFFSET
#define APB2ENR_GPIOC_ENA_BIT 4 //BIT 4 TIENE QUE ESTAR EN 1 PARA DARLE ENABLE AL CLOCK DEL GPIOC
#define APB2ENR_GPIOB_ENA_BIT 3 //BIT 4 TIENE QUE ESTAR EN 1 PARA DARLE ENABLE AL CLOCK DEL GPIOB


//GPIOC AND GPIOB, CRH AND ODR BASE ADDRESS AND OFFSETS
#define GPIOC_BASE_ADDRESS ((uint32_t) 0x40011000) // GPIOC BASE ADDRESS
#define GPIOB_BASE_ADDRESS ((uint32_t) 0x40010C00) // GPIOB BASE ADDRESS
#define GPIO_CRH_OFFSET ((uint32_t) 0x04) // CONFIGURATION REGISTER HIGH OFFSET
#define GPIO_ODR_OFFSET ((uint32_t) 0x0C) // OUTPUT DATA REGISTER OFFSET
//#define GPIO_IDR_OFFSET ((uint32_t) 0x08) // INPUT DATA REGISTER OFFSET



//DEFINICIONES DE BITS PARA CONFIG DEL PC13 COMO OUTPUT
#define PC13_MODE_BIT0 20
#define PC13_MODE_BIT1 21
#define PC13_CNF_BIT0 22
#define PC13_CNF_BIT1 23
#define PC13_ODR_BIT 13

//DEFINICIONES DE BITS PARA CONFIG DEL PB14 COMO OUTPUT
#define PB14_MODE_BIT0 24
#define PB14_MODE_BIT1 25
#define PB14_CNF_BIT0 26
#define PB14_CNF_BIT1 27
#define PB14_ODR_BIT 14

int main(void)
{

	// DEVMAP->RCC.REGs.APB2ENR |= (1 << 4);                   // Enable GPIOC clock.

	// DEVMAP->GPIOs[GPIOC].REGs.CRL  = 0x33333333;            // Make low GPIOC output
	// DEVMAP->GPIOs[GPIOC].REGs.CRH  = 0x33333333;            // Make high GPIOC output
	// DEVMAP->GPIOs[GPIOC].REGs.ODR ^= -1;

    //CALCULO LAS DIRECCIONES DEL APB2ENR, CRH Y ODR
    volatile uint32_t *pAPB2ENR = (uint32_t *)(RCC_BASE_ADDRESS + RCC_APB2ENR_OFFSET);
    volatile uint32_t *pCRH_B = (uint32_t *)(GPIOB_BASE_ADDRESS + GPIO_CRH_OFFSET);
    volatile uint32_t *pODR_B = (uint32_t *)(GPIOB_BASE_ADDRESS + GPIO_ODR_OFFSET);
    //volatile uint32_t *pIDR_B = (uint32_t *)(GPIOB_BASE_ADDRESS + GPIO_IDR_OFFSET);
    volatile uint32_t *pCRH_C = (uint32_t *)(GPIOC_BASE_ADDRESS + GPIO_CRH_OFFSET);
    volatile uint32_t *pODR_C = (uint32_t *)(GPIOC_BASE_ADDRESS + GPIO_ODR_OFFSET);  
    //volatile uint32_t *pIDR_C = (uint32_t *)(GPIOC_BASE_ADDRESS + GPIO_IDR_OFFSET);

    *pAPB2ENR |= (0x1 << APB2ENR_GPIOC_ENA_BIT); //DOY ENABLE AL CLOCK DEL GPIOC
    *pAPB2ENR |= (0x1 << APB2ENR_GPIOB_ENA_BIT); //DOY ENABLE AL CLOCK DEL GPIOB

    //PC13 SETUP AS OUTPUT
    *pCRH_C |= (0x1 << PC13_MODE_BIT0); 
    *pCRH_C |= (0x0 << PC13_MODE_BIT1); //SETEO EL MODE DEL CRH CON 01 MODO OUTPUT MAX 10MHz
    *pCRH_C |= (0x0 << PC13_CNF_BIT0); 
    *pCRH_C |= (0x0 << PC13_CNF_BIT1); // SETEO EL CNF DEL CRH CON 00 GENERAL PURPOSE OUTPUT PUSH-PULL
    *pODR_C |= (0x0 << PC13_ODR_BIT); //PONGO EN 0 EL BIT DEL ODR (OUTPUT DATA)    (((((((((((((VER SI FUNCA,SINO PONERLO EN 1)))))))))))))

    //PB14 SETUP AS OUTPUT
    *pCRH_B |= (0x1 << PB14_MODE_BIT0); 
    *pCRH_B |= (0x0 << PB14_MODE_BIT1); //SETEO EL MODE DEL CRH CON 01 MODO OUTPUT MAX 10MHz
    *pCRH_B |= (0x0 << PB14_CNF_BIT0); 
    *pCRH_B |= (0x0 << PB14_CNF_BIT1); // SETEO EL CNF DEL CRH CON 00 GENERAL PURPOSE OUTPUT PUSH-PULL
    *pODR_B |= (0x1 << PB14_ODR_BIT); /*PONGO EN 1 EL BIT DEL ODR (OUTPUT DATA)    ((((((((((((((((LO PONGO EN 1 PORQUE NOSE SI TIENE TENSION POR DEFECTO))))))))))))))))
    (((((((((((((VER SI FUNCA,SINO PONERLO EN 1)))))))))))))*/


	for(;;);

	return 0;
}

#define SRAM_SIZE ((uint32_t) 0x00005000)
#define SRAM_BASE ((uint32_t) 0x20000000)
#define STACKINIT ((interrupt_t)(SRAM_BASE+SRAM_SIZE))

typedef void(*interrupt_t)(void);
const interrupt_t vector_table[256] __attribute__ ((section(".vtab"))) = {
	STACKINIT,												// 0x0000_0000 Stack Pointer
	(interrupt_t) main,										// 0x0000_0004 Reset
};