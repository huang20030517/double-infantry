#include "utils.h"

/// @brief 低通滤波器函数
/// @param input 当前输入值
/// @param previous_output 前一次输出值
/// @param alpha 滤波系数（范围 0.0 - 1.0）
/// @return 滤波后的输出值
float low_pass_filter(float input, float previous_output, float alpha)
{
    // 使用乘法运算替代多余的减法，减少浮点计算开销
    return previous_output + alpha * (input - previous_output);
}

/// @brief 更新并计算旋转物体的多圈实际角度
/// @param current_angle 当前角度（0 - 360）
/// @return 累积的多圈角度
float Update_Angle(float current_angle)
{
    static int32_t rotation_count = 0; // 使用 int32_t 处理多圈累积
    static float previous_angle = 0.0f;

    // 计算当前和上次角度的差值
    float angle_difference = current_angle - previous_angle;

    // 优化边界检测，避免小误差引发的错误累加
    if (angle_difference > 180.0f)
    {
        rotation_count--;
    }
    else if (angle_difference < -180.0f)
    {
        rotation_count++;
    }

    // 更新前一次角度值
    previous_angle = current_angle;

    // 使用FPU加速累积角度计算
    return rotation_count * 360.0f + current_angle;
}
