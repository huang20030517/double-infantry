#include "pid.h"

// 清空 PID 缓存
void PID_Clear(pid_controler *PID)
{
    PID->d_err = 0;
    PID->i_err = 0;
    PID->err = 0;
    PID->output = 0;
}

// 设置 PID 参数
void PID_Set_Kpid(pid_controler *PID, float kp, float ki, float kd)
{
    PID->kp = kp;
    PID->ki = ki;
    PID->kd = kd;
}

// 初始化 PID 控制器
void PID_Init(pid_controler *PID)
{
    PID_Clear(PID);
    PID_Set_Kpid(PID, 0, 0, 0);
    PID->err_dz = 0;
    PID->i_err_limit = 0;
    PID->output_limit = 0;
    PID->state = PID_DISABLE;
}

// 设置 PID 参数并清空缓存
void PID_Set(pid_controler *PID, float kp, float ki, float kd, float i_err_limit, float err_dz, float output_limit)
{
    PID_Set_Kpid(PID, kp, ki, kd);
    PID->i_err_limit = i_err_limit;
    PID->err_dz = err_dz;
    PID->output_limit = output_limit;
    PID_Clear(PID);
}

// 设定限幅参数
void PID_Set_limit(pid_controler *PID, float err_limit, float i_err_limit, float err_dz, float output_limit)
{
    PID->err_limit = err_limit;
    PID->i_err_limit = i_err_limit;
    PID->err_dz = err_dz;
    PID->output_limit = output_limit;
}

// 误差输入并计算 PID 输出
float PID_Input(pid_controler *PID, float err)
{
    // if (PID->state == PID_DISABLE)
    // {
    //     return 0; // 如果 PID 被禁用，直接返回 0
    // }

    // 浮点数死区判断
    if (fabsf(err) < PID->err_dz)
        err = 0;

    // 更新误差值
    PID->i_err += err;
    PID->d_err = err - PID->err;
    PID->err = err;

    // 积分限幅
    if (PID->i_err > PID->i_err_limit)
        PID->i_err = PID->i_err_limit;
    else if (PID->i_err < -PID->i_err_limit)
        PID->i_err = -PID->i_err_limit;

    // 计算 P, I, D 输出
    PID->p_output = PID->kp * PID->err;
    PID->i_output = PID->ki * PID->i_err;
    PID->d_output = PID->kd * PID->d_err;

    // 计算 PID 总输出并进行限幅
    PID->output = PID->p_output + PID->i_output + PID->d_output;
    if (PID->output > PID->output_limit)
        PID->output = PID->output_limit;
    else if (PID->output < -PID->output_limit)
        PID->output = -PID->output_limit;

    return PID->output;
}
