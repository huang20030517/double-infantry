#include "main.h"
#include "can.h"

typedef struct __motor_dataTypeDef
{
    float angle;         // 角度值
    int16_t speed;       // 速度值
    int16_t current;     // 电流值
    uint8_t temperature; // 温度值

} motor_dataTypeDef;

typedef enum
{
    CHASSIS_MOTOR_CAN_ID = 0x200,  // 底盘电机
    FEEDER_SHOOTER_CAN_ID = 0x1FF, // 供弹/发射电机
    GIMBAL_MOTOR_CAN_ID = 0x2FE    // 云台电机
} MotorCanId;


void Can_Init(void);
void Motor_Msg_Receive(void);
HAL_StatusTypeDef Send_Motor_Msg(MotorCanId std_id, int16_t *currents);
float Update_Angle(float current_angle);

int16_t low_pass_filter(int16_t input, int16_t previous_output, float alpha);

extern motor_dataTypeDef chassis_motor_data_list[4];
extern motor_dataTypeDef gimbal_motor_data_list[2];
extern motor_dataTypeDef shooter_motor_data_list[2];
extern motor_dataTypeDef feeder_motor_data;
