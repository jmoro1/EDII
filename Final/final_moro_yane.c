typedef int int32_t;
typedef short int16_t;
typedef char int8_t;
typedef unsigned int uint32_t;
typedef unsigned short uint16_t;
typedef unsigned char uint8_t;
typedef void (*interrupt_t)(void);

/*Base Addresses*/
#define SRAM_SIZE ((uint32_t)0x00005000)
#define SRAM_BASE ((uint32_t)0x20000000)
#define STACKINIT ((interrupt_t)(SRAM_BASE + SRAM_SIZE))
#define PERIPH_BASE ((uint32_t)0x40000000)
#define GPIOC_BASE ((uint32_t)0x40011000) // GPIOC base address
#define GPIOB_BASE ((uint32_t)0x40010C00) // GPIOB base address
#define GPIOA_BASE ((uint32_t)0x40010800) // GPIOA base address
#define AFIO_BASE ((uint32_t)0x40010C00)
#define RCC_BASE ((uint32_t)0x40021000) // RCC base address
#define APB1PERIPH_BASE PERIPH_BASE
#define APB2PERIPH_BASE (uint32_t)(PERIPH_BASE + 0x10000)
#define AHBPERIPH_BASE (uint32_t)(PERIPH_BASE + 0x20000)
#define FLASH_BASE (uint32_t)(AHBPERIPH_BASE + 0x2000) // FLASH base address is 0x40022000
#define EXTI_BASE (APB2PERIPH_BASE + 0x0400)		   //  EXTI base address is 0x40010400
#define NVIC_BASE ((uint32_t)0xE000E100)			   // NVIC base address
#define SYSTICK_BASE ((uint32_t)0xE000E010)			   // Systick base address

/*Struct Pointers*/
#define GPIO_A ((GPIO_Type *)GPIOA_BASE)
#define GPIO_B ((GPIO_Type *)GPIOB_BASE)
#define GPIO_C ((GPIO_Type *)GPIOC_BASE)
#define RCC ((RCC_Type *)RCC_BASE)
#define AFIO ((AFIO_Type *)AFIO_BASE)
#define FLASH ((FLASH_Type *)FLASH_BASE)
#define EXTI ((EXTI_Type *)EXTI_BASE)
#define NVIC ((NVIC_Type *)NVIC_BASE)
#define SysTick ((SysTick_Type *)SYSTICK_BASE)

/* Individual bit definitions*/
#define PC13_MODE_BIT0 20
#define PC13_MODE_BIT1 21
#define PC13_CNF_BIT0 22
#define PC13_CNF_BIT1 23
#define PC13_ODR_BIT 13
#define P4_IDR_BIT 4
#define P5_IDR_BIT 5
#define P4_ODR_BIT 4
#define P5_ODR_BIT 5
#define P6_ODR_BIT 6
#define P7_ODR_BIT 7
#define P4_MODE_BIT0 16
#define P4_MODE_BIT1 17
#define P4_CNF_BIT0 18
#define P4_CNF_BIT1 19
#define P5_MODE_BIT0 20
#define P5_MODE_BIT1 21
#define P5_CNF_BIT0 22
#define P5_CNF_BIT1 23
#define P6_MODE_BIT0 24
#define P6_MODE_BIT1 25
#define P6_CNF_BIT0 26
#define P6_CNF_BIT1 27
#define P7_MODE_BIT0 28
#define P7_MODE_BIT1 29
#define P7_CNF_BIT0 30
#define P7_CNF_BIT1 31
#define APB2ENR_GPIOA_ENA_BIT 2
#define APB2ENR_GPIOC_ENA_BIT 4
#define APB2ENR_GPIOB_ENA_BIT 3


/*Constants Definitions*/
#define SYSTICK_RELOAD_VALUE_1s (72000000 - 1)
#define MAX_COUNT 3


/*Register Structs*/
typedef struct
{
	volatile uint32_t CRL;		 /*Port configuration register low (0-7),		Address offset: 0x00*/
	volatile uint32_t CRH;		 /*Port configuration register HIGH (8-15),		Address offset: 0x04*/
	volatile const uint32_t IDR; /*Input data register,							Address offset: 0x08*/
	volatile uint32_t ODR;		 /*Output data register,						Address offset: 0x0C*/
	volatile uint32_t BSRR;		 /*Bit set/reset register,						Address offset: 0x10*/
	volatile uint32_t BRR;		 /*Bit reset register,							Address offset: 0x14*/
	volatile uint32_t LCKR;		 /*Port Configuration LOCK register,			Address offset: 0x18*/
} GPIO_Type;

