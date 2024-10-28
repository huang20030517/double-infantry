/**
  ******************************************************************************
  * @file    pid.h
  * @brief    PID��������
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */
	
#ifndef __PID_H
#define __PID_H
/* Includes ------------------------------------------------------------------*/
#include "stdint.h"

typedef enum
{
	PID_DISABLE,
	PID_ENABLE
}PID_STATE;

typedef struct pid_controler
{
	float err;	//errֵ��ؼ���ֵ
	float err_dz;
	float err_limit;
	
	float d_err;
	float i_err;
	float i_err_limit;
	
	float kp;	//PID����
	float ki;
	float kd;

	float output;			//������
	float output_limit;		//����޷�
	float p_output;
	float i_output;
	float d_output;
	
	PID_STATE startfalg;
	
}pid_controler;

//PID��ʼ��
void PID_Init(pid_controler *PID);

//PID�Ĳ���
void PID_Set(pid_controler *PID, float kp, float ki, float kd, float i_err_limit, float err_dz, float output_limit);

//PID�������
void PID_Clear(pid_controler *PID);

//���������
//PID ģ��ṹ��
//err ����PID�����еõ���errֵ
float pid_error_input(pid_controler *PID, float err);

#endif

