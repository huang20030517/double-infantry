/**
  ******************************************************************************
  * @file    dbus.c
  * @brief   接收并解码dbus。
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/


#include "dbus.h"



uint8_t dbus_buffer[18];
_RC_Ctl_t RC_Ctl;

void dbus_init()
{
	RC_Ctl.rc.ch0 = 0.0;
	RC_Ctl.rc.ch1 = 0.0;
	RC_Ctl.rc.ch2 = 0.0;
	RC_Ctl.rc.ch3 = 0.0;
	RC_Ctl.rc.dial_wheel = 0.0;
		
	RC_Ctl.mouse.x = 0;
	RC_Ctl.mouse.y = 0;
	RC_Ctl.mouse.z = 0;
	
	HAL_UART_Receive_DMA(&huart3, dbus_buffer, 18); 


	
	
}

//遥控数据混乱
bool REMOTE_IfDataError( void )
{
	if ( (RC_Ctl.rc.s1 != 1 && RC_Ctl.rc.s1 != 3 && RC_Ctl.rc.s1 != 2)
		|| (RC_Ctl.rc.s2 != 1 && RC_Ctl.rc.s2 != 3 && RC_Ctl.rc.s2 != 2)
		|| (RC_Ctl.rc.ch0 > 1 || RC_Ctl.rc.ch0 < -1)
		|| (RC_Ctl.rc.ch1 > 1 || RC_Ctl.rc.ch1 < -1)
		|| (RC_Ctl.rc.ch2 > 1 || RC_Ctl.rc.ch2 < -1)
		|| (RC_Ctl.rc.ch3 > 1 || RC_Ctl.rc.ch3 < -1) )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}



void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle)
{
    if (UartHandle == &huart3) {
		
		
		unsigned short rx = (dbus_buffer[0]| (dbus_buffer[1] << 8)) & 0x7ff;          
		unsigned short ry = ((dbus_buffer[1] >> 3) | (dbus_buffer[2] << 5)) & 0x7ff;       
		unsigned short lx = ((dbus_buffer[2] >> 6) | (dbus_buffer[3] << 2) | (dbus_buffer[4] << 10)) & 0x7ff;          
		unsigned short ly = ((dbus_buffer[4] >> 1) | (dbus_buffer[5] << 7)) & 0x7ff;
		unsigned short dial_wheel = (dbus_buffer[17] << 8) | dbus_buffer[16];

		//	区间 （1 ~ -1）
		RC_Ctl.rc.ch0 = (rx - 1024.0) / (1684.0 - 1024.0);
		RC_Ctl.rc.ch1 = (ry - 1024.0) / (1684.0 - 1024.0);
		RC_Ctl.rc.ch2 = (lx - 1024.0) / (1684.0 - 1024.0);
		RC_Ctl.rc.ch3 = (ly - 1024.0) / (1684.0 - 1024.0); 
		RC_Ctl.rc.dial_wheel =  ((dial_wheel - 1024.0) / (1684.0 - 1024.0));

		RC_Ctl.rc.s1 = ((dbus_buffer[5] >> 4)  & 0x00C ) >> 2;
		RC_Ctl.rc.s2 = (dbus_buffer[5] >> 4) & 0x03;

			
		RC_Ctl.mouse.x = (dbus_buffer[6]) | (dbus_buffer[7] << 8);
		RC_Ctl.mouse.y = (dbus_buffer[8]) | (dbus_buffer[9] << 8);
		RC_Ctl.mouse.z = (dbus_buffer[10]) | (dbus_buffer[11] << 8);     
		RC_Ctl.mouse.left_key = dbus_buffer[12];
		RC_Ctl.mouse.right_key = dbus_buffer[13];
		
		
		RC_Ctl.keyboard.w = dbus_buffer[14] & 0b00000001;
		RC_Ctl.keyboard.s = (dbus_buffer[14] & 0b00000010) >> 1;
		RC_Ctl.keyboard.a = (dbus_buffer[14] & 0b00000100) >> 2;
		RC_Ctl.keyboard.d = (dbus_buffer[14] & 0b00001000) >> 3;
		
		RC_Ctl.keyboard.shift = (dbus_buffer[14] & 0b00010000) >> 4;
		RC_Ctl.keyboard.ctrl = (dbus_buffer[14] & 0b00100000) >> 5;
		RC_Ctl.keyboard.q= (dbus_buffer[14] & 0b01000000) >> 6;
		RC_Ctl.keyboard.e = (dbus_buffer[14] & 0b10000000) >> 7;
		
		
		HAL_UART_Receive_DMA(&huart3, dbus_buffer, 18);
		
    }
	

}





