#include <xc.h> // include processor files - each processor file is guarded.  

#ifndef MANUAL_H
#define MANUAL_H

#include <stdint.h>

void manualPWM_setup(void);
void manualPWM_ISR(void);
//uint8_t manualPWM_duty(uint8_t adc_value);


#endif