typedef int            int32_t;
typedef short          int16_t;
typedef char           int8_t;
typedef unsigned int   uint32_t;
typedef unsigned short uint16_t;
typedef unsigned char  uint8_t;
typedef void(*interrupt_t)(void);

#define SRAM_SIZE ((uint32_t) 0x00005000)
#define SRAM_BASE ((uint32_t) 0x20000000)
#define STACKINIT ((interrupt_t)(SRAM_BASE+SRAM_SIZE))

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
#define GPIO_B ((GPIO_Type *) GPIOB_BASE_ADDRESS)


// Clock registers
#define RCC_BASE			0x40021000	// Direccion base del Clock
#define RCC_CR_OFFSET		0x00
#define RCC_CFGR_OFFSET		0x04
#define AHBENR_OFFSET		0x14
#define APB2ENR_OFFSET		0x18
#define APB1ENR_OFFSET		0x1C

#define FLASH_BASE			0x40022000
#define FLASH_ACR_OFFSET	0x00

// Direccion del registro APB2ENR del RCC
volatile int * const RCC_APB2ENR_ptr = (int *)(RCC_BASE + APB2ENR_OFFSET);
volatile int * const RCC_APB1ENR_ptr = (int *)(RCC_BASE + APB1ENR_OFFSET);
volatile int * const RCC_AHBENR_ptr = (int *)(RCC_BASE + AHBENR_OFFSET);
volatile int * const RCC_CR_ptr = (int *)(RCC_BASE + RCC_CR_OFFSET);
volatile int * const RCC_CFGR_ptr = (int *)(RCC_BASE + RCC_CFGR_OFFSET);

volatile int * const FLASH_ACR_ptr = (int *)(FLASH_BASE + FLASH_ACR_OFFSET);
//DEFINICIONES DE BITS PARA CONFIG DEL PC13 COMO OUTPUT
#define PC13_MODE_BIT0 20
#define PC13_MODE_BIT1 21
#define PC13_CNF_BIT0 22
#define PC13_CNF_BIT1 23
#define PC13_ODR_BIT 13
#define PB6_ODR_BIT 6
#define PB7_ODR_BIT 7
#define PB4_MODE_BIT0 16
#define PB4_MODE_BIT1 17
#define PB4_CNF_BIT0 18
#define PB4_CNF_BIT1 19 
#define PB5_MODE_BIT0 20
#define PB5_MODE_BIT1 21
#define PB5_CNF_BIT0 22
#define PB5_CNF_BIT1 23
#define PB6_MODE_BIT0 24
#define PB6_MODE_BIT1 25
#define PB6_CNF_BIT0 26
#define PB6_CNF_BIT1 27
#define PB7_MODE_BIT0 28
#define PB7_MODE_BIT1 29
#define PB7_CNF_BIT0 30
#define PB7_CNF_BIT1 31
#define PB5_IDR_BIT 5
#define PB4_IDR_BIT 4

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

void SystemClockInit(void){
    *RCC_CR_ptr |= (1 << 16); 				// Enable HSE
	while (!(*RCC_CR_ptr & (1 << 17)));		// Wait for HSE is locked

	*RCC_CR_ptr &= ~(1 << 24); 				// Disable PLL
	*RCC_CFGR_ptr |= (0b0111 << 18);        // Set PLLMULL to 9. Set PLL output clock to 72 Mhz
	*RCC_CFGR_ptr |= (1 << 16);             // Select HSE as the PLL source clock
	*RCC_CR_ptr |= (1 << 24);               // Enable PLL
	while (!(*RCC_CR_ptr & (1 << 25)));		// Wait for PLL to lock
	
	/*FLASH ACR*/
	*FLASH_ACR_ptr |= (0b010 << 0);			// Set FLASH WAIT STATE to 2
	*RCC_CFGR_ptr |= (0b0000 << 4);         // Set AHB HPRE division to 1. Set AHB clock to 72 Mhz
	*RCC_CFGR_ptr |= (0b0010 << 14);         // Set AHB HPRE division to 1. Set AHB clock to 72 Mhz
	*RCC_CFGR_ptr |= (0b100 << 8);          // Set APB1 PPRE1 division to 2. Set AHB clock to 36 Mhz
	*RCC_CFGR_ptr |= (0b10 << 0);			// Select PLL clock as the system clock
	while (!(*RCC_CFGR_ptr & (0b10 << 2)));	// Wait for PLL clock to be selected
}

