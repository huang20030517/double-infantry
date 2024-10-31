#include "can_motor.h"
#include "gimbal_control.h"

MotorSystem_t motor_system;

/// @brief 初始化 CAN 配置
///
/// 该函数用于初始化 CAN 硬件的全局过滤器、启动 CAN 硬件、并激活接收中断通知。
void Can_Init(void)
{
    // 配置全局过滤器，允许接收 RX FIFO0 的新消息
    HAL_FDCAN_ConfigGlobalFilter(&hfdcan2,
                                 FDCAN_ACCEPT_IN_RX_FIFO0,
                                 FDCAN_REJECT,
                                 FDCAN_REJECT_REMOTE,
                                 FDCAN_REJECT_REMOTE);

    // 启动 FDCAN2 硬件
    HAL_FDCAN_Start(&hfdcan2);

    // 激活接收新消息的中断通知
    HAL_FDCAN_ActivateNotification(&hfdcan2, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);
}

/// @brief 处理 CAN 接收消息并更新电机数据
///
/// 该函数用于解析接收到的 CAN 数据，并根据数据更新电机的角度、速度、电流等信息。
/// @param header CAN 消息头
/// @param data CAN 消息数据
void Motor_ProcessCANMessage(FDCAN_RxHeaderTypeDef *header, uint8_t *data)
{
    MotorData_t *motor = NULL;

    // 确保接收的 ID 在指定范围内（0x205 - 0x208）
    if (header->Identifier >= 0x205 && header->Identifier <= 0x208)
    {
        uint8_t index = header->Identifier - 0x205;
        motor = &motor_system.gimbal.left_head.motors[index];

        // 解析并更新电机数据
        motor->angle = (((data[0] << 8) | data[1]) / 8191.0f) * 360.0f; // 电机角度，范围 0-360 度
        motor->speed = (int16_t)((data[2] << 8) | data[3]);             // 电机速度
        motor->current = (int16_t)((data[4] << 8) | data[5]);           // 电机电流
        motor->temperature = data[6];                                   // 电机温度

        // 调用云台左侧头部电机控制函数
        Gimbal_Left_Head_Motor_Control(motor, index);
    }
}

/// @brief 发送多个电机的电流数据（最多4个电机）
///
/// 该函数通过 CAN 发送指定电机的电流控制信号。
/// @param currents 电流数组
/// @param num_motors 电机数量（最多 4 个）
/// @return 返回 HAL 操作状态
HAL_StatusTypeDef Send_Motor_Currents(int16_t *currents, uint8_t num_motors)
{
    if (num_motors > 4)
        return HAL_ERROR; // 限制最多 4 个电机

    uint8_t data[8] = {0}; // 每个电机电流占用 2 个字节，共 8 个字节

    // 将电流数据填充到 CAN 数据帧中
    for (uint8_t i = 0; i < num_motors; i++)
    {
        data[i * 2] = (uint8_t)(currents[i] >> 8);       // 高 8 位
        data[i * 2 + 1] = (uint8_t)(currents[i] & 0xFF); // 低 8 位
    }

    // 配置 CAN 帧头
    FDCAN_TxHeaderTypeDef tx_header = {
        .Identifier = 0x1FF,             // 设置 CAN ID（可以根据需求动态调整）
        .IdType = FDCAN_STANDARD_ID,     // 标准 ID
        .TxFrameType = FDCAN_DATA_FRAME, // 数据帧
        .DataLength = FDCAN_DLC_BYTES_8  // 数据长度为 8 字节
    };

    // 发送 CAN 消息
    return HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan2, &tx_header, data);
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

    // 设置最大速度值（低字节和高字节）
    data[2] = (uint8_t)(maxSpeed & 0xFF);        // 低字节
    data[3] = (uint8_t)((maxSpeed >> 8) & 0xFF); // 高字节

    // 设置目标角度值（4 个字节表示）
    data[4] = (uint8_t)(angleControl & 0xFF);         // 低字节
    data[5] = (uint8_t)((angleControl >> 8) & 0xFF);  // 第二个字节
    data[6] = (uint8_t)((angleControl >> 16) & 0xFF); // 第三个字节
    data[7] = (uint8_t)((angleControl >> 24) & 0xFF); // 高字节

    // 配置 CAN 帧头
    FDCAN_TxHeaderTypeDef tx_header = {
        .Identifier = 0x141,             // 设置 CAN ID（可根据需求调整）
        .IdType = FDCAN_STANDARD_ID,     // 标准 ID
        .TxFrameType = FDCAN_DATA_FRAME, // 数据帧
        .DataLength = FDCAN_DLC_BYTES_8  // 数据长度为 8 字节
    };

    // 发送 CAN 消息
    return HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan2, &tx_header, data);
}