typedef struct
{
	volatile uint32_t CR;		/* RCC clock control register,                Address offset: 0x00 */
	volatile uint32_t CFGR;		/* RCC clock configuration register,          Address offset: 0x04 */
	volatile uint32_t CIR;		/* RCC clock interrupt register,              Address offset: 0x08 */
	volatile uint32_t APB2RSTR; /* RCC APB2 peripheral reset register,        Address offset: 0x0C */
	volatile uint32_t APB1RSTR; /* RCC APB1 peripheral reset register,        Address offset: 0x10 */
	volatile uint32_t AHBENR;	/* RCC AHB peripheral clock enable register,  Address offset: 0x14 */
	volatile uint32_t APB2ENR;	/* RCC APB2 peripheral clock enable register, Address offset: 0x18 */
	volatile uint32_t APB1ENR;	/* RCC APB1 peripheral clock enable register, Address offset: 0x1C */
	volatile uint32_t BDCR;		/* RCC backup domain control register,        Address offset: 0x20 */
	volatile uint32_t CSR;		/* RCC control/status register,               Address offset: 0x24 */
	volatile uint32_t AHBRSTR;	/* RCC AHB peripheral clock reset register,   Address offset: 0x28 */
	volatile uint32_t CFGR2;	/* RCC clock configuration register 2,        Address offset: 0x2C */
} RCC_Type;

typedef struct
{
	uint32_t ACR;
	uint32_t KEYR;
	uint32_t OPTKEYR;
	uint32_t SR;
	uint32_t CR;
	uint32_t AR;
	uint32_t RESERVED;
	uint32_t OBR;
	uint32_t WRPR;
} FLASH_Type;

typedef struct
{
	uint32_t EVCR;	  /* Address offset: 0x00 */
	uint32_t MAPR;	  /* Address offset: 0x04 */
	uint32_t EXTICR1; /* Address offset: 0x08 */
	uint32_t EXTICR2; /* Address offset: 0x0C */
	uint32_t EXTICR3; /* Address offset: 0x10 */
	uint32_t EXTICR4; /* Address offset: 0x14 */
	uint32_t MAPR2;	  /* Address offset: 0x18 */
} AFIO_Type;

typedef struct
{
	uint32_t IMR;	/* Interrupt mask register,            Address offset: 0x00 */
	uint32_t EMR;	/* Event mask register,                Address offset: 0x04 */
	uint32_t RTSR;	/* Rising trigger selection register,  Address offset: 0x08 */
	uint32_t FTSR;	/* Falling trigger selection register, Address offset: 0x0C */
	uint32_t SWIER; /* Software interrupt event register,  Address offset: 0x10 */
	uint32_t PR;	/* Pending register,                   Address offset: 0x14 */
} EXTI_Type;

typedef struct
{
	volatile uint32_t SYST_CSR;			/*Control and status register,		 Address offset: 0x00*/
	volatile uint32_t SYST_RVR;			/*Reload value register, 			 Address offset: 0x04*/
	volatile uint32_t SYST_CVR;			/*Current value register, 			 Address offset: 0x08*/
	volatile const uint32_t SYST_CALIB; /*Calibration value register, Address offset: 0x0C*/
} SysTick_Type;

typedef struct
{
	uint32_t ISER[8];	/* Address offset: 0x000 - 0x01C */
	uint32_t RES0[24];	/* Address offset: 0x020 - 0x07C */
	uint32_t ICER[8];	/* Address offset: 0x080 - 0x09C */
	uint32_t RES1[24];	/* Address offset: 0x0A0 - 0x0FC */
	uint32_t ISPR[8];	/* Address offset: 0x100 - 0x11C */
	uint32_t RES2[24];	/* Address offset: 0x120 - 0x17C */
	uint32_t ICPR[8];	/* Address offset: 0x180 - 0x19C */
	uint32_t RES3[24];	/* Address offset: 0x1A0 - 0x1FC */
	uint32_t IABR[8];	/* Address offset: 0x200 - 0x21C */
	uint32_t RES4[56];	/* Address offset: 0x220 - 0x2FC */
	uint8_t IPR[240];	/* Address offset: 0x300 - 0x3EC */
	uint32_t RES5[644]; /* Address offset: 0x3F0 - 0xEFC */
	uint32_t STIR;		/* Address offset:         0xF00 */
} NVIC_Type;

