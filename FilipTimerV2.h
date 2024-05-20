#ifndef FILIPTIMERV2_H_
#define FILIPTIMERV2_H_

//====== INSTRUCTIONS ======
//Put timerStart() after a new audio signal has been detected
//Put timerEnd() after your function has completed all processes

#define samples 10 //change this number to change the amount of times the timer will run

#include <stdio.h>
#include "platform.h"
#include "xparameters.h"
#include "xtime_l.h"

// #define CPS 0x135F1B40
#define CPuS 325

void timerStart();
void timerEnd();

#endif /* FILIPTIMERV2_H_ */
