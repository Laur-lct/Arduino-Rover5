//All our permament constants should be defined here

#define DEBUG //debug configuration flag

// a macro that executes one line of code only if DEBUG flag is set.
#if defined(DEBUG)
    #define DBG_ONLY(x) (x)            
#else
    #define DBG_ONLY(x) 
#endif

#define MODES_MAX 2 //currently 2 modes for two different strategies
#define MODES_MIN_BROWSABLE 0 //minimum mode index, that could be set via mode button. 
#define MODE_BLINKER 0 // genius strategy of blinking with status led 2 

//indicates correct forward direction flag for each side.
#define MOTOR_FWD_TL 0
#define MOTOR_FWD_TR 0
#define MOTOR_FWD_BL 1
#define MOTOR_FWD_BR 1

//here go all EEPROM memory addresses
#define MEMADDR_LASTMODE 0