void SystemClockInit(void)
{
	RCC->CR |= (1 << 16); // Enable HSE
	while (!(RCC->CR & (1 << 17)))
		; // Wait for HSE is locked

	RCC->CR &= ~(1 << 24);		 // Disable PLL
	RCC->CFGR |= (0b0111 << 18); // Set PLLMULL to 9. Set PLL output clock to 72 Mhz
	RCC->CFGR |= (1 << 16);		 // Select HSE as the PLL source clock
	RCC->CR |= (1 << 24);		 // Enable PLL
	while (!(RCC->CR & (1 << 25)))
		; // Wait for PLL to lock

	/*FLASH ACR*/
	FLASH->ACR |= (0b010 << 0);	 // Set FLASH WAIT STATE to 2
	RCC->CFGR |= (0b0000 << 4);	 // Set AHB HPRE division to 1. Set AHB clock to 72 Mhz
	RCC->CFGR |= (0b0010 << 14); // Set AHB HPRE division to 1. Set AHB clock to 72 Mhz
	RCC->CFGR |= (0b100 << 8);	 // Set APB1 PPRE1 division to 2. Set AHB clock to 36 Mhz
	RCC->CFGR |= (0b10 << 0);	 // Select PLL clock as the system clock
	while (!(RCC->CFGR & (0b10 << 2)))
		; // Wait for PLL clock to be selected
}

void GPIOInit(void)
{
	RCC->APB2ENR |= (0x1 << APB2ENR_GPIOA_ENA_BIT); // Enable GPIOA
	RCC->APB2ENR |= (0x1 << APB2ENR_GPIOB_ENA_BIT); // Enable GPIOB
	RCC->APB2ENR |= (0x1 << APB2ENR_GPIOC_ENA_BIT); // Enable GPIOC

	// Output pin setting (LEDs) as  GENERAL PURPOSE OUTPUT (00) and MODE OUTPUT MAX 2MHz (10)
	GPIO_C->CRH |= ((0x1 << PC13_MODE_BIT0) | (0x0 << PC13_MODE_BIT1) | (0x0 << PC13_CNF_BIT0) | (0x0 << PC13_CNF_BIT1));

	GPIO_A->CRL |= ((0x1 << P4_MODE_BIT0) | (0x0 << P4_MODE_BIT1) | (0x0 << P4_CNF_BIT0) | (0x0 << P4_CNF_BIT1)); // LED 1
	GPIO_A->CRL |= ((0x1 << P5_MODE_BIT0) | (0x0 << P5_MODE_BIT1) | (0x0 << P5_CNF_BIT0) | (0x0 << P5_CNF_BIT1)); // LED 2
	GPIO_A->CRL |= ((0x1 << P6_MODE_BIT0) | (0x0 << P6_MODE_BIT1) | (0x0 << P6_CNF_BIT0) | (0x0 << P6_CNF_BIT1)); // LED 3
	GPIO_A->CRL |= ((0x1 << P7_MODE_BIT0) | (0x0 << P7_MODE_BIT1) | (0x0 << P7_CNF_BIT0) | (0x0 << P7_CNF_BIT1)); // LED 4
	// Input pin setting as MODE 00  INPUT - CNF 01
	GPIO_B->CRL |= ((0x0 << P5_MODE_BIT0) | (0x0 << P5_MODE_BIT1) | (0x0 << P5_CNF_BIT0) | (0x1 << P5_CNF_BIT1)); // IR MODULE A
	GPIO_B->CRL |= ((0x0 << P4_MODE_BIT0) | (0x0 << P4_MODE_BIT1) | (0x0 << P4_CNF_BIT0) | (0x1 << P4_CNF_BIT1)); // IR MODULE B
	// Leds initialize off
	GPIO_C->ODR |= ((0x1 << PC13_ODR_BIT));
	GPIO_A->ODR = ((0x1 << P4_ODR_BIT) | (0x1 << P5_ODR_BIT) | (0x1 << P6_ODR_BIT) | (0x1 << P7_ODR_BIT));
}

