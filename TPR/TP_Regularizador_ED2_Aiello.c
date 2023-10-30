#define SRAM_SIZE		((uint32_t) 0x00005000)
#define SRAM_BASE		((uint32_t) 0x20000000)
#define STACKINIT		((interrupt_t)(SRAM_BASE+SRAM_SIZE))

#include <math.h>
#define PI 3.14159265
#define SAMPLES 512

typedef int			   int32_t;
typedef short		   int16_t;
typedef char		   int8_t;
typedef unsigned int   uint32_t;
typedef unsigned short uint16_t;
typedef unsigned char  uint8_t;

typedef void(*interrupt_t)(void);

int  main(void);

const interrupt_t vector_table[SAMPLES] __attribute__ ((section(".vtab"))) = {
	STACKINIT,												// 0x0000_0000 Stack Pointer
	(interrupt_t) main,										// 0x0000_0004 Reset
};

/*Vector data donde se guardan los ceros y unos de cada señal que se envía*/
uint32_t data[SAMPLES] = {};

/**SIGMA_DELTA**/
void sigma_delta(int * wavein, int mysamples, int cont){
	int i;
	int integrator = wavein[0];
	int waveout_aux[mysamples]; 
	
	for(i=0; i<mysamples; i++){
		if(i > 0) integrator += (wavein[i] - waveout_aux[i-1]);
		if(integrator > 0){
			waveout_aux[i] = 127*cont;
			data[i] = 0x00002000;								// Es un 1 en el pin 13 del ODR por donde saldrá la señal en la placa
		}
		else {
			waveout_aux[i] = -127*cont;
			data[i] = 0x00000000;								// Es un 0 en el pin 13 del ODR por donde saldrá la señal en la placa
		}
	}
}


