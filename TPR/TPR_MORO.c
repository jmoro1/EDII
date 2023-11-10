/*					UNIVERSIDAD NACIONAL DE SAN MARTIN				*/
/*					ELECTRONICA DIGITAL II - TP REGULARIZADOR		*/
/*El objetivo del código es el de replicar el funcionamiento de un piano musical de 4 notas,
consta de 4 botones que son las entradas que representan señales senoidales de 1kHz,2kHz,4kHz y 8kHz respectivamente,
cuando se apreta cada botón la salida es una onda senoidal que es combinacion lineal de las señales de entrada,
Desde el firmware se leen los botones mediante puertos GPIO, la transferencia de datos se hace mediante DMA
utilizando el timer del Cortex como trigger. La señal analógica es convertida por un algoritmo Sigma Delta




*/


#define SRAM_SIZE		((uint32_t) 0x00005000)
#define SRAM_BASE		((uint32_t) 0x20000000)
#define STACKINIT		((interrupt_t)(SRAM_BASE+SRAM_SIZE))

typedef int			   int32_t;
typedef short		   int16_t;
typedef char		   int8_t;
typedef unsigned int   uint32_t;
typedef unsigned short uint16_t;
typedef unsigned char  uint8_t;

/*-------------------------------------------------MEMORY MAPPING------------------------------------------------------
All the registers used in the code are implemented as structs for readability and to avoid unnecesary address operations 
as only the registers base address is hardcoded in each case. Some peripherals like the TIM2 are not
fully implemented but correctly commented for warning.
*/


//**********FLASH******************

typedef struct{ //DECLARO UNA STRUCT PARA ACCEDER A LOS REGISTROS DE CONTROL DE ACCESO A LA FLASH
	volatile uint32_t ACR;     // FLASH ACCESS CONTROL REGISTER
}FLASH_Type;

#define FLASH_BASE_ADDRESS ((uint32_t) 0x40022000) // GPIO_C BASE ADDRESS
#define FLASH ((FLASH_Type *) FLASH_BASE_ADDRESS)  // POINTER TO BASE ADDRESS

//**********GPIO REGISTERS******************

typedef struct{ //DECLARO UNA STRUCT PARA ACCEDER A LOS REGISTROS DE LOS GPIO
    volatile uint32_t CRL; //PORT CONFIGURATION REGISTER LOW (0-7) (OFFSET 0x00)
    volatile uint32_t CRH; //PORT CONFIGURATION REGISTER HIGH (8-15)(OFFSET 0x04) 
    volatile const uint32_t IDR;//INPUT DATA REGISTER(OFFSET 0x08) 
    volatile uint32_t ODR;//OUTPUT DATA REGISTER(OFFSET 0x0C)
    volatile uint32_t BSRR; // BIT SET/RESET REGISTER(OFFSET 0x10)
    volatile uint32_t BRR;//BIT RESET REGISTER(OFFSET 0x14)
    volatile uint32_t LCKR;//PORT CONFIGURATION LOCK REGISTER(OFFSET 0x18)
}GPIO_Type;

#define GPIO_C_BASE_ADDRESS ((uint32_t) 0x40011000) // GPIO_C BASE ADDRESS
#define GPIOB_BASE_ADDRESS ((uint32_t) 0x40010C00) // GPIOB BASE ADDRESS
#define GPIO_B ((GPIO_Type *) GPIOB_BASE_ADDRESS)  // POINTER TO GPIOB BASE ADDRESS
#define GPIO_C ((GPIO_Type *) GPIO_C_BASE_ADDRESS)  // POINTER TO GPIO_C BASE ADDRESS

//**********RCC REGISTERS******************

typedef struct{ //DECLARO UNA STRUCT PARA ACCEDER A LOS REGISTROS DE LOS GPIO
	volatile uint32_t CR;      // CLOCK CONTROL REGISTER
    volatile uint32_t CFGR;    // CLOCK CONFIGURATION REGISTER
    volatile uint32_t CIR;     // CLOCK INTERRUPT REGISTER
    volatile uint32_t APB2RSTR; // APB2 PERIPHERAL RESET REGISTER
    volatile uint32_t APB1RSTR; // APB1 PERIPHERAL RESET REGISTER
    volatile uint32_t AHBENR;  // AHB PERIPHERAL CLOCK ENABLE REGISTER
    volatile uint32_t APB2ENR; // APB2 PERIPHERAL CLOCK ENABLE REGISTER
    volatile uint32_t APB1ENR; // APB1 PERIPHERAL CLOCK ENABLE REGISTER
    volatile uint32_t BDCR;    // BACKUP DOMAIN CONTROL REGISTER
    volatile uint32_t CSR;     // CONTROL/STATUS REGISTER
}RCC_Type;

#define RCC_BASE_ADDRESS ((uint32_t) 0x40021000) // RCC BASE ADDRESS
#define RCC ((RCC_Type *) RCC_BASE_ADDRESS)  // POINTER TO RCC BASE ADDRESS


//**********DMA1 REGISTERS******************

typedef struct {
    volatile uint32_t ISR;      // INTERRUPT STATUS REGISTER
    volatile uint32_t IFCR;     // INTERRUPT FLAG CLEAR REGISTER
    struct {
        volatile uint32_t CCR;   // CHANNEL CONTROL REGISTER
        volatile uint32_t CNDTR; // CHANNEL NUMBER OF DATA TO TRANSFER REGISTER
        volatile uint32_t CPAR;  // CHANNEL PERIPHERAL ADDRESS REGISTER
        volatile uint32_t CMAR;  // CHANNEL MEMORY ADDRESS REGISTER
        volatile uint32_t RESERVED; // RESERVED REGISTER
    } CHN[8];                   // ARRAY OF 8 CHANNELS
} DMA_Type;

#define DMA1_BASE_ADDRESS ((uint32_t) 0x40020000) // DMA1 BASE ADDRESS
#define DMA1 ((DMA_Type *) DMA1_BASE_ADDRESS)  // POINTER TO DMA1 BASE ADDRESS

//**********TIM2 REGISTERS******************

typedef struct {
    volatile uint32_t CR1;   // CONTROL REGISTER 1
    volatile uint32_t CR2;   // CONTROL REGISTER 2
    volatile uint32_t SMCR;  // SLAVE MODE CONTROL REGISTER
    volatile uint32_t DIER;  // DMA/INTERRUPT ENABLE REGISTER
    volatile uint32_t SR;    // STATUS REGISTER
    volatile uint32_t EGR;   // EVENT GENERATION REGISTER
    volatile uint32_t CCMR1; // CAPTURE/COMPARE MODE REGISTER 1
    volatile uint32_t CCMR2; // CAPTURE/COMPARE MODE REGISTER 2
    volatile uint32_t CCER;  // CAPTURE/COMPARE ENABLE REGISTER
    volatile uint32_t CNT;   // COUNTER REGISTER
    volatile uint32_t PSC;   // PRESCALER REGISTER
    volatile uint32_t ARR;   // AUTO-RELOAD REGISTER
    // ... otros registros del TIM2
} TIM2_Type;

