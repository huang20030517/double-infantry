#include "can.h"

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

void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{
    if (hfdcan == &hfdcan2)
    { // 确保回调来自 CAN2

        // 检查是否有新消息到达
        if (RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE)
        {
            FDCAN_RxHeaderTypeDef rx_header;
            uint8_t rx_data[8];

            // 从 FIFO0 中读取数据
            if (HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &rx_header, rx_data) == HAL_OK)
            {

                if (rx_header.Identifier == 0x20A)
                {

                    // printf("Speed: %d, Position: %d\r\n", speed, position);
                }
            }
            else
            {
                // printf("Error: Failed to get CAN2 message.\r\n");
            }
        }

        // // 检查 FIFO 是否已满
        // if (RxFifo0ITs & FDCAN_IT_RX_FIFO0_FULL)
        // {
        //     printf("Warning: CAN2 FIFO0 is full.\n");
        // }

        // // 检查是否有消息丢失
        // if (RxFifo0ITs & FDCAN_IT_RX_FIFO0_MESSAGE_LOST)
        // {
        //     printf("Error: Message lost in CAN2 FIFO0.\n");
        // }
    }
}
