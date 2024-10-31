#include "motor.h"

MotorSystem_t motor_system;

/// @brief 发送电机电流数据到 CAN 总线
/// @param hfdcan CAN 句柄
/// @param can_id CAN 消息 ID
/// @param currents 电机电流数组，每个电机的电流占用 2 个字节
/// @param num_motors 电机数量，最大为 4 个
/// @return 返回 HAL 状态，表示操作是否成功
HAL_StatusTypeDef Send_Motor_Currents(FDCAN_HandleTypeDef *hfdcan, uint32_t can_id, int16_t *currents, uint8_t num_motors)
{
    if (num_motors > 4)
        return HAL_ERROR;

    uint8_t data[8] = {0};

    for (uint8_t i = 0; i < num_motors; i++)
    {
        data[i * 2] = (uint8_t)(currents[i] >> 8);
        data[i * 2 + 1] = (uint8_t)(currents[i] & 0xFF);
    }

    FDCAN_TxHeaderTypeDef tx_header = {
        .Identifier = can_id,
        .IdType = FDCAN_STANDARD_ID,
        .TxFrameType = FDCAN_DATA_FRAME,
        .DataLength = num_motors * 2};

    return HAL_FDCAN_AddMessageToTxFifoQ(hfdcan, &tx_header, data);
}

/// @brief 发送多圈位置控制命令
///
/// 该函数用于通过 CAN 发送多圈位置控制命令，控制电机的目标角度和速度。
/// @param angleControl 控制的角度值（单位：0.01度）
/// @param maxSpeed 最大速度（最大 15000）
/// @return 返回 HAL 操作状态
HAL_StatusTypeDef Send_MultiTurn_Position_Control_Command(int32_t angleControl, uint16_t maxSpeed)
{
    uint8_t data[8] = {0xA4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    data[2] = (uint8_t)(maxSpeed & 0xFF);
    data[3] = (uint8_t)((maxSpeed >> 8) & 0xFF);

    data[4] = (uint8_t)(angleControl & 0xFF);
    data[5] = (uint8_t)((angleControl >> 8) & 0xFF);
    data[6] = (uint8_t)((angleControl >> 16) & 0xFF);
    data[7] = (uint8_t)((angleControl >> 24) & 0xFF);

    FDCAN_TxHeaderTypeDef tx_header = {
        .Identifier = 0x141,
        .IdType = FDCAN_STANDARD_ID,
        .TxFrameType = FDCAN_DATA_FRAME,
        .DataLength = FDCAN_DLC_BYTES_8};

    return HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan2, &tx_header, data);
}
