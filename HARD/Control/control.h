#ifndef __control_H
#define __control_H


#include "sys.h"
#include "Globals.h"

#define INT_MAX   0x7fffffff   /* maximum value for an object of type int */
    
extern u8  ActualAxis;                      //!< monitored axis
extern u32 Delay;                            //!< Delay timer
extern u8  StandbyTimerInitFlag;            //!< TRUE after the standby timers have been intialized
extern u32 StandbyTimer[NUM_Motor];         //!< Standby timers
extern u32 FreewheelingTimer[NUM_Motor];    //!< Freewheeling timers

extern u32 Tick_Counter;

void SystemControl(void);

#endif