int main(void)
{
	// PCLK code
	int volatile *CR = 0x40021000;
	*CR |= (1 << 16);										// Enable HSE
	while (!(*CR & (1 << 17)));								// Wait for HSE is locked
	*CR &= ~(1 << 24);										// Disable PLL

	int volatile *CFGR = 0x40021000 + 0x04;
	*CFGR |= (0b0111 << 18);								// Set PLLMULL to 9. Set PLL output clock to 72 Mhz
	*CFGR |=  (1 << 16);									// Select HSE as the PLL source clock
	*CR |=  (1 << 24);										// Enable PLL
	while (!(*CR & (1 << 25)));								// Wait for PLL to lock

/*FLASH ACR*/
	int volatile *ACR = 0x40022000;							
	*ACR |= (0b010 << 0);									// Set FLASH WAIT STATE to 2
	*CFGR |= (0b0000 << 4);									// Set AHB HPRE division to 1. Set AHB clock to 72 Mhz
	*CFGR |= (0b100 << 8);									// Set APB1 PPRE1 division to 2. Set AHB clock to 36 Mhz
	*CFGR |= (0b10 << 0);									// Select PLL clock as the system clock
	while (!(*CFGR & (0b10 << 2)));							// Wait for PLL clock to be selected


/* DMA code*/
	int volatile *APB2ENR = 0x40021000 + 0x18;				// APB2 clock enable adress
	*APB2ENR |= (1<<4);										// Enable GPIOC clock (output)
	*APB2ENR |= (1<<3);										// Enable GPIOB clock (input for polling)
	int volatile *APB1ENR = 0x40021000 + 0x1C;				// APB1 clock enable adress
	*APB1ENR |= (1 << 0);									// Enable TIM2 clock
	int volatile *AHBENR = 0x40021000 + 0x14;				// Enable DMA1 clock
	*AHBENR |= (1 << 0);

	int volatile *ODR = 0x40011000 + 0x0c;					// ODR GPIOC

/*GPIOB (input)*/
	int volatile *crlb = 0x40010c00;						// Low Register
	*crlb = 0x88888888;										// Input -> MODE=00 & CNF=10 |1000|1000|1000|1000|
	int volatile *crhb = 0x40010c00 + 0x04;					// High Register
	*crhb = 0x88888888;										// Input -> MODE=00 & CNF=10 |1000|1000|1000|1000|
	int volatile *IDR = 0x40010c00 + 0x08;

/*GPIOC (output)*/
	int volatile *CRL = 0x40011000 + 0x00;
	*CRL = 0x33333333;										// Make low GPIOC output |0011|0011|0011|0011
	int volatile *CRH = 0x40011000 + 0x04;
	*CRH = 0x33333333;										// Make low GPIOC output |0011|0011|0011|0011

/*DMA*/
	int volatile *DMA_CNDTR = 0x40020000 + 0x20;			// offset = 0x0c + 0d20 * (2-1) channel 2
	*DMA_CNDTR = sizeof(data)/sizeof(uint32_t);				// transfer size
	int volatile *DMA_CMAR = 0x40020000 + 0x28;				// offset = 0x14 + 0d20 * (2-1) channel 2
	*DMA_CMAR = (uint32_t) data;							// Memory source address
	int volatile *DMA_CPAR = 0x40020000 + 0x24;				// offset = 0x10 + 0d20 * (2-1) channel 2
	*DMA_CPAR = (uint32_t) & *ODR;							// Peripheral destination address

	int volatile *DMA_CCR = 0x40020000 + 0x1c;				// offset = 0x08 + 0d20 * (2-1) channel 2
	*DMA_CCR = 0x00000000;									// Reset CCR --> set CCR's value to 0x00000000
	*DMA_CCR &= ~(1 << 14); 								// Disable memory to memory transfer on DMA1 channel 2 --> (MEM2MEM = 0)
	*DMA_CCR |= (0b11 << 12);								// Set DMA priority to very high --> (PL = 11)
	*DMA_CCR |= (0b10 << 10);								// Set memory transfer size to 32-bits -->(MSIZE = 10)
	*DMA_CCR |= (0b10 << 8);								// Set peripheral transfer size to 32-bits -->(PSIZE = 10)
	*DMA_CCR |= (1 << 7);									// Enable memory increment mode -->(MINC = 1)
	*DMA_CCR &= ~(1 << 6);									// Disable peripheral increment mode -->(PINC = 0)
	*DMA_CCR |=  (1 << 5);									// Enable circular mode -->(CIRC = 1)
	*DMA_CCR |=  (1 << 4);									// Read from memory -->(DIR = 1)
	*DMA_CCR |=  (1 << 2);									// Enable half transfer completed interrupt -->(HTIE = 1)
	*DMA_CCR |=  (1 << 1);									// Enable transfer completed interrupt -->(TCIE = 1)

	*DMA_CCR |=  (1 << 0);									// Enable DMA -->(EN = 1)


/*TIM2*/
	int volatile *TIM2_CR1 = 0x40000000 + 0x00;					// offset CR1 0x00
	*TIM2_CR1 = 0x0000;											// Reset CR1 just in case
	int volatile *TIM2_PSC = 0x40000000 + 0x28;					// offset PSC 0x28
	*TIM2_PSC = (0.9e6/100)/(sizeof(data)/sizeof(data[0]))-1;	// fCK_PSC / (PSC[15:0] + 1)
	int volatile *TIM2_ARR = 0x40000000 + 0x2c;					// offset ARR 0x2c
	*TIM2_ARR = 8-1;
	int volatile *TIM2_DIER = 0x40000000 + 0x0c;				// offset DIER 0x0c
	*TIM2_DIER |= (1 << 14);									// Trigger DMA request enable
	*TIM2_DIER |= (1 << 8);										// Update DMA request enable

	*TIM2_CR1 |= (1 << 0);									// Enable TIM1 module: Counter enable -->(CEN=1)


/**FUNCIONES SENOIDALES BASE**/
	int i = 0;
/*1kHz*/
	int sin1khz[SAMPLES];
	for(i=0; i<SAMPLES; i++)
		sin1khz[i] = 127 * sin(2.0*PI*i*1/SAMPLES);
/*2kHz*/
	int sin2khz[SAMPLES];
	for(i=0; i<SAMPLES; i++)
		sin2khz[i] = 127 * sin(2.0*PI*i*2/SAMPLES);
/*4kHz*/
	int sin4khz[SAMPLES];
	for(i=0; i<SAMPLES; i++)
		sin4khz[i] = 127 * sin(2.0*PI*i*4/SAMPLES);
/*8kHz*/
	int sin8khz[SAMPLES];
	for(i=0; i<SAMPLES; i++)
		sin8khz[i] = 127 * sin(2.0*PI*i*8/SAMPLES);

	
	int sumatoria[SAMPLES];								// Vector para las combinaciones de todos los senos

	int volatile a = 0;									// Variables auxiliares para ayudar a realizar la suma
	int volatile b = 0;
	int volatile c = 0;
	int volatile d = 0;

	for(;;){

		int cont = 0;

		if(*IDR &= (1<<12)){
			a = 1;
			cont++;
		}
		else
			a = 0;

		if(*IDR &= (1<<13)){
			b = 1;
			cont++;
		}
		else
			b = 0;

		if(*IDR &= (1<<14)){
			c = 1;
			cont++;
		}
		else
			c = 0;

		if(*IDR &= (1<<15)){
			d = 1;
			cont++;
		}
		else
			d = 0;

		for(i=0; i<SAMPLES; i++)
			sumatoria[i] = a*sin1khz[i] + b*sin2khz[i] + c*sin4khz[i] + d*sin8khz[i];		// Suma de funciones seno
	
		sigma_delta(sumatoria, SAMPLES, cont);												// Conversión por Sigma Delta 

	};

	return 0;
}