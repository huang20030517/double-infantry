#include "buzzer.h"

void Buzzer_Init(void)
{

    HAL_TIM_PWM_Start(&htim12, BUZZER_CHANNEL);

    TIM12->CCR2 = 0;
}

// 0 - 2499
void Buzzer_SetTone(uint16_t duty_cycle)
{

    TIM12->CCR2 = duty_cycle;
}