void GPIOInit(void){

    //CALCULO LA DIRECCION DEL APB2ENR
    volatile uint32_t *pAPB2ENR = (uint32_t *)(RCC_BASE_ADDRESS + RCC_APB2ENR_OFFSET);

    *pAPB2ENR |= (0x1 << APB2ENR_GPIOC_ENA_BIT); //DOY ENABLE AL CLOCK DEL GPIOC
    *pAPB2ENR |= (0x1 << APB2ENR_GPIOB_ENA_BIT); //DOY ENABLE AL CLOCK DEL GPIOB

    //SETEO EL CRH DEL PC13 COMO 00 GENERAL PURPOSE OUTPUT Y OUTPUT CON 01 MODO OUTPUT MAX 10MHz
    GPIO_C->GPIO_CRH |= ((0x1 << PC13_MODE_BIT0) | (0x0 << PC13_MODE_BIT1) | (0x0 << PC13_CNF_BIT0) | (0x0 << PC13_CNF_BIT1));
    GPIO_B->GPIO_CRL |= ((0x1 << PB6_MODE_BIT0) | (0x0 << PB6_MODE_BIT1) | (0x0 << PB6_CNF_BIT0) | (0x0 << PB6_CNF_BIT1)); // LED 2
    GPIO_B->GPIO_CRL |= ((0x1 << PB7_MODE_BIT0) | (0x0 << PB7_MODE_BIT1) | (0x0 << PB7_CNF_BIT0) | (0x0 << PB7_CNF_BIT1)); // LED 3
    //SETEO EL CRL DEL PB5 COMO - MODE 00  INPUT - CNF 10 INPUT WITH PUSH-PULL
    GPIO_B->GPIO_CRL |= ((0x0 << PB5_MODE_BIT0) | (0x0 << PB5_MODE_BIT1) | (0x0 << PB5_CNF_BIT0) | (0x1 << PB5_CNF_BIT1)); // MODULO A
    GPIO_B->GPIO_CRL |= ((0x0 << PB4_MODE_BIT0) | (0x0 << PB4_MODE_BIT1) | (0x0 << PB4_CNF_BIT0) | (0x1 << PB4_CNF_BIT1)); // MODULO B

    GPIO_C->GPIO_ODR |= (0x1 << PC13_ODR_BIT); // INICIALIZO EL LED PC13 APAGADO
}

void SystickIRQInit_ms(void){
/*     //#####################SYSTICK CODE###########################
    //EXTRAIGO LOS ULTIMOS 24 BITS DEL REGISTRO DE CALIBRACION (TENMS)
    uint32_t CalibrationValue_10ms = SysTick->SYST_CALIB; //RELOAD VALUE PARA 10ms
    uint32_t CalibrationValue_ms = (CalibrationValue_10ms/10) *ms; //modifico el reload value para tener un tiempo de ms milisegundos
    //BORRO LOS ULTIMOS 24 BITS DEL REGISTRO DE RELOAD VALUE
    //uint32_t LoadCurrentValue = SysTick->SYST_RVR & 0xFF000000;
    //ARMO LA PALABRA USANDO LOS PRIMEROS 8 BITS DEL RVR Y LOS 24 DEL TENMS DEL CALIB Y LOS ASIGNO AL RVR
    //uint32_t ReloadValue = SysTick->SYST_RVR & 0xFF0000000;
    SysTick->SYST_RVR |= CalibrationValue_ms ; //1° SETEO EL RELOAD VALUE (ESTO SERIA COMO GUARDAR LOS PRIMEROS DOS BYTES DEL RVR Y PEGAR LOS ULTIMOS 6 BYTES DEL CALIB)
    SysTick->SYST_CVR |= (0x1 << 0) ; //2° - ESCRIBO UN 1 EN EL PRIMER BIT DEL REGISTRO CURRENT VALUE PARA LIMPIARLO
    //AHORA SI 3° SETEO EL CONTROL AND STATUS REGISTER CSR, INCLUYENDO ENABLE
    SysTick->SYST_CSR |= ((0x1 << CSR_ENA_BIT) | (0x0 << CSR_TICKINT_BIT) | (0x0 << CSR_CLKSRC_BIT) ); // OTRA FORMA DE HACER LO DE ARRIBA EN UNA LINEA (SETEO ENABLE, TICKINT Y CLK COMO EXTERNO 110)
    //4°HAGO ESTO EN ESTE ORDEN PORQUE ASI LO DICE LA USER GUIDE DEL CORTEX-M3 */

    SysTick->SYST_RVR=72000; // Pongo en SYSTEM_RELOAD_VALUE para que funcione cada 1 seg con el reloj de 72MHZ
	SysTick->SYST_CSR= ((0<<16) |(1<<2) | (1<<1) | (1<<0)); //Configuro el Control and Status Register BIT=16 (FLAG), BIT=2 (CLKSOURCE), BIT=1 (TICKINT),  BIT=0 (ENABLE)
}

