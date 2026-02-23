
//--------------------------------------------------------------------------- IO pins

#if 0

#define STEP_1A 9
#define STEP_1B 10
#define STEP_1C 11
#define STEP_1D 12

#define STEP_2A 5
#define STEP_2B 6
#define STEP_2C 7
#define STEP_2D 8

#define NUM_LEDS 3

#else

#define STEP_1A 9
#define STEP_1B 10
#define STEP_1C 11
#define STEP_1D 12

#define STEP_2A 8
#define STEP_2B 7
#define STEP_2C 6
#define STEP_2D 5

#define NUM_LEDS 12

#endif

#define SERVO 4
#define SPEAKER 3
#define NEOPIXEL 2
#define SWITCH_F A4
#define SWITCH_B A5
#define SENSOR A6

//--------------------------------------------------------------------------- global robot geometry and speed constants etc.

const uint16_t minDelay = 1000;               //                              minimum delay time between steps (in microseconds)
const uint16_t maxDelay = 3000;               //                              maximum delay time between steps (in microseconds)
const uint16_t rampValue = 5;                 //                              smaller value = longer speed ramp
const int stepsPerRotation = 64 * 64;         //                              64 steps per rotation and 1:64 gearbox
const float wheelCircumference = 23.39 * PI;  //                              mm <- to be adjusted if distances are drawn incorrect
const float stepsPerMM = stepsPerRotation / wheelCircumference;
const float wheelDistance = 33.5;  //                                         mm <- to be adjusted if angles are drawn incorrect
const float stepsPer360Rotation = wheelDistance * PI * stepsPerMM;
const int servoPenDown = 0;     //                                            servo angle pen down
const int servoPenUp = 180;     //                                            servo angle pen up
const float sensorOffset = 16;  //                                            distance between photo sensor pinhole and center of robot (15mm in reality, might make sense to have this longer because of pen drag and line thickness)
