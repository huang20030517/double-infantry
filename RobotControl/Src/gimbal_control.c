#include "gimbal_control.h"
#include "string.h"

bool initialized = false;
float filtered_ = 0;

int16_t friction_wheel_speed = 0;

uint8_t motor_update_flags = 0;

// 低通滤波器
int16_t low_pass_filter(int16_t input, int16_t previous_output, float alpha)
{
    return (int16_t)(alpha * input + (1.0f - alpha) * previous_output);
}

// 更新并计算旋转物体的实际角度
float Update_Angle(float current_angle)
{
    static int16_t rotation_count = 0;
    static float previous_angle = 0;

    float angle_difference = current_angle - previous_angle;

    // 判断是否穿越了0度或360度的边界
    if (angle_difference > 180.0f)
    {
        rotation_count--;
    }
    else if (angle_difference < -180.0f)
    {
        rotation_count++;
    }

    previous_angle = current_angle;

    return rotation_count * 360.0f + current_angle;
}

void Gimbal_Left_Head_Pid_Init(void)
{
    // 初始化左头的 PID 控制器，直接访问全局变量
    GimbalHead_t *left_head = &motor_system.gimbal.left_head;

    // 初始化供弹和摩擦轮的 PID 控制器
    for (uint8_t i = 0; i < 3; i++)
    {
        PID_Init(&left_head->pid_group[i]);
        switch (i)
        {
        case 0: // 供弹电机
            PID_Set(&left_head->pid_group[i], 10, 0, 0.0, 5000, 0, 10000);
            break;
        case 1: // 左摩擦轮
            PID_Set(&left_head->pid_group[i], 20, 0.4, 0.0, 5000, 0, 10000);
            break;
        case 2: // 右摩擦轮
            PID_Set(&left_head->pid_group[i], 20, 0.4, 0.0, 5000, 0, 10000);
            break;
        }
    }

    // 初始化 Yaw 电机的 PID 控制器
    // yaw_pid[0]: 位置环
    PID_Init(&left_head->yaw_pid[0]);
    PID_Set(&left_head->yaw_pid[0], 1.1, 0.0, 0.0, 0, 0.25, 5000);

    // yaw_pid[1]: 速度环
    PID_Init(&left_head->yaw_pid[1]);
    PID_Set(&left_head->yaw_pid[1], 200, 1.5, 0, 5000, 0, 15000);
    memset(currents, 0, sizeof(currents));
}

void Gimbal_Left_Head_Motor_Control(MotorData_t *motor, uint8_t index)
{

    GimbalHead_t *left_head = &motor_system.gimbal.left_head;

    if (!initialized)
    {
        yaw_init = motor_system.gimbal.left_head.motors[3].angle;
        initialized = true;
    }

    if (initialized == false)
        return;

    friction_wheel_speed = (RC_Ctl.rc.s1 == 3) ? 2000 : 0;
    int16_t feed_motor_speed = (RC_Ctl.rc.s2 == 3) ? -500 : 0;

    L_x_t += (float)RC_Ctl.rc.ch0 * -0.2f;

    switch (index)
    {
    case 0:
    case 1:
    case 2:
        if (feed_motor_speed != 0 || friction_wheel_speed != 0)
        {
            currents[0] = PID_Set_Err(&left_head->pid_group[0], feed_motor_speed - motor_system.gimbal.left_head.motors[0].speed);
            currents[1] = PID_Set_Err(&left_head->pid_group[1], friction_wheel_speed - motor_system.gimbal.left_head.motors[1].speed);
            currents[2] = PID_Set_Err(&left_head->pid_group[2], -friction_wheel_speed - motor_system.gimbal.left_head.motors[2].speed);
        }
        else
        {
            currents[0] = 0;
            currents[1] = 0;
            currents[2] = 0;
        }

        break;

    case 3: // Yaw 电机控制逻辑（位置环+速度环）
        L_x_t_actual = Update_Angle(motor_system.gimbal.left_head.motors[3].angle - yaw_init);
        float position_output = PID_Set_Err(&left_head->yaw_pid[0], L_x_t - L_x_t_actual);

        filtered_speed = low_pass_filter(motor->speed, filtered_speed, 0.7f);
        currents[3] = PID_Set_Err(&left_head->yaw_pid[1], position_output - filtered_speed);

        break;

    default:
        return;
    }

    motor_update_flags |= (1 << index);

    if (__builtin_popcount(motor_update_flags) >= 3)
    {
        Send_Motor_Currents(currents, 4);
        motor_update_flags = 0;
    }
}