volatile uint32_t tiempo_ms = 0;
//Esta es la funcion que se ejecuta por interrupciones
void SysTick_handler(void)
{
    tiempo_ms++;
}

void delay_ms(uint32_t delay){
    volatile uint32_t timer_actual;
    timer_actual = tiempo_ms;
    while(tiempo_ms-timer_actual < delay);

}

uint32_t GetIDR(void){
    uint32_t IDR = GPIO_B->GPIO_IDR;
    return IDR;
}

int main(void)
{
    SystemClockInit();
    GPIOInit();
    //SystickIRQInit_ms();


    uint32_t pc13ODRMask = 1 << PC13_ODR_BIT; //DEFINO LA MASCARA DEL ODR CON EL BIT 13 EN 1 
    /*
    uint32_t SysTickCountFlagMask = 1 << CSR_COUNTFLAG_BIT;//DEFINO LA MASCARA DEL CSR CON EL BIT 16 (COUNTFLAG) EN 1
    uint32_t pb5IDRMask = 1 << PB5_IDR_BIT;
    uint32_t pb4IDRMask = 1 << PB4_IDR_BIT;
    uint32_t pb6ODRMask = 1 << PB6_ODR_BIT;
    uint32_t pb7ODRMask = 1 << PB7_ODR_BIT;
    
    */
    uint32_t contador = 0;


	while(1){


        uint32_t GPIOBIDRValue = GetIDR();

        /*
        if((GPIOBIDRValue == 16))  { //VEO SI EL BIT 16 DEL CSR ESTA EN 1
            GPIO_C->GPIO_ODR = (0x0 << PC13_ODR_BIT); // CAMBIO EL ESTADO DEL BIT 13 DEL ODR
            for(volatile uint32_t i=0;i<5000000;i++);
            GPIO_C->GPIO_ODR = (0x1 << PC13_ODR_BIT);
        }

        if((GPIOBIDRValue == 32))  { //VEO SI EL BIT 16 DEL CSR ESTA EN 1
            GPIO_C->GPIO_ODR = (0x0 << PC13_ODR_BIT); // CAMBIO EL ESTADO DEL BIT 13 DEL ODR
            for(volatile uint32_t i=0;i<5000000;i++);
            GPIO_C->GPIO_ODR = (0x1 << PC13_ODR_BIT);
        } */

        if((GPIOBIDRValue == 16))  { //VEO SI EL BIT 16 DEL CSR ESTA EN 1
            for(volatile uint32_t i=0;i<500000;i++);
            uint32_t GPIOBIDRValue = GetIDR();
            if(GPIOBIDRValue == 0){
                GPIO_C->GPIO_ODR = (0x0 << PC13_ODR_BIT); // CAMBIO EL ESTADO DEL BIT 13 DEL ODR
                GPIO_B->GPIO_ODR = (0x1 << PB6_ODR_BIT);
                for(volatile uint32_t i=0;i<5000000;i++);
                GPIO_C->GPIO_ODR = (0x1 << PC13_ODR_BIT);
                GPIO_B->GPIO_ODR = (0x0 << PB6_ODR_BIT);
                contador++;
            }

        }

        if((GPIOBIDRValue == 32))  { //VEO SI EL BIT 16 DEL CSR ESTA EN 1
            for(volatile uint32_t i=0;i<500000;i++);
            uint32_t GPIOBIDRValue = GetIDR();
            if(GPIOBIDRValue == 0){
                GPIO_C->GPIO_ODR = (0x0 << PC13_ODR_BIT); // CAMBIO EL ESTADO DEL BIT 13 DEL ODR
                for(volatile uint32_t i=0;i<5000000;i++);
                GPIO_C->GPIO_ODR = (0x1 << PC13_ODR_BIT);
                if(contador > 0){
                    contador--;
            }
        }

    }
return 0;
}
}

const interrupt_t vector_table[] __attribute__ ((section(".vtab"))) = {
	STACKINIT,												// 0x0000_0000 Stack Pointer
	(interrupt_t) main, 									// 0x0000_0004 Reset
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
	0,                                        // Habilito las interrupciones por Systick
};
