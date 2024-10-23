#ifndef _BUZZER_H_
#define _BUZZER_H_

#include "main.h"
#include "tim.h"

#define BUZZER_CHANNEL TIM_CHANNEL_2

void Buzzer_Init(void);

void Buzzer_SetTone(uint16_t duty_cycle);

#endif // !_BUZZER_H_