void SystickInit(void)
{

	SysTick->SYST_RVR = (SYSTICK_RELOAD_VALUE_1s / 1000);			   // Sets the reload value for a 1ms tick
	SysTick->SYST_CVR |= (0x1 << 0);								   // Cleans the current count
	SysTick->SYST_CSR |= ((0 << 16) | (0 << 2) | (1 << 1) | (1 << 0)); // Enables the Systick and his interrupt, cleans the count flag and selects external clock source
}

volatile uint32_t Ticks_ms = 0; // Global variable to be used by the IRQ Handler

void SysTick_Handler(void) // Systick IRQ Handler function
{
	Ticks_ms++; // This happens each ms
}

void delay_ms(uint32_t ms)
{
	uint32_t Initial_Ticks = Ticks_ms;
	while ((Ticks_ms - Initial_Ticks) < ms)
	{
		// Waits
	}
}

uint32_t GetIDR(void)
{
	uint32_t IDR = GPIO_B->IDR;
	return IDR;
}

int main(void)
{

	SystemClockInit();
	GPIOInit();
	SystickInit();
	uint32_t contador = 0;
	uint32_t pb5IDRMask = (1 << P5_IDR_BIT);
	uint32_t pb4IDRMask = (1 << P4_IDR_BIT);

	while (1)
	{

		uint32_t GPIOBIDRValue = GetIDR();

		if ((GPIOBIDRValue == pb4IDRMask)) // Right to Left Case, adds to count
		{
			delay_ms(300);
			GPIOBIDRValue = GetIDR();
			if (GPIOBIDRValue == 0)
			{
				GPIO_A->ODR &= ~(0x1 << P5_ODR_BIT);
				delay_ms(500);
				GPIO_A->ODR |= (0x1 << P5_ODR_BIT);
				contador++;
			}
		}

		if ((GPIOBIDRValue == pb5IDRMask)) // Left to Right Case, decrease the count
		{
			delay_ms(300);
			GPIOBIDRValue = GetIDR();
			if (GPIOBIDRValue == 0)
			{
				GPIO_A->ODR &= ~(0x1 << P4_ODR_BIT);
				delay_ms(500);
				GPIO_A->ODR |= (0x1 << P4_ODR_BIT);
				if (contador > 0)
				{
					contador--;
				}
			}
		}

		if (contador >= MAX_COUNT) // Counter reaching max Case, toggles the red LED 5 times and cleans the count
		{
			for (volatile uint32_t j = 0; j < 10; j++)
			{
				delay_ms(300);
				GPIO_A->ODR ^= (0x1 << P6_ODR_BIT);
			}
			contador = 0;
		}

		if ((GPIOBIDRValue == 0)) // "Failure" State when both sensors are active, toggles the red led 4 times rapidly
		{
			for (volatile uint32_t i = 0; i < 500000; i++)
			{
			};
			GPIOBIDRValue = GetIDR();
			if (GPIOBIDRValue == 0)
			{

				for (volatile uint32_t j = 0; j < 8; j++)
				{
					delay_ms(200);
					GPIO_A->ODR ^= (0x1 << P7_ODR_BIT);
				}
			}
		}
	}
	return 0;
}

typedef void (*interrupt_t)(void);
const interrupt_t vector_table[256] __attribute__((section(".vtab"))) = {
	STACKINIT,		   // 0x0000_0000 Stack Pointer
	(interrupt_t)main, // 0x0000_0004 Reset
	0,				   // 0x0000_0008
	0,				   // 0x0000_000C
	0,				   // 0x0000_0010
	0,				   // 0x0000_0014
	0,				   // 0x0000_0018
	0,				   // 0x0000_001C
	0,				   // 0x0000_0020
	0,				   // 0x0000_0024
	0,				   // 0x0000_0028
	0,				   // 0x0000_002C
	0,				   // 0x0000_0030
	0,				   // 0x0000_0034
	0,				   // 0x0000_0038
	SysTick_Handler,   // 0x0000_003C
};
