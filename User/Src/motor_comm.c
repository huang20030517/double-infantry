#include "motor_comm.h"

// 电机数据结构体
motor_dataTypeDef chassis_motor_data_list[4];
motor_dataTypeDef gimbal_motor_data_list[2];
motor_dataTypeDef shooter_motor_data_list[2];
motor_dataTypeDef feeder_motor_data;

// CAN 初始化
void Can_Init(void)
{
    CAN_FilterTypeDef filterConfig = {0};

    filterConfig.FilterBank = 0;
    filterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
    filterConfig.FilterScale = CAN_FILTERSCALE_16BIT;
    filterConfig.FilterFIFOAssignment = CAN_FILTER_FIFO0;
    filterConfig.FilterActivation = ENABLE;

    HAL_CAN_ConfigFilter(&hcan1, &filterConfig);
    HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);
    HAL_CAN_Start(&hcan1);
}

// 读取电机的数据
void Motor_Msg_Receive(void)
{
    CAN_RxHeaderTypeDef rx_header;
    uint8_t data[8] = {0};

    if (HAL_OK == HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &rx_header, data))
    {
        // 解析电机数据
        float angle = ((float)((data[0] << 8) | data[1]) / 8192.0f * 360.0f);
        int16_t speed = (data[2] << 8) | data[3];
        int16_t current = (data[4] << 8) | data[5];

        switch (rx_header.StdId)
        {
        case 0x201:
        case 0x202:
        case 0x203:
        case 0x204:
        {
            uint8_t index = rx_header.StdId - 0x201;
            if (index < sizeof(chassis_motor_data_list) / sizeof(chassis_motor_data_list[0]))
            {
                // chassis_motor_data_list[index].angle = angle;
                chassis_motor_data_list[index].speed = speed;
                // chassis_motor_data_list[index].current = current;
            }
            break;
        }

        case 0x209:
        case 0x20A:
        case 0x20B:
        {
            uint8_t index = rx_header.StdId - 0x209;
            if (index < sizeof(gimbal_motor_data_list) / sizeof(gimbal_motor_data_list[0]))
            {
                gimbal_motor_data_list[index].angle = angle;
                gimbal_motor_data_list[index].speed = speed;
            }
            break;
        }

        case 0x205:
            feeder_motor_data.angle = angle;
            feeder_motor_data.speed = speed;
            break;

        case 0x206:
        case 0x207:
        {
            uint8_t index = rx_header.StdId - 0x206;
            if (index < sizeof(shooter_motor_data_list) / sizeof(shooter_motor_data_list[0]))
            {
                shooter_motor_data_list[index].speed = speed;
            }
            break;
        }

        default:
            break;
        }
    }
}

// 发送电机消息
HAL_StatusTypeDef Send_Motor_Msg(MotorCanId std_id, int16_t *currents)
{
    uint8_t data[8] = {0};
    uint8_t num_motors = 0;

    switch (std_id)
    {
    case CHASSIS_MOTOR_CAN_ID:
        num_motors = 4;
        break;
    case FEEDER_SHOOTER_CAN_ID:
        num_motors = 3;
        break;
    case GIMBAL_MOTOR_CAN_ID:
        num_motors = 2;
        break;
    default:
        return HAL_ERROR;
    }

    for (uint8_t i = 0; i < num_motors; i++)
    {
        data[i * 2] = (uint8_t)(currents[i] >> 8);
        data[i * 2 + 1] = (uint8_t)currents[i];
    }

    CAN_TxHeaderTypeDef tx_header;
    tx_header.StdId = std_id;
    tx_header.IDE = CAN_ID_STD;
    tx_header.RTR = CAN_RTR_DATA;
    tx_header.DLC = 8;

    return HAL_CAN_AddTxMessage(&hcan1, &tx_header, data, (uint32_t *)NULL);
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

// 低通滤波器
int16_t low_pass_filter(int16_t input, int16_t previous_output, float alpha)
{
    return (int16_t)(alpha * input + (1.0f - alpha) * previous_output);
}
