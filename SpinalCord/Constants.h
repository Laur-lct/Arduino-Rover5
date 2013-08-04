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
#define MODE_LIGHTSEEKER 1 // robot will try to turn to strongest light source 

//indicates correct forward direction flag for each shaft.
#define MOTOR_FWD_T 0
#define MOTOR_FWD_B 1

#define MOTOR_TIMER_INTERVAL  1000//timer tick interval in microseconds
#define MOTOR_CALIBRATION_TICK 250 // timer tick number to run motor calibration 

//binary wheel codes
#define MOTOR_WHEEL_TL 1
#define MOTOR_WHEEL_TR 2
#define MOTOR_WHEEL_BR 4
#define MOTOR_WHEEL_BL 8

//head servo constants
#define HEAD_PAN_CENTER 88
#define HEAD_TILT_CENTER 100
#define HEAD_PAN_MAX 178
#define HEAD_PAN_MIN 0
#define HEAD_TILT_MAX 140
#define HEAD_TILT_MIN 15

//binary bumper  IR sensor codes
#define SENSOR_IRBUMP_TL 1
#define SENSOR_IRBUMP_TR 2
#define SENSOR_IRBUMP_BR 4
#define SENSOR_IRBUMP_BL 8

//here go all EEPROM memory addresses
#define MEMADDR_LASTMODE 0
// motor calibration cache
#define MEMADDR_MOTORCACHE_START 1 
#define MEMADDR_MOTORCACHE_END MEMADDR_MOTORCACHE_START + 160


