

/* 
 * File:   
 * Author: Luis Pedro Gonzalez
 * Comments:
 * Revision history: 
 */



#include <xc.h> // include processor files - each processor file is guarded.  

#ifndef PWM_H
#define PWM_H

void PWM_config(char canal, float periodo_ms);
void PWM_duty(char canal, float duty);

#endif // PWM_H


