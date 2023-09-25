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
#define GPIO_CRL_OFFSET ((uint32_t) 0x00) // CONFIGURATION REGISTER LOW (0-7) OFFSET
#define GPIO_CRH_OFFSET ((uint32_t) 0x04) // CONFIGURATION REGISTER HIGH (8-15) OFFSET
#define GPIO_ODR_OFFSET ((uint32_t) 0x0C) // OUTPUT DATA REGISTER OFFSET
#define GPIO_IDR_OFFSET ((uint32_t) 0x08) // INPUT DATA REGISTER OFFSET



//DEFINICIONES DE BITS PARA CONFIG DEL PB13/PB5 COMO OUTPUT (SALIDA CON LED) - CRH/CRL (MISMOS BITS PARA PB13 EN CRH Y PB5 EN CRL)
#define PB13_MODE_BIT0 20
#define PB13_MODE_BIT1 21
#define PB13_CNF_BIT0 22
#define PB13_CNF_BIT1 23
#define PB13_ODR_BIT 13
#define PB13_IDR_BIT 13
#define PB5_ODR_BIT 5
#define PB5_IDR_BIT 5
#define GPIO_IDR_BIT5 ((uint32_t) 0x00000020) // BIT 5 DEL IDR


int main(void)
{


    //CALCULO LAS DIRECCIONES DEL APB2ENR, CRH Y ODR
    volatile uint32_t *pAPB2ENR = (uint32_t *)(RCC_BASE_ADDRESS + RCC_APB2ENR_OFFSET);
    volatile uint32_t *pCRL = (uint32_t *)(GPIOB_BASE_ADDRESS + GPIO_CRL_OFFSET);
    volatile uint32_t *pCRH = (uint32_t *)(GPIOB_BASE_ADDRESS + GPIO_CRH_OFFSET);
    volatile uint32_t *pODR = (uint32_t *)(GPIOB_BASE_ADDRESS + GPIO_ODR_OFFSET);
    volatile uint32_t *pIDR = (uint32_t *)(GPIOB_BASE_ADDRESS + GPIO_IDR_OFFSET);
    
    //*pAPB2ENR |= (0x1 << APB2ENR_GPIOC_ENA_BIT); //DOY ENABLE AL CLOCK DEL GPIOC
    *pAPB2ENR |= (0x1 << APB2ENR_GPIOB_ENA_BIT); //DOY ENABLE AL CLOCK DEL GPIOB

    //PB13 SETUP AS OUTPUT
    *pCRH |= (0x1 << PB13_MODE_BIT0); 
    *pCRH |= (0x0 << PB13_MODE_BIT1); //SETEO EL MODE DEL CRH CON 01 MODO OUTPUT MAX 10MHz
    *pCRH |= (0x0 << PB13_CNF_BIT0); 
    *pCRH |= (0x0 << PB13_CNF_BIT1); // SETEO EL CNF DEL CRH CON 00 GENERAL PURPOSE OUTPUT PUSH-PULL

    //PB5 SETUP AS INPUT
    *pCRL |= (0x0 << PB13_MODE_BIT0); 
    *pCRL |= (0x0 << PB13_MODE_BIT1); //SETEO EL MODE DEL CRL CON 00 INPUT MODE
    *pCRL |= (0x1 << PB13_CNF_BIT0); 
    *pCRL |= (0x0 << PB13_CNF_BIT1); // SETEO EL CNF DEL CRL CON 01 FLOATING INPUT (RESET STATE)



	for(;;){
        if(*pIDR & GPIO_IDR_BIT5 != 0 ){ //VEO SI EL BIT 5 DEL IDR DEL PB5 ESTA EN 1
            *pODR |= (0x0 << PB13_ODR_BIT); //PONGO EN 0 EL BIT DEL ODR (OUTPUT DATA)    (((((((((((((VER SI FUNCA,SINO PONERLO EN 1)))))))))))))
        }else{
            *pODR |= (0x1 << PB13_ODR_BIT); //PONGO EN 0 EL BIT DEL ODR (OUTPUT DATA)    (((((((((((((VER SI FUNCA,SINO PONERLO EN 0)))))))))))))
        }



    }

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