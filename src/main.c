#include "HardwareProfile.h"
#include "ledCube.h"

// Let compile time pre-processor calculate the CORE_TICK_PERIOD
#define SYS_FREQ            (80000000L)
#define TOGGLES_PER_SEC     1000
#define CORE_TICK_RATE      (SYS_FREQ/2/TOGGLES_PER_SEC)

#define TIMER_MODE_RUN      0
#define TIMER_MODE_WAIT     1

// Animation timing variables
static int TimerMode;
volatile static bool SkipAnimation;
volatile static unsigned long Waited;
volatile static unsigned long CycleTimer;

void waitMicro(int);

int main(void) {
    // initialize the DIO pins we use for the LEDs to outputs
    PORTSetPinsDigitalOut(IOPORT_A, 0xC63F);
    PORTSetPinsDigitalOut(IOPORT_B, 0xFF38);
    PORTSetPinsDigitalOut(IOPORT_C, 0x2000);
    PORTSetPinsDigitalOut(IOPORT_D, 0xFFFD);
    PORTSetPinsDigitalOut(IOPORT_E, 0x001F);
    PORTSetPinsDigitalOut(IOPORT_F, 0x3134);
    PORTSetPinsDigitalOut(IOPORT_G, 0x7001);

    // Port B Bits 8+9 are inputs used for the HW interrupts
    PORTSetPinsDigitalIn(IOPORT_E, BIT_8 | BIT_9);
    
    // Configure the proper PB frequency and the number of wait states
    SYSTEMConfigPerformance(80000000L);

    // Turn off JTAG so we get the pins back
    mJTAGPortEnable(0);

    // initialize animation timing variables
    SkipAnimation = false;
    TimerMode = TIMER_MODE_RUN;
    Waited = 0;
    CycleTimer = 0;

    // Open up the core timer at our 1ms rate
    OpenCoreTimer(CORE_TICK_RATE);

    // set up the timer, skip animation, and sleep interrupts
    mConfigIntCoreTimer((CT_INT_ON | CT_INT_PRIOR_2 | CT_INT_SUB_PRIOR_0));
    ConfigINT2(EXT_INT_ENABLE | EXT_INT_PRI_3 | FALLING_EDGE_INT);
    ConfigINT1(EXT_INT_ENABLE | EXT_INT_PRI_4 | FALLING_EDGE_INT);

    // enable multi-vector interrupts
    INTEnableSystemMultiVectoredInt();

    // initialize the pseudo-random number generator
    srand(ReadCoreTimer());

    unsigned int i, f, cycle = 0;
    const Animation *cur;
    Frame *active, temp = { 0, 0, 0, 0, 0 };

    // main program loop
    while(1){
        // set the current animation
        cur = &animations[cycle++ % NUM_ANIMATIONS];

        for(i = 0; i < cur->Iterations; i++){   // foreach iteration
            for(f = 0; f < cur->Length; f++){   // foreach frame
                // were we told by the interrupt to stop?
                if(SkipAnimation){
                    SkipAnimation = false;
                    i = cur->Iterations;
                    break;
                }

                // if there is a generation function defined, use it
                if(cur->GetNextFrame > 0){
                    active = &temp;     // active = frame to be displayed
                    cur->GetNextFrame(f, active);
                }else{
                    // otherwise use the predefined frames
                    active = (Frame*)&cur->Frames[f];
                }
                
                CycleTimer = active->DisplayMS;

                // display the frame
                while (CycleTimer) {
                    writeLayer(1, &active->L1);
                    writeLayer(2, &active->L2);
                    writeLayer(3, &active->L3);
                    writeLayer(4, &active->L4);
                }
            }
        }
    }
}

/* Microsecond wait
 *
 * Spins for the given number of microseconds
 */
void waitMicro(int msec){
    TimerMode = TIMER_MODE_WAIT;

    while(Waited < msec);

    TimerMode = TIMER_MODE_RUN;

    while(Waited != 0);
}

/* Core Timer Handler
 *
 * Handle events created by the core timer interrupt
 */
void __ISR(_CORE_TIMER_VECTOR, ipl2) CoreTimerHandler(void) {
    // clear the interrupt flag
    mCTClearIntFlag();

    // are we waiting or running?
    switch (TimerMode) {
        case TIMER_MODE_RUN:
            if (CycleTimer) CycleTimer--;
            Waited = 0;
            break;
        case TIMER_MODE_WAIT:
            Waited++;
            break;
    }
    
    // update the period
    UpdateCoreTimer(CORE_TICK_RATE);
}

/* User Button Press Handler
 *
 * Handle events caused by a press of the "User" button
 *
 * In our case, skip the currently playing animation
 */
void __ISR(_EXTERNAL_2_VECTOR, ipl3) UserButtonPressHandler(void) {
    SkipAnimation = true;

    mINT2ClearIntFlag();
}

/* Prog Button Press Handler
 *
 * Handle events caused by a press of the "Prog" button
 *
 * In our case, put the board to sleep
 */
void __ISR(_EXTERNAL_1_VECTOR, ipl4) ProgButtonPressHandler(void) {
    // turn off all the leds
    PORTClearBits(IOPORT_A, 0xFFFF);
    PORTClearBits(IOPORT_B, 0xFFFF);
    PORTClearBits(IOPORT_C, 0xFFFF);
    PORTClearBits(IOPORT_D, 0xFFFF);
    PORTClearBits(IOPORT_E, 0xFFF0);
    PORTClearBits(IOPORT_F, 0xFFFF);
    PORTClearBits(IOPORT_G, 0xFFFF);

    // don't forget the 4 on the UBW32 itself
    PORTSetBits(IOPORT_E, 0x000F);

    // zzzZZZzzzZZZzzz
    PowerSaveSleep();

    mINT1ClearIntFlag();
}