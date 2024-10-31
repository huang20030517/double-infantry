#include "pid.h"

/**
 * @brief 清空 PID 缓存
 * @param PID 指向 pid_controler 结构体的指针
 * @details 将 PID 控制器的误差值、输出值等缓存数据重置为 0
 */
void PID_Clear(pid_controler *PID)
{
    PID->d_err = 0;
    PID->i_err = 0;
    PID->err = 0;
    PID->output = 0;
}

/**
 * @brief 设置 PID 参数
 * @param PID 指向 pid_controler 结构体的指针
 * @param kp 比例增益
 * @param ki 积分增益
 * @param kd 微分增益
 * @details 设置 PID 控制器的比例、积分、微分增益
 */
void PID_Set_Kpid(pid_controler *PID, float kp, float ki, float kd)
{
    PID->kp = kp;
    PID->ki = ki;
    PID->kd = kd;
}

/**
 * @brief 初始化 PID 控制器
 * @param PID 指向 pid_controler 结构体的指针
 * @details 初始化 PID 控制器的状态、误差值和增益参数
 */
void PID_Init(pid_controler *PID)
{
    PID_Clear(PID);
    PID_Set_Kpid(PID, 0, 0, 0);
    PID->err_dz = 0;
    PID->i_err_limit = 0;
    PID->output_limit = 0;
    PID->state = PID_DISABLE;
}

/**
 * @brief 设置 PID 参数并清空缓存
 * @param PID 指向 pid_controler 结构体的指针
 * @param kp 比例增益
 * @param ki 积分增益
 * @param kd 微分增益
 * @param i_err_limit 积分误差限幅
 * @param err_dz 误差死区
 * @param output_limit 输出限幅
 * @details 设置 PID 参数并清空误差缓存和输出缓存
 */
void PID_Set(pid_controler *PID, float kp, float ki, float kd, float i_err_limit, float err_dz, float output_limit)
{
    PID_Set_Kpid(PID, kp, ki, kd);
    PID->i_err_limit = i_err_limit;
    PID->err_dz = err_dz;
    PID->output_limit = output_limit;
    PID_Clear(PID);
}

/**
 * @brief 设定 PID 限幅参数
 * @param PID 指向 pid_controler 结构体的指针
 * @param err_limit 误差限幅
 * @param i_err_limit 积分误差限幅
 * @param err_dz 误差死区
 * @param output_limit 输出限幅
 * @details 为 PID 控制器设置各类限幅参数
 */
void PID_Set_limit(pid_controler *PID, float err_limit, float i_err_limit, float err_dz, float output_limit)
{
    PID->err_limit = err_limit;
    PID->i_err_limit = i_err_limit;
    PID->err_dz = err_dz;
    PID->output_limit = output_limit;
}

/**
 * @brief 设置误差输入并计算 PID 输出
 * @param PID 指向 pid_controler 结构体的指针
 * @param err 当前误差
 * @return PID 控制器的输出值
 * @details 使用当前误差计算 PID 输出值，并考虑积分、微分项的限幅和死区
 */
float PID_Set_Err(pid_controler *PID, float err)
{
    // 判断是否在误差死区范围内
    if (fabsf(err) < PID->err_dz)
    {
        err = 0;
    }

    // 更新误差值
    PID->i_err += err;
    PID->d_err = err - PID->err;
    PID->err = err;

    // 积分限幅
    if (PID->i_err > PID->i_err_limit)
        PID->i_err = PID->i_err_limit;
    else if (PID->i_err < -PID->i_err_limit)
        PID->i_err = -PID->i_err_limit;

    // 计算 P、I、D 输出
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
