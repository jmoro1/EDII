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


typedef struct{ //DECLARO UNA STRUCT PARA ACCEDER A LOS REGISTROS DE LOS GPIO
    volatile uint32_t GPIO_CRL; //PORT CONFIGURATION REGISTER LOW (0-7) (OFFSET 0x00)
    volatile uint32_t GPIO_CRH; //PORT CONFIGURATION REGISTER HIGH (8-15)(OFFSET 0x04) 
    volatile const uint32_t GPIO_IDR;//INPUT DATA REGISTER(OFFSET 0x08) 
    volatile uint32_t GPIO_ODR;//OUTPUT DATA REGISTER(OFFSET 0x0C)
    volatile uint32_t GPIO_BSRR; // BIT SET/RESET REGISTER(OFFSET 0x10)
    volatile uint32_t GPIO_BRR;//BIT RESET REGISTER(OFFSET 0x14)
    volatile uint32_t GPIO_LCKR;//PORT CONFIGURATION LOCK REGISTER(OFFSET 0x18)
}GPIO_Type;   

//PUNTERO A LA STRUCT GPIO
#define GPIO_C ((GPIO_Type *) GPIOC_BASE_ADDRESS)

//DEFINICIONES DE BITS PARA CONFIG DEL PC13 COMO OUTPUT
#define PC13_MODE_BIT0 20
#define PC13_MODE_BIT1 21
#define PC13_CNF_BIT0 22
#define PC13_CNF_BIT1 23
#define PC13_ODR_BIT 13     

//SYSTICK ADDRESS
#define SYSTICK_BASE_ADDRESS ((uint32_t) 0xE000E010)
#define CSR_ENA_BIT 0
#define CSR_TICKINT_BIT 1
#define CSR_CLKSRC_BIT 2
#define CSR_COUNTFLAG_BIT 16



typedef struct{ //DECLARO UNA STRUCT PARA ACCEDER A LOS REGISTROS DEL SYSTICK
    volatile uint32_t SYST_CSR; //CONTROL AND STATUS REGISTER (SHOULD BE 0xE000E010)
    volatile uint32_t SYST_RVR; //RELOAD VALUE REGISTER (SHOULD BE 0xE000E014 ) 
    volatile uint32_t SYST_CVR;//CURRENT VALUE REGISTER (SHOULD BE 0xE000E018 )
    volatile const uint32_t SYST_CALIB;//CALIBRATION VALUE REGISTER (SHOULD BE 0xE000E01C )

}SysTick_Type;   

//PUNTERO A LA STRUCT SYSTICK
#define SysTick ((SysTick_Type *) SYSTICK_BASE_ADDRESS)

void SysTick_handler(void);

int main(void)
{
    //CALCULO LA DIRECCION DEL APB2ENR
    volatile uint32_t *pAPB2ENR = (uint32_t *)(RCC_BASE_ADDRESS + RCC_APB2ENR_OFFSET);

    *pAPB2ENR |= (0x1 << APB2ENR_GPIOC_ENA_BIT); //DOY ENABLE AL CLOCK DEL GPIOC
    
    //SETEO EL CRH COMO 00 GENERAL PURPOSE OUTPUT Y OUTPUT CON 01 MODO OUTPUT MAX 10MHz
    GPIO_C->GPIO_CRH |= ((0x1 << PC13_MODE_BIT0) | (0x0 << PC13_MODE_BIT1) | (0x0 << PC13_CNF_BIT0) | (0x0 << PC13_CNF_BIT1));// LO DE ARRIBA EN UNA LINEA

    //EXTRAIGO LOS ULTIMOS 24 BITS DEL REGISTRO DE CALIBRACION (TENMS)
    uint32_t CalibrationValue_10ms = SysTick->SYST_CALIB; //RELOAD VALUE PARA 10ms
    uint32_t CalibrationValue_1s = CalibrationValue_10ms *100; //modifico el reload value para tener un tiempo de 1000ms
    //BORRO LOS ULTIMOS 24 BITS DEL REGISTRO DE RELOAD VALUE
    //uint32_t LoadCurrentValue = SysTick->SYST_RVR & 0xFF000000;
    //ARMO LA PALABRA USANDO LOS PRIMEROS 8 BITS DEL RVR Y LOS 24 DEL TENMS DEL CALIB Y LOS ASIGNO AL RVR
    uint32_t ReloadValue = SysTick->SYST_RVR & 0xFF0000000;
    SysTick->SYST_RVR |= CalibrationValue_1s ; //1° SETEO EL RELOAD VALUE (ESTO SERIA COMO GUARDAR LOS PRIMEROS DOS BYTES DEL RVR Y PEGAR LOS ULTIMOS 6 BYTES DEL CALIB)
    SysTick->SYST_CVR |= (0x1 << 0) ; //2° - ESCRIBO UN 1 EN EL PRIMER BIT DEL REGISTRO CURRENT VALUE PARA LIMPIARLO
    //AHORA SI 3° SETEO EL CONTROL AND STATUS REGISTER CSR, INCLUYENDO ENABLE
    SysTick->SYST_CSR |= ((0x1 << CSR_ENA_BIT) | (0x0 << CSR_TICKINT_BIT) | (0x0 << CSR_CLKSRC_BIT) ); // OTRA FORMA DE HACER LO DE ARRIBA EN UNA LINEA (SETEO ENABLE, TICKINT Y CLK COMO EXTERNO 110)
    //4°HAGO ESTO EN ESTE ORDEN PORQUE ASI LO DICE LA USER GUIDE DEL CORTEX-M3
    GPIO_C->GPIO_ODR |= (0x1 << PC13_ODR_BIT); // INICIALIZO EL LED PC13 APAGADO
    uint32_t pc13ODRMask = 1 << PC13_ODR_BIT; //DEFINO LA MASCARA DEL ODR CON EL BIT 13 EN 1 
    uint32_t SysTickCountFlagMask = 1 << CSR_COUNTFLAG_BIT;//DEFINO LA MASCARA DEL CSR CON EL BIT 16 (COUNTFLAG) EN 1


	while(1){

return 0;
}
#define SRAM_SIZE ((uint32_t) 0x00005000)
#define SRAM_BASE ((uint32_t) 0x20000000)
#define STACKINIT ((interrupt_t)(SRAM_BASE+SRAM_SIZE))

typedef void(*interrupt_t)(void);
const interrupt_t vector_table[256] __attribute__ ((section(".vtab"))) = {
	STACKINIT,												// 0x0000_0000 Stack Pointer
	(interrupt_t) main,										// 0x0000_0004 Reset
    0,														// 0x0000_0008
	0,														// 0x0000_000C
	0,														// 0x0000_0010
	0,														// 0x0000_0014
	0,														// 0x0000_0018
	0,														// 0x0000_001C
	0,														// 0x0000_0020
	0,														// 0x0000_0024
	0,														// 0x0000_0028
	0,														// 0x0000_002C
	0,														// 0x0000_0030
	0,														// 0x0000_0034
	0,														// 0x0000_0038
	SysTick_handler,   
};

volatile uint32_t pc13ODRMask = 1 << PC13_ODR_BIT; //DEFINO LA MASCARA DEL ODR CON EL BIT 13 EN 1 
void SysTick_handler(void)
{

	GPIO_C->GPIO_ODR ^= pc13ODRMask; // CAMBIO EL ESTADO DEL BIT 13 DEL ODR
}