#define TIM2_BASE_ADDRESS ((uint32_t) 0x40000000) // TIM2 BASE ADDRESS
#define TIM2 ((TIM2_Type *) TIM2_BASE_ADDRESS)  // POINTER TO BASE ADDRESS




int  main(void);

#define RESOLUTION 15
#define MAXVAL  ((unsigned short) ((1 << RESOLUTION)-1))  //Max value to be represented by RESOLUTION bits
#define MAXHALF ((unsigned short)  (1 << (RESOLUTION-1))) // Half of the max value to be represented by RESOLUTION bits
#define TABSIZE 4096 //Amount of samples
typedef unsigned short pcm_t;

//Sine function samples array
const unsigned short sintab[TABSIZE] = {
	16384, 16409, 16434, 16459, 16484, 16509, 16534, 16559, 16585, 16610, 16635, 16660, 16685, 16710, 16735, 16760, 
	16786, 16811, 16836, 16861, 16886, 16911, 16936, 16961, 16987, 17012, 17037, 17062, 17087, 17112, 17137, 17162, 
	17187, 17212, 17238, 17263, 17288, 17313, 17338, 17363, 17388, 17413, 17438, 17463, 17488, 17514, 17539, 17564, 
	17589, 17614, 17639, 17664, 17689, 17714, 17739, 17764, 17789, 17814, 17839, 17864, 17889, 17914, 17939, 17964, 
	17989, 18014, 18039, 18064, 18089, 18114, 18139, 18164, 18189, 18214, 18239, 18264, 18289, 18314, 18339, 18364, 
	18389, 18414, 18439, 18464, 18489, 18514, 18539, 18563, 18588, 18613, 18638, 18663, 18688, 18713, 18738, 18763, 
	18787, 18812, 18837, 18862, 18887, 18912, 18936, 18961, 18986, 19011, 19036, 19060, 19085, 19110, 19135, 19160, 
	19184, 19209, 19234, 19259, 19283, 19308, 19333, 19358, 19382, 19407, 19432, 19456, 19481, 19506, 19530, 19555, 
	19580, 19604, 19629, 19654, 19678, 19703, 19727, 19752, 19777, 19801, 19826, 19850, 19875, 19899, 19924, 19949, 
	19973, 19998, 20022, 20047, 20071, 20096, 20120, 20144, 20169, 20193, 20218, 20242, 20267, 20291, 20315, 20340, 
	20364, 20389, 20413, 20437, 20462, 20486, 20510, 20535, 20559, 20583, 20608, 20632, 20656, 20680, 20705, 20729, 
	20753, 20777, 20801, 20826, 20850, 20874, 20898, 20922, 20946, 20971, 20995, 21019, 21043, 21067, 21091, 21115, 
	21139, 21163, 21187, 21211, 21235, 21259, 21283, 21307, 21331, 21355, 21379, 21403, 21427, 21451, 21475, 21499, 
	21523, 21546, 21570, 21594, 21618, 21642, 21666, 21689, 21713, 21737, 21761, 21784, 21808, 21832, 21855, 21879, 
	21903, 21926, 21950, 21974, 21997, 22021, 22045, 22068, 22092, 22115, 22139, 22162, 22186, 22209, 22233, 22256, 
	22280, 22303, 22327, 22350, 22373, 22397, 22420, 22443, 22467, 22490, 22513, 22537, 22560, 22583, 22607, 22630, 
	22653, 22676, 22699, 22723, 22746, 22769, 22792, 22815, 22838, 22861, 22884, 22907, 22931, 22954, 22977, 23000, 
	23023, 23046, 23068, 23091, 23114, 23137, 23160, 23183, 23206, 23229, 23252, 23274, 23297, 23320, 23343, 23365, 
	23388, 23411, 23434, 23456, 23479, 23502, 23524, 23547, 23569, 23592, 23614, 23637, 23660, 23682, 23705, 23727, 
	23749, 23772, 23794, 23817, 23839, 23862, 23884, 23906, 23929, 23951, 23973, 23995, 24018, 24040, 24062, 24084, 
	24106, 24129, 24151, 24173, 24195, 24217, 24239, 24261, 24283, 24305, 24327, 24349, 24371, 24393, 24415, 24437, 
	24459, 24481, 24502, 24524, 24546, 24568, 24590, 24611, 24633, 24655, 24676, 24698, 24720, 24741, 24763, 24784, 
	24806, 24828, 24849, 24871, 24892, 24914, 24935, 24956, 24978, 24999, 25021, 25042, 25063, 25085, 25106, 25127, 
	25148, 25170, 25191, 25212, 25233, 25254, 25275, 25296, 25318, 25339, 25360, 25381, 25402, 25423, 25444, 25465, 
	25485, 25506, 25527, 25548, 25569, 25590, 25610, 25631, 25652, 25673, 25693, 25714, 25735, 25755, 25776, 25796, 
	25817, 25838, 25858, 25879, 25899, 25919, 25940, 25960, 25981, 26001, 26021, 26042, 26062, 26082, 26102, 26123, 
	26143, 26163, 26183, 26203, 26223, 26243, 26264, 26284, 26304, 26324, 26344, 26363, 26383, 26403, 26423, 26443, 
	26463, 26483, 26502, 26522, 26542, 26562, 26581, 26601, 26621, 26640, 26660, 26679, 26699, 26718, 26738, 26757, 
	26777, 26796, 26816, 26835, 26854, 26874, 26893, 26912, 26931, 26951, 26970, 26989, 27008, 27027, 27046, 27065, 
	27084, 27103, 27122, 27141, 27160, 27179, 27198, 27217, 27236, 27255, 27273, 27292, 27311, 27330, 27348, 27367, 
	27386, 27404, 27423, 27441, 27460, 27478, 27497, 27515, 27534, 27552, 27571, 27589, 27607, 27626, 27644, 27662, 
	27680, 27698, 27717, 27735, 27753, 27771, 27789, 27807, 27825, 27843, 27861, 27879, 27897, 27915, 27932, 27950, 
	27968, 27986, 28004, 28021, 28039, 28057, 28074, 28092, 28109, 28127, 28144, 28162, 28179, 28197, 28214, 28231, 
	28249, 28266, 28283, 28301, 28318, 28335, 28352, 28369, 28387, 28404, 28421, 28438, 28455, 28472, 28489, 28506, 
	28523, 28539, 28556, 28573, 28590, 28607, 28623, 28640, 28657, 28673, 28690, 28706, 28723, 28739, 28756, 28772, 
	28789, 28805, 28822, 28838, 28854, 28871, 28887, 28903, 28919, 28935, 28952, 28968, 28984, 29000, 29016, 29032, 
	29048, 29064, 29080, 29095, 29111, 29127, 29143, 29159, 29174, 29190, 29206, 29221, 29237, 29252, 29268, 29284, 
	29299, 29314, 29330, 29345, 29361, 29376, 29391, 29406, 29422, 29437, 29452, 29467, 29482, 29497, 29512, 29527, 
	29542, 29557, 29572, 29587, 29602, 29617, 29632, 29646, 29661, 29676, 29691, 29705, 29720, 29734, 29749, 29764, 
	29778, 29792, 29807, 29821, 29836, 29850, 29864, 29879, 29893, 29907, 29921, 29935, 29949, 29963, 29977, 29991, 
	30005, 30019, 30033, 30047, 30061, 30075, 30089, 30102, 30116, 30130, 30143, 30157, 30171, 30184, 30198, 30211, 
	30225, 30238, 30252, 30265, 30278, 30292, 30305, 30318, 30331, 30344, 30358, 30371, 30384, 30397, 30410, 30423, 
	30436, 30449, 30461, 30474, 30487, 30500, 30513, 30525, 30538, 30551, 30563, 30576, 30588, 30601, 30613, 30626, 
	30638, 30651, 30663, 30675, 30687, 30700, 30712, 30724, 30736, 30748, 30760, 30772, 30784, 30796, 30808, 30820, 
	30832, 30844, 30856, 30867, 30879, 30891, 30902, 30914, 30926, 30937, 30949, 30960, 30972, 30983, 30995, 31006, 
	31017, 31028, 31040, 31051, 31062, 31073, 31084, 31095, 31106, 31117, 31128, 31139, 31150, 31161, 31172, 31183, 
	31194, 31204, 31215, 31226, 31236, 31247, 31257, 31268, 31278, 31289, 31299, 31310, 31320, 31330, 31341, 31351, 
	31361, 31371, 31381, 31391, 31401, 31411, 31421, 31431, 31441, 31451, 31461, 31471, 31481, 31490, 31500, 31510, 
	31519, 31529, 31539, 31548, 31558, 31567, 31576, 31586, 31595, 31605, 31614, 31623, 31632, 31641, 31651, 31660, 
	31669, 31678, 31687, 31696, 31705, 31713, 31722, 31731, 31740, 31749, 31757, 31766, 31775, 31783, 31792, 31800, 
	31809, 31817, 31826, 31834, 31842, 31851, 31859, 31867, 31875, 31884, 31892, 31900, 31908, 31916, 31924, 31932, 
	31940, 31947, 31955, 31963, 31971, 31979, 31986, 31994, 32002, 32009, 32017, 32024, 32032, 32039, 32046, 32054, 
	32061, 32068, 32076, 32083, 32090, 32097, 32104, 32111, 32118, 32125, 32132, 32139, 32146, 32153, 32160, 32166, 
	32173, 32180, 32186, 32193, 32200, 32206, 32213, 32219, 32225, 32232, 32238, 32245, 32251, 32257, 32263, 32269, 
	32276, 32282, 32288, 32294, 32300, 32306, 32311, 32317, 32323, 32329, 32335, 32340, 32346, 32352, 32357, 32363, 
	32368, 32374, 32379, 32385, 32390, 32395, 32401, 32406, 32411, 32416, 32422, 32427, 32432, 32437, 32442, 32447, 
	32452, 32457, 32461, 32466, 32471, 32476, 32480, 32485, 32490, 32494, 32499, 32503, 32508, 32512, 32517, 32521, 
	32525, 32530, 32534, 32538, 32542, 32546, 32550, 32554, 32558, 32562, 32566, 32570, 32574, 32578, 32582, 32585, 
	32589, 32593, 32596, 32600, 32604, 32607, 32611, 32614, 32617, 32621, 32624, 32627, 32631, 32634, 32637, 32640, 
	32643, 32646, 32649, 32652, 32655, 32658, 32661, 32664, 32667, 32669, 32672, 32675, 32677, 32680, 32683, 32685, 
	32688, 32690, 32692, 32695, 32697, 32699, 32702, 32704, 32706, 32708, 32710, 32712, 32714, 32716, 32718, 32720, 
	32722, 32724, 32726, 32727, 32729, 32731, 32733, 32734, 32736, 32737, 32739, 32740, 32742, 32743, 32744, 32746, 
	32747, 32748, 32749, 32750, 32751, 32752, 32753, 32754, 32755, 32756, 32757, 32758, 32759, 32760, 32760, 32761, 
	32762, 32762, 32763, 32763, 32764, 32764, 32765, 32765, 32765, 32766, 32766, 32766, 32766, 32766, 32766, 32766, 
	32767, 32766, 32766, 32766, 32766, 32766, 32766, 32766, 32765, 32765, 32765, 32764, 32764, 32763, 32763, 32762, 
	32762, 32761, 32760, 32760, 32759, 32758, 32757, 32756, 32755, 32754, 32753, 32752, 32751, 32750, 32749, 32748, 
	32747, 32746, 32744, 32743, 32742, 32740, 32739, 32737, 32736, 32734, 32733, 32731, 32729, 32727, 32726, 32724, 
	32722, 32720, 32718, 32716, 32714, 32712, 32710, 32708, 32706, 32704, 32702, 32699, 32697, 32695, 32692, 32690, 
	32688, 32685, 32683, 32680, 32677, 32675, 32672, 32669, 32667, 32664, 32661, 32658, 32655, 32652, 32649, 32646, 
	32643, 32640, 32637, 32634, 32631, 32627, 32624, 32621, 32617, 32614, 32611, 32607, 32604, 32600, 32596, 32593, 
	32589, 32585, 32582, 32578, 32574, 32570, 32566, 32562, 32558, 32554, 32550, 32546, 32542, 32538, 32534, 32530, 
	32525, 32521, 32517, 32512, 32508, 32503, 32499, 32494, 32490, 32485, 32480, 32476, 32471, 32466, 32461, 32457, 
	32452, 32447, 32442, 32437, 32432, 32427, 32422, 32416, 32411, 32406, 32401, 32395, 32390, 32385, 32379, 32374, 
	32368, 32363, 32357, 32352, 32346, 32340, 32335, 32329, 32323, 32317, 32311, 32306, 32300, 32294, 32288, 32282, 
	32276, 32269, 32263, 32257, 32251, 32245, 32238, 32232, 32225, 32219, 32213, 32206, 32200, 32193, 32186, 32180, 
	32173, 32166, 32160, 32153, 32146, 32139, 32132, 32125, 32118, 32111, 32104, 32097, 32090, 32083, 32076, 32068, 
	32061, 32054, 32046, 32039, 32032, 32024, 32017, 32009, 32002, 31994, 31986, 31979, 31971, 31963, 31955, 31947, 
	31940, 31932, 31924, 31916, 31908, 31900, 31892, 31884, 31875, 31867, 31859, 31851, 31842, 31834, 31826, 31817, 
	31809, 31800, 31792, 31783, 31775, 31766, 31757, 31749, 31740, 31731, 31722, 31713, 31705, 31696, 31687, 31678, 
	31669, 31660, 31651, 31641, 31632, 31623, 31614, 31605, 31595, 31586, 31576, 31567, 31558, 31548, 31539, 31529, 
	31519, 31510, 31500, 31490, 31481, 31471, 31461, 31451, 31441, 31431, 31421, 31411, 31401, 31391, 31381, 31371, 
	31361, 31351, 31341, 31330, 31320, 31310, 31299, 31289, 31278, 31268, 31257, 31247, 31236, 31226, 31215, 31204, 
	31194, 31183, 31172, 31161, 31150, 31139, 31128, 31117, 31106, 31095, 31084, 31073, 31062, 31051, 31040, 31028, 
	31017, 31006, 30995, 30983, 30972, 30960, 30949, 30937, 30926, 30914, 30902, 30891, 30879, 30867, 30856, 30844, 
	30832, 30820, 30808, 30796, 30784, 30772, 30760, 30748, 30736, 30724, 30712, 30700, 30687, 30675, 30663, 30651, 
	30638, 30626, 30613, 30601, 30588, 30576, 30563, 30551, 30538, 30525, 30513, 30500, 30487, 30474, 30461, 30449, 
	30436, 30423, 30410, 30397, 30384, 30371, 30358, 30344, 30331, 30318, 30305, 30292, 30278, 30265, 30252, 30238, 
	30225, 30211, 30198, 30184, 30171, 30157, 30143, 30130, 30116, 30102, 30089, 30075, 30061, 30047, 30033, 30019, 
	30005, 29991, 29977, 29963, 29949, 29935, 29921, 29907, 29893, 29879, 29864, 29850, 29836, 29821, 29807, 29792, 
	29778, 29764, 29749, 29734, 29720, 29705, 29691, 29676, 29661, 29646, 29632, 29617, 29602, 29587, 29572, 29557, 
	29542, 29527, 29512, 29497, 29482, 29467, 29452, 29437, 29422, 29406, 29391, 29376, 29361, 29345, 29330, 29314, 
	29299, 29284, 29268, 29252, 29237, 29221, 29206, 29190, 29174, 29159, 29143, 29127, 29111, 29095, 29080, 29064, 
	29048, 29032, 29016, 29000, 28984, 28968, 28952, 28935, 28919, 28903, 28887, 28871, 28854, 28838, 28822, 28805, 
	28789, 28772, 28756, 28739, 28723, 28706, 28690, 28673, 28657, 28640, 28623, 28607, 28590, 28573, 28556, 28539, 
	28523, 28506, 28489, 28472, 28455, 28438, 28421, 28404, 28387, 28369, 28352, 28335, 28318, 28301, 28283, 28266, 
	28249, 28231, 28214, 28197, 28179, 28162, 28144, 28127, 28109, 28092, 28074, 28057, 28039, 28021, 28004, 27986, 
	27968, 27950, 27932, 27915, 27897, 27879, 27861, 27843, 27825, 27807, 27789, 27771, 27753, 27735, 27717, 27698, 
	27680, 27662, 27644, 27626, 27607, 27589, 27571, 27552, 27534, 27515, 27497, 27478, 27460, 27441, 27423, 27404, 
	27386, 27367, 27348, 27330, 27311, 27292, 27273, 27255, 27236, 27217, 27198, 27179, 27160, 27141, 27122, 27103, 
	27084, 27065, 27046, 27027, 27008, 26989, 26970, 26951, 26931, 26912, 26893, 26874, 26854, 26835, 26816, 26796, 
	26777, 26757, 26738, 26718, 26699, 26679, 26660, 26640, 26621, 26601, 26581, 26562, 26542, 26522, 26502, 26483, 
	26463, 26443, 26423, 26403, 26383, 26363, 26344, 26324, 26304, 26284, 26264, 26243, 26223, 26203, 26183, 26163, 
	26143, 26123, 26102, 26082, 26062, 26042, 26021, 26001, 25981, 25960, 25940, 25919, 25899, 25879, 25858, 25838, 
	25817, 25796, 25776, 25755, 25735, 25714, 25693, 25673, 25652, 25631, 25610, 25590, 25569, 25548, 25527, 25506, 
	25485, 25465, 25444, 25423, 25402, 25381, 25360, 25339, 25318, 25296, 25275, 25254, 25233, 25212, 25191, 25170, 
	25148, 25127, 25106, 25085, 25063, 25042, 25021, 24999, 24978, 24956, 24935, 24914, 24892, 24871, 24849, 24828, 
	24806, 24784, 24763, 24741, 24720, 24698, 24676, 24655, 24633, 24611, 24590, 24568, 24546, 24524, 24502, 24481, 
	24459, 24437, 24415, 24393, 24371, 24349, 24327, 24305, 24283, 24261, 24239, 24217, 24195, 24173, 24151, 24129, 
	24106, 24084, 24062, 24040, 24018, 23995, 23973, 23951, 23929, 23906, 23884, 23862, 23839, 23817, 23794, 23772, 
	23749, 23727, 23705, 23682, 23660, 23637, 23614, 23592, 23569, 23547, 23524, 23502, 23479, 23456, 23434, 23411, 
	23388, 23365, 23343, 23320, 23297, 23274, 23252, 23229, 23206, 23183, 23160, 23137, 23114, 23091, 23068, 23046, 
	23023, 23000, 22977, 22954, 22931, 22907, 22884, 22861, 22838, 22815, 22792, 22769, 22746, 22723, 22699, 22676, 
	22653, 22630, 22607, 22583, 22560, 22537, 22513, 22490, 22467, 22443, 22420, 22397, 22373, 22350, 22327, 22303, 
	22280, 22256, 22233, 22209, 22186, 22162, 22139, 22115, 22092, 22068, 22045, 22021, 21997, 21974, 21950, 21926, 
	21903, 21879, 21855, 21832, 21808, 21784, 21761, 21737, 21713, 21689, 21666, 21642, 21618, 21594, 21570, 21546, 
	21523, 21499, 21475, 21451, 21427, 21403, 21379, 21355, 21331, 21307, 21283, 21259, 21235, 21211, 21187, 21163, 
	21139, 21115, 21091, 21067, 21043, 21019, 20995, 20971, 20946, 20922, 20898, 20874, 20850, 20826, 20801, 20777, 
	20753, 20729, 20705, 20680, 20656, 20632, 20608, 20583, 20559, 20535, 20510, 20486, 20462, 20437, 20413, 20389, 
	20364, 20340, 20315, 20291, 20267, 20242, 20218, 20193, 20169, 20144, 20120, 20096, 20071, 20047, 20022, 19998, 
	19973, 19949, 19924, 19899, 19875, 19850, 19826, 19801, 19777, 19752, 19727, 19703, 19678, 19654, 19629, 19604, 
	19580, 19555, 19530, 19506, 19481, 19456, 19432, 19407, 19382, 19358, 19333, 19308, 19283, 19259, 19234, 19209, 
	19184, 19160, 19135, 19110, 19085, 19060, 19036, 19011, 18986, 18961, 18936, 18912, 18887, 18862, 18837, 18812, 
	18787, 18763, 18738, 18713, 18688, 18663, 18638, 18613, 18588, 18563, 18539, 18514, 18489, 18464, 18439, 18414, 
	18389, 18364, 18339, 18314, 18289, 18264, 18239, 18214, 18189, 18164, 18139, 18114, 18089, 18064, 18039, 18014, 
	17989, 17964, 17939, 17914, 17889, 17864, 17839, 17814, 17789, 17764, 17739, 17714, 17689, 17664, 17639, 17614, 
	17589, 17564, 17539, 17514, 17488, 17463, 17438, 17413, 17388, 17363, 17338, 17313, 17288, 17263, 17238, 17212, 
	17187, 17162, 17137, 17112, 17087, 17062, 17037, 17012, 16987, 16961, 16936, 16911, 16886, 16861, 16836, 16811, 
	16786, 16760, 16735, 16710, 16685, 16660, 16635, 16610, 16585, 16559, 16534, 16509, 16484, 16459, 16434, 16409, 
	16384, 16358, 16333, 16308, 16283, 16258, 16233, 16208, 16182, 16157, 16132, 16107, 16082, 16057, 16032, 16007, 
	15981, 15956, 15931, 15906, 15881, 15856, 15831, 15806, 15780, 15755, 15730, 15705, 15680, 15655, 15630, 15605, 
	15580, 15555, 15529, 15504, 15479, 15454, 15429, 15404, 15379, 15354, 15329, 15304, 15279, 15253, 15228, 15203, 
	15178, 15153, 15128, 15103, 15078, 15053, 15028, 15003, 14978, 14953, 14928, 14903, 14878, 14853, 14828, 14803, 
	14778, 14753, 14728, 14703, 14678, 14653, 14628, 14603, 14578, 14553, 14528, 14503, 14478, 14453, 14428, 14403, 
	14378, 14353, 14328, 14303, 14278, 14253, 14228, 14204, 14179, 14154, 14129, 14104, 14079, 14054, 14029, 14004, 
	13980, 13955, 13930, 13905, 13880, 13855, 13831, 13806, 13781, 13756, 13731, 13707, 13682, 13657, 13632, 13607, 
	13583, 13558, 13533, 13508, 13484, 13459, 13434, 13409, 13385, 13360, 13335, 13311, 13286, 13261, 13237, 13212, 
	13187, 13163, 13138, 13113, 13089, 13064, 13040, 13015, 12990, 12966, 12941, 12917, 12892, 12868, 12843, 12818, 
	12794, 12769, 12745, 12720, 12696, 12671, 12647, 12623, 12598, 12574, 12549, 12525, 12500, 12476, 12452, 12427, 
	12403, 12378, 12354, 12330, 12305, 12281, 12257, 12232, 12208, 12184, 12159, 12135, 12111, 12087, 12062, 12038, 
	12014, 11990, 11966, 11941, 11917, 11893, 11869, 11845, 11821, 11796, 11772, 11748, 11724, 11700, 11676, 11652, 
	11628, 11604, 11580, 11556, 11532, 11508, 11484, 11460, 11436, 11412, 11388, 11364, 11340, 11316, 11292, 11268, 
	11244, 11221, 11197, 11173, 11149, 11125, 11101, 11078, 11054, 11030, 11006, 10983, 10959, 10935, 10912, 10888, 
	10864, 10841, 10817, 10793, 10770, 10746, 10722, 10699, 10675, 10652, 10628, 10605, 10581, 10558, 10534, 10511, 
	10487, 10464, 10440, 10417, 10394, 10370, 10347, 10324, 10300, 10277, 10254, 10230, 10207, 10184, 10160, 10137, 
	10114, 10091, 10068, 10044, 10021,  9998,  9975,  9952,  9929,  9906,  9883,  9860,  9836,  9813,  9790,  9767, 
	 9744,  9721,  9699,  9676,  9653,  9630,  9607,  9584,  9561,  9538,  9515,  9493,  9470,  9447,  9424,  9402, 
	 9379,  9356,  9333,  9311,  9288,  9265,  9243,  9220,  9198,  9175,  9153,  9130,  9107,  9085,  9062,  9040, 
	 9018,  8995,  8973,  8950,  8928,  8905,  8883,  8861,  8838,  8816,  8794,  8772,  8749,  8727,  8705,  8683, 
	 8661,  8638,  8616,  8594,  8572,  8550,  8528,  8506,  8484,  8462,  8440,  8418,  8396,  8374,  8352,  8330, 
	 8308,  8286,  8265,  8243,  8221,  8199,  8177,  8156,  8134,  8112,  8091,  8069,  8047,  8026,  8004,  7983, 
	 7961,  7939,  7918,  7896,  7875,  7853,  7832,  7811,  7789,  7768,  7746,  7725,  7704,  7682,  7661,  7640, 
	 7619,  7597,  7576,  7555,  7534,  7513,  7492,  7471,  7449,  7428,  7407,  7386,  7365,  7344,  7323,  7302, 
	 7282,  7261,  7240,  7219,  7198,  7177,  7157,  7136,  7115,  7094,  7074,  7053,  7032,  7012,  6991,  6971, 
	 6950,  6929,  6909,  6888,  6868,  6848,  6827,  6807,  6786,  6766,  6746,  6725,  6705,  6685,  6665,  6644, 
	 6624,  6604,  6584,  6564,  6544,  6524,  6503,  6483,  6463,  6443,  6423,  6404,  6384,  6364,  6344,  6324, 
	 6304,  6284,  6265,  6245,  6225,  6205,  6186,  6166,  6146,  6127,  6107,  6088,  6068,  6049,  6029,  6010, 
	 5990,  5971,  5951,  5932,  5913,  5893,  5874,  5855,  5836,  5816,  5797,  5778,  5759,  5740,  5721,  5702, 
	 5683,  5664,  5645,  5626,  5607,  5588,  5569,  5550,  5531,  5512,  5494,  5475,  5456,  5437,  5419,  5400, 
	 5381,  5363,  5344,  5326,  5307,  5289,  5270,  5252,  5233,  5215,  5196,  5178,  5160,  5141,  5123,  5105, 
	 5087,  5069,  5050,  5032,  5014,  4996,  4978,  4960,  4942,  4924,  4906,  4888,  4870,  4852,  4835,  4817, 
	 4799,  4781,  4763,  4746,  4728,  4710,  4693,  4675,  4658,  4640,  4623,  4605,  4588,  4570,  4553,  4536, 
	 4518,  4501,  4484,  4466,  4449,  4432,  4415,  4398,  4380,  4363,  4346,  4329,  4312,  4295,  4278,  4261, 
	 4244,  4228,  4211,  4194,  4177,  4160,  4144,  4127,  4110,  4094,  4077,  4061,  4044,  4028,  4011,  3995, 
	 3978,  3962,  3945,  3929,  3913,  3896,  3880,  3864,  3848,  3832,  3815,  3799,  3783,  3767,  3751,  3735, 
	 3719,  3703,  3687,  3672,  3656,  3640,  3624,  3608,  3593,  3577,  3561,  3546,  3530,  3515,  3499,  3483, 
	 3468,  3453,  3437,  3422,  3406,  3391,  3376,  3361,  3345,  3330,  3315,  3300,  3285,  3270,  3255,  3240, 
	 3225,  3210,  3195,  3180,  3165,  3150,  3135,  3121,  3106,  3091,  3076,  3062,  3047,  3033,  3018,  3003, 
	 2989,  2975,  2960,  2946,  2931,  2917,  2903,  2888,  2874,  2860,  2846,  2832,  2818,  2804,  2790,  2776, 
	 2762,  2748,  2734,  2720,  2706,  2692,  2678,  2665,  2651,  2637,  2624,  2610,  2596,  2583,  2569,  2556, 
	 2542,  2529,  2515,  2502,  2489,  2475,  2462,  2449,  2436,  2423,  2409,  2396,  2383,  2370,  2357,  2344, 
	 2331,  2318,  2306,  2293,  2280,  2267,  2254,  2242,  2229,  2216,  2204,  2191,  2179,  2166,  2154,  2141, 
	 2129,  2116,  2104,  2092,  2080,  2067,  2055,  2043,  2031,  2019,  2007,  1995,  1983,  1971,  1959,  1947, 
	 1935,  1923,  1911,  1900,  1888,  1876,  1865,  1853,  1841,  1830,  1818,  1807,  1795,  1784,  1772,  1761, 
	 1750,  1739,  1727,  1716,  1705,  1694,  1683,  1672,  1661,  1650,  1639,  1628,  1617,  1606,  1595,  1584, 
	 1573,  1563,  1552,  1541,  1531,  1520,  1510,  1499,  1489,  1478,  1468,  1457,  1447,  1437,  1426,  1416, 
	 1406,  1396,  1386,  1376,  1366,  1356,  1346,  1336,  1326,  1316,  1306,  1296,  1286,  1277,  1267,  1257, 
	 1248,  1238,  1228,  1219,  1209,  1200,  1191,  1181,  1172,  1162,  1153,  1144,  1135,  1126,  1116,  1107, 
	 1098,  1089,  1080,  1071,  1062,  1054,  1045,  1036,  1027,  1018,  1010,  1001,   992,   984,   975,   967, 
	  958,   950,   941,   933,   925,   916,   908,   900,   892,   883,   875,   867,   859,   851,   843,   835, 
	  827,   820,   812,   804,   796,   788,   781,   773,   765,   758,   750,   743,   735,   728,   721,   713, 
	  706,   699,   691,   684,   677,   670,   663,   656,   649,   642,   635,   628,   621,   614,   607,   601, 
	  594,   587,   581,   574,   567,   561,   554,   548,   542,   535,   529,   522,   516,   510,   504,   498, 
	  491,   485,   479,   473,   467,   461,   456,   450,   444,   438,   432,   427,   421,   415,   410,   404, 
	  399,   393,   388,   382,   377,   372,   366,   361,   356,   351,   345,   340,   335,   330,   325,   320, 
	  315,   310,   306,   301,   296,   291,   287,   282,   277,   273,   268,   264,   259,   255,   250,   246, 
	  242,   237,   233,   229,   225,   221,   217,   213,   209,   205,   201,   197,   193,   189,   185,   182, 
	  178,   174,   171,   167,   163,   160,   156,   153,   150,   146,   143,   140,   136,   133,   130,   127, 
	  124,   121,   118,   115,   112,   109,   106,   103,   100,    98,    95,    92,    90,    87,    84,    82, 
	   79,    77,    75,    72,    70,    68,    65,    63,    61,    59,    57,    55,    53,    51,    49,    47, 
	   45,    43,    41,    40,    38,    36,    34,    33,    31,    30,    28,    27,    25,    24,    23,    21, 
	   20,    19,    18,    17,    16,    15,    14,    13,    12,    11,    10,     9,     8,     7,     7,     6, 
	    5,     5,     4,     4,     3,     3,     2,     2,     2,     1,     1,     1,     1,     1,     1,     1, 
	    1,     1,     1,     1,     1,     1,     1,     1,     2,     2,     2,     3,     3,     4,     4,     5, 
	    5,     6,     7,     7,     8,     9,    10,    11,    12,    13,    14,    15,    16,    17,    18,    19, 
	   20,    21,    23,    24,    25,    27,    28,    30,    31,    33,    34,    36,    38,    40,    41,    43, 
	   45,    47,    49,    51,    53,    55,    57,    59,    61,    63,    65,    68,    70,    72,    75,    77, 
	   79,    82,    84,    87,    90,    92,    95,    98,   100,   103,   106,   109,   112,   115,   118,   121, 
	  124,   127,   130,   133,   136,   140,   143,   146,   150,   153,   156,   160,   163,   167,   171,   174, 
	  178,   182,   185,   189,   193,   197,   201,   205,   209,   213,   217,   221,   225,   229,   233,   237, 
	  242,   246,   250,   255,   259,   264,   268,   273,   277,   282,   287,   291,   296,   301,   306,   310, 
	  315,   320,   325,   330,   335,   340,   345,   351,   356,   361,   366,   372,   377,   382,   388,   393, 
	  399,   404,   410,   415,   421,   427,   432,   438,   444,   450,   456,   461,   467,   473,   479,   485, 
	  491,   498,   504,   510,   516,   522,   529,   535,   542,   548,   554,   561,   567,   574,   581,   587, 
	  594,   601,   607,   614,   621,   628,   635,   642,   649,   656,   663,   670,   677,   684,   691,   699, 
	  706,   713,   721,   728,   735,   743,   750,   758,   765,   773,   781,   788,   796,   804,   812,   820, 
	  827,   835,   843,   851,   859,   867,   875,   883,   892,   900,   908,   916,   925,   933,   941,   950, 
	  958,   967,   975,   984,   992,  1001,  1010,  1018,  1027,  1036,  1045,  1054,  1062,  1071,  1080,  1089, 
	 1098,  1107,  1116,  1126,  1135,  1144,  1153,  1162,  1172,  1181,  1191,  1200,  1209,  1219,  1228,  1238, 
	 1248,  1257,  1267,  1277,  1286,  1296,  1306,  1316,  1326,  1336,  1346,  1356,  1366,  1376,  1386,  1396, 
	 1406,  1416,  1426,  1437,  1447,  1457,  1468,  1478,  1489,  1499,  1510,  1520,  1531,  1541,  1552,  1563, 
	 1573,  1584,  1595,  1606,  1617,  1628,  1639,  1650,  1661,  1672,  1683,  1694,  1705,  1716,  1727,  1739, 
	 1750,  1761,  1772,  1784,  1795,  1807,  1818,  1830,  1841,  1853,  1865,  1876,  1888,  1900,  1911,  1923, 
	 1935,  1947,  1959,  1971,  1983,  1995,  2007,  2019,  2031,  2043,  2055,  2067,  2080,  2092,  2104,  2116, 
	 2129,  2141,  2154,  2166,  2179,  2191,  2204,  2216,  2229,  2242,  2254,  2267,  2280,  2293,  2306,  2318, 
	 2331,  2344,  2357,  2370,  2383,  2396,  2409,  2423,  2436,  2449,  2462,  2475,  2489,  2502,  2515,  2529, 
	 2542,  2556,  2569,  2583,  2596,  2610,  2624,  2637,  2651,  2665,  2678,  2692,  2706,  2720,  2734,  2748, 
	 2762,  2776,  2790,  2804,  2818,  2832,  2846,  2860,  2874,  2888,  2903,  2917,  2931,  2946,  2960,  2975, 
	 2989,  3003,  3018,  3033,  3047,  3062,  3076,  3091,  3106,  3121,  3135,  3150,  3165,  3180,  3195,  3210, 
	 3225,  3240,  3255,  3270,  3285,  3300,  3315,  3330,  3345,  3361,  3376,  3391,  3406,  3422,  3437,  3453, 
	 3468,  3483,  3499,  3515,  3530,  3546,  3561,  3577,  3593,  3608,  3624,  3640,  3656,  3672,  3687,  3703, 
	 3719,  3735,  3751,  3767,  3783,  3799,  3815,  3832,  3848,  3864,  3880,  3896,  3913,  3929,  3945,  3962, 
	 3978,  3995,  4011,  4028,  4044,  4061,  4077,  4094,  4110,  4127,  4144,  4160,  4177,  4194,  4211,  4228, 
	 4244,  4261,  4278,  4295,  4312,  4329,  4346,  4363,  4380,  4398,  4415,  4432,  4449,  4466,  4484,  4501, 
	 4518,  4536,  4553,  4570,  4588,  4605,  4623,  4640,  4658,  4675,  4693,  4710,  4728,  4746,  4763,  4781, 
	 4799,  4817,  4835,  4852,  4870,  4888,  4906,  4924,  4942,  4960,  4978,  4996,  5014,  5032,  5050,  5069, 
	 5087,  5105,  5123,  5141,  5160,  5178,  5196,  5215,  5233,  5252,  5270,  5289,  5307,  5326,  5344,  5363, 
	 5381,  5400,  5419,  5437,  5456,  5475,  5494,  5512,  5531,  5550,  5569,  5588,  5607,  5626,  5645,  5664, 
	 5683,  5702,  5721,  5740,  5759,  5778,  5797,  5816,  5836,  5855,  5874,  5893,  5913,  5932,  5951,  5971, 
	 5990,  6010,  6029,  6049,  6068,  6088,  6107,  6127,  6146,  6166,  6186,  6205,  6225,  6245,  6265,  6284, 
	 6304,  6324,  6344,  6364,  6384,  6404,  6423,  6443,  6463,  6483,  6503,  6524,  6544,  6564,  6584,  6604, 
	 6624,  6644,  6665,  6685,  6705,  6725,  6746,  6766,  6786,  6807,  6827,  6848,  6868,  6888,  6909,  6929, 
	 6950,  6971,  6991,  7012,  7032,  7053,  7074,  7094,  7115,  7136,  7157,  7177,  7198,  7219,  7240,  7261, 
	 7282,  7302,  7323,  7344,  7365,  7386,  7407,  7428,  7449,  7471,  7492,  7513,  7534,  7555,  7576,  7597, 
	 7619,  7640,  7661,  7682,  7704,  7725,  7746,  7768,  7789,  7811,  7832,  7853,  7875,  7896,  7918,  7939, 
	 7961,  7983,  8004,  8026,  8047,  8069,  8091,  8112,  8134,  8156,  8177,  8199,  8221,  8243,  8265,  8286, 
	 8308,  8330,  8352,  8374,  8396,  8418,  8440,  8462,  8484,  8506,  8528,  8550,  8572,  8594,  8616,  8638, 
	 8661,  8683,  8705,  8727,  8749,  8772,  8794,  8816,  8838,  8861,  8883,  8905,  8928,  8950,  8973,  8995, 
	 9018,  9040,  9062,  9085,  9107,  9130,  9153,  9175,  9198,  9220,  9243,  9265,  9288,  9311,  9333,  9356, 
	 9379,  9402,  9424,  9447,  9470,  9493,  9515,  9538,  9561,  9584,  9607,  9630,  9653,  9676,  9699,  9721, 
	 9744,  9767,  9790,  9813,  9836,  9860,  9883,  9906,  9929,  9952,  9975,  9998, 10021, 10044, 10068, 10091, 
	10114, 10137, 10160, 10184, 10207, 10230, 10254, 10277, 10300, 10324, 10347, 10370, 10394, 10417, 10440, 10464, 
	10487, 10511, 10534, 10558, 10581, 10605, 10628, 10652, 10675, 10699, 10722, 10746, 10770, 10793, 10817, 10841, 
	10864, 10888, 10912, 10935, 10959, 10983, 11006, 11030, 11054, 11078, 11101, 11125, 11149, 11173, 11197, 11221, 
	11244, 11268, 11292, 11316, 11340, 11364, 11388, 11412, 11436, 11460, 11484, 11508, 11532, 11556, 11580, 11604, 
	11628, 11652, 11676, 11700, 11724, 11748, 11772, 11796, 11821, 11845, 11869, 11893, 11917, 11941, 11966, 11990, 
	12014, 12038, 12062, 12087, 12111, 12135, 12159, 12184, 12208, 12232, 12257, 12281, 12305, 12330, 12354, 12378, 
	12403, 12427, 12452, 12476, 12500, 12525, 12549, 12574, 12598, 12623, 12647, 12671, 12696, 12720, 12745, 12769, 
	12794, 12818, 12843, 12868, 12892, 12917, 12941, 12966, 12990, 13015, 13040, 13064, 13089, 13113, 13138, 13163, 
	13187, 13212, 13237, 13261, 13286, 13311, 13335, 13360, 13385, 13409, 13434, 13459, 13484, 13508, 13533, 13558, 
	13583, 13607, 13632, 13657, 13682, 13707, 13731, 13756, 13781, 13806, 13831, 13855, 13880, 13905, 13930, 13955, 
	13980, 14004, 14029, 14054, 14079, 14104, 14129, 14154, 14179, 14204, 14228, 14253, 14278, 14303, 14328, 14353, 
	14378, 14403, 14428, 14453, 14478, 14503, 14528, 14553, 14578, 14603, 14628, 14653, 14678, 14703, 14728, 14753, 
	14778, 14803, 14828, 14853, 14878, 14903, 14928, 14953, 14978, 15003, 15028, 15053, 15078, 15103, 15128, 15153, 
	15178, 15203, 15228, 15253, 15279, 15304, 15329, 15354, 15379, 15404, 15429, 15454, 15479, 15504, 15529, 15555, 
	15580, 15605, 15630, 15655, 15680, 15705, 15730, 15755, 15780, 15806, 15831, 15856, 15881, 15906, 15931, 15956, 
	15981, 16007, 16032, 16057, 16082, 16107, 16132, 16157, 16182, 16208, 16233, 16258, 16283, 16308, 16333, 16358};
