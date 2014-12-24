#ifndef HARDWARE_PROFILE_UBW32_H
#define HARDWARE_PROFILE_UBW32_H

#include "plib.h"

typedef unsigned char bool;

#ifndef false
	#define false	0
#endif
#ifndef true
	#define true 	1
#endif
#ifndef FALSE
	#define FALSE	0
#endif
#ifndef TRUE
	#define TRUE	1
#endif
#ifndef False
	#define False	0
#endif
#ifndef True
	#define True 	1
#endif

    /** CPU Clock Speed Assignment *************************************/
    //#define RUN_AT_48MHZ
    //#define RUN_AT_24MHZ
    //#define RUN_AT_60MHZ
    #define RUN_AT_80MHZ

    // Various clock values
    #if defined(RUN_AT_48MHZ)
        #define GetSystemClock()            48000000UL
        #define GetPeripheralClock()        48000000UL
        #define GetInstructionClock()       (GetSystemClock())
    #elif defined(RUN_AT_24MHZ)
        #define GetSystemClock()            24000000UL
        #define GetPeripheralClock()        24000000UL
        #define GetInstructionClock()       (GetSystemClock())
    #elif defined(RUN_AT_60MHZ)    
        #define GetSystemClock()            (60000000ul)
        #define GetPeripheralClock()        (GetSystemClock()) 
        #define GetInstructionClock()       (GetSystemClock())
    #elif defined(RUN_AT_80MHZ)    
        #define GetSystemClock()            (80000000ul)
        #define GetPeripheralClock()        (GetSystemClock()) 
        #define GetInstructionClock()       (GetSystemClock())
    #else
        #error Choose a speed
    #endif
#endif


