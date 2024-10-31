#include "buzzer.h"

/// @brief 初始化蜂鸣器模块
///
///  该函数通过启动定时器 PWM 通道来初始化蜂鸣器模块，并将初始占空比设置为 0，保证蜂鸣器初始状态静音。
void Buzzer_Init(void)
{

    HAL_TIM_PWM_Start(&htim12, BUZZER_CHANNEL);

    TIM12->CCR2 = 0;
}

/// @brief 设置蜂鸣器的音调
///
/// 通过设置 PWM 占空比来控制蜂鸣器的音量和频率。
/// @param duty_cycle 占空比值（范围 0 - 2499）
///
/// 更高的占空比将使蜂鸣器发出更高的音量或频率。
void Buzzer_SetTone(uint16_t duty_cycle)
{

    TIM12->CCR2 = duty_cycle;
}