//Sine function peak values for each of 2^4 combinations.
const unsigned peak[16] = {     0,  32767,  32767,  61604,  32767,  64357,  61604,  85727,  32767,  65223,  64357,  94297,  61604,  93930,  85727, 106398};
//Array of data to be sent for output
volatile uint16_t data[TABSIZE];

//Sigma Delta ADC function
unsigned char sosd (pcm_t pcm, int acc[2], pcm_t fdbk)
{
	int diff1;
	int diff2;

	diff1 =   fdbk;
	diff1 +=  pcm;
	diff1 >>= 1;

	acc[0] += diff1-MAXHALF;

	diff2  = acc[0];
	diff2 += MAXHALF;
	diff2 += fdbk;
	diff2 >>= 1;

	acc[1] += diff2;
	acc[1] -= MAXHALF;

	return (acc[1] > 0) ? 1 : 0;
}

int main(void)
{
	
	
	// PCLK code
	RCC->CR |= (1 << 16);						// Enable HSE
	while (!(RCC->CR & (1 << 17)));				// Wait for HSE is locked

	RCC->CR   &= ~(1 << 24);					// Disable PLL
	RCC->CFGR |= (0b0111 << 18);				// Set PLLMULL to 9. Set PLL output clock to 72 Mhz
	RCC->CFGR |=  (1 << 16);					// Select HSE as the PLL source clock
	RCC->CR   |=  (1 << 24);					// Enable PLL
	while (!(RCC->CR & (1 << 25)));				// Wait for PLL to lock

	FLASH->ACR |= (0b010 << 0);					// Set FLASH WAIT STATE to 2
	RCC->CFGR  |= (0b0000 << 4);				// Set AHB HPRE division to 1. Set AHB clock to 72 Mhz
	RCC->CFGR  |= (0b100 << 8);					// Set APB1 PPRE1 division to 2. Set AHB clock to 36 Mhz

	RCC->CFGR |= (0b10 << 0);					// Select PLL clock as the system clock
	while (!(RCC->CFGR & (0b10 << 2)));			// Wait for PLL clock to be selected

	
	// DMA code
	RCC->APB2ENR |= (1 << 4);					// Enable GPIO_C clock.
	RCC->APB2ENR |= (1 << 3);					// Enable GPIOB clock.
	RCC->APB1ENR |= (1 << 0);					// Enable TIM2 clock.
	RCC->AHBENR  |= (1 << 0);					// Enable DMA1 clock.

	GPIO_C->CRL  = 0x33333333;			// Make low GPIO_C output
	GPIO_C->CRH = 0x33333333;			// Make high GPIO_C output
//	GPIO_C->ODR ^= -1;

	
	DMA1->CHN[2].CNDTR = sizeof(data)/sizeof(data[0]); // Transfer size
	DMA1->CHN[2].CMAR	= (uint32_t) data;				 // Memory source address
	DMA1->CHN[2].CPAR	= (uint32_t) &GPIO_C->ODR; // Peripheral destination address

	DMA1->CHN[2].CCR  = 0;				// Reset CCR
	DMA1->CHN[2].CCR &= ~(1 << 14);	// Disable memory to memory transfer on DMA1 channel 2
	DMA1->CHN[2].CCR |=  (0b11 << 12); // Set DMA priority to very high
	DMA1->CHN[2].CCR |=  (0b01 << 10); // Set memory transfer size to 16-bits
	DMA1->CHN[2].CCR |=  (0b10 << 8);	// Set peripheral transfer size to 32-bits
	DMA1->CHN[2].CCR |=  (1 << 7);		// Enable memory increment mode
	DMA1->CHN[2].CCR &= ~(1 << 6);		// Disable peripheral increment mode
	DMA1->CHN[2].CCR |=  (1 << 5);		// Enable circular mode
	DMA1->CHN[2].CCR |=  (1 << 4);		// Read from memory
	DMA1->CHN[2].CCR |=  (1 << 2);		// Enable half transfer completed interrupt
	DMA1->CHN[2].CCR |=  (1 << 1);		// Enable transfer completed interrupt

	DMA1->CHN[2].CCR |= (1 << 0);		// Enable DMA



	TIM2->CR1  = 0x0000;					// Reset CR1 just in case
//	TIM2->CR1  |= (1 << 4);				// Down counter mode
	TIM2->PSC  = (72e3/8)/(sizeof(data)/sizeof(data[0]))-1;	// fCK_PSC / (PSC[15:0] + 1)
	TIM2->ARR   = 8-1;
	TIM2->DIER |= (1 << 14);				// Trigger DMA request enable
	TIM2->DIER |= (1 <<  8);				// Update DMA request enable
//	TIM2->DIER |= (1 <<  6);				// Enable interrupt
//	TIM2->DIER |= (1 <<  0);				// Update interrupt enable

	TIM2->CR1  |= (1 << 0);				// Finally enable TIM1 module

	uint16_t idr;
	uint16_t idr_last;
	
	
	idr_last = 0b0000;
	for(;;) {
		int acc[2] = {0, 0};

		idr = (GPIO_B->IDR >> 6);			// PB[9:6]
		idr &= 0xf;
		//idr = 0b1001;
		if (idr_last != idr) {
			unsigned short fdbk = 0;

			for (int i = 0; i < TABSIZE-1; i++) {
				uint32_t sample = 0;
				uint8_t sd;

				for (int j = 0; j < 4; j++) {
					if (idr & (1 << j)) sample += sintab[i*(1 << j) & (TABSIZE-1)];
				}
				if (peak[idr & 0xf]) {
					sample = (sample*MAXVAL)/peak[idr & 0xf];
				}

				// sd = fosd(sample, acc, fdbk);
				sd = sosd(sample, acc, fdbk);
				data[i] = (sd) ? (1 << 13) : 0;
				fdbk    = (sd > 0) ? 0 : MAXVAL;
			}
			
		}
		else{
					for (int i = 0; i < TABSIZE-1; i++) {
					data[i] =0;
					}
			
			idr_last = idr;
		}
	
	
	}

	return 0;
}
