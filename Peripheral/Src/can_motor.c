#include "can_motor.h"
#include "gimbal_control.h"

MotorSystem_t motor_system;

void Can_Init(void)
{

    HAL_FDCAN_ConfigGlobalFilter(&hfdcan2,
                                 FDCAN_ACCEPT_IN_RX_FIFO0,
                                 FDCAN_REJECT,
                                 FDCAN_REJECT_REMOTE,
                                 FDCAN_REJECT_REMOTE);

    HAL_FDCAN_Start(&hfdcan2);

    HAL_FDCAN_ActivateNotification(&hfdcan2, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);
}

void Motor_ProcessCANMessage(FDCAN_RxHeaderTypeDef *header, uint8_t *data)
{
    MotorData_t *motor = NULL;

    if (header->Identifier >= 0x205 && header->Identifier <= 0x208)
    {
        uint8_t index = header->Identifier - 0x205;
        motor = &motor_system.gimbal.left_head.motors[index];

        motor->angle = (((data[0] << 8) | data[1]) / 8191.0f) * 360.0f;
        motor->speed = (int16_t)((data[2] << 8) | data[3]);
        motor->current = (int16_t)((data[4] << 8) | data[5]);
        motor->temperature = data[6];

        Gimbal_Left_Head_Motor_Control(motor, index);
    }
}

// 发送多个电机电流数据（最多4个电机）
HAL_StatusTypeDef Send_Motor_Currents(int16_t *currents, uint8_t num_motors)
{
    if (num_motors > 4)
        return HAL_ERROR; // 限制最多4个电机

    uint8_t data[8] = {0}; // 每个电机电流占2字节，共8字节

    // 将电流数据填充到 CAN 数据帧中
    for (uint8_t i = 0; i < num_motors; i++)
    {
        data[i * 2] = (uint8_t)(currents[i] >> 8);       // 高8位
        data[i * 2 + 1] = (uint8_t)(currents[i] & 0xFF); // 低8位
    }

    // 配置 CAN 帧头
    FDCAN_TxHeaderTypeDef tx_header = {
        .Identifier = 0x1FF,             // 设置CAN ID（可根据需求动态调整）
        .IdType = FDCAN_STANDARD_ID,     // 标准ID
        .TxFrameType = FDCAN_DATA_FRAME, // 数据帧
        .DataLength = FDCAN_DLC_BYTES_8  // 数据长度为8字节
    };

    // 发送 CAN 消息
    return HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan2, &tx_header, data);
}

/// @brief
/// @param angleControl  实际角度 *0.01
/// @param maxSpeed  最大15000
HAL_StatusTypeDef Send_MultiTurn_Position_Control_Command(int32_t angleControl, uint16_t maxSpeed)
{
    uint8_t data[8] = {0xA4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    data[2] = (uint8_t)(maxSpeed & 0xFF);        // 低字节
    data[3] = (uint8_t)((maxSpeed >> 8) & 0xFF); // 高字节

    data[4] = (uint8_t)(angleControl & 0xFF);         // 低字节
    data[5] = (uint8_t)((angleControl >> 8) & 0xFF);  // 第二个字节
    data[6] = (uint8_t)((angleControl >> 16) & 0xFF); // 第三个字节
    data[7] = (uint8_t)((angleControl >> 24) & 0xFF); // 高字节

    FDCAN_TxHeaderTypeDef tx_header = {
        .Identifier = 0x141,
        .IdType = FDCAN_STANDARD_ID,
        .TxFrameType = FDCAN_DATA_FRAME,
        .DataLength = FDCAN_DLC_BYTES_8};

    return HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan2, &tx_header, data);
}
