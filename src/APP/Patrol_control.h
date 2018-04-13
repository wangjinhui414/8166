#ifndef _PATROL_CONTROL_H
#define _PATROL_CONTROL_H
#include "main.h"
#include <stdio.h>
#include "hal_process_CONTROL.h"
typedef enum
{
	Forward,
	Backward,
	Left,
	Right,
	Stop
} TrackingDir;
extern TrackingDir abcsse;
typedef union{
  float f;
  char  PmBuf[4];
}PmData;

typedef union{
  float pid;
  char  PwBuf[4];
}PwData;

typedef struct PID //视觉pid循线算法
{ 
	float SetPoint;      // 设定目标Desired value 
	float Proportion;    // 比例常数Proportional Const 
	float Integral;      // 积分常数Integral Const 
	float Derivative;    // 微分常数Derivative Const 
	float LastError;     // Error[-1] 
	float PrevError;    // Error[-2] 
	float SumError;    // Sums of Errors 
} PID;


/***************************函数声明****************************************************/
float PIDCalc( PID *pp, float CurrPoint );
//float getdis(bool Q1, bool Q2, bool Q3, bool Q4, bool Q5, bool Q6, bool Q7, bool Q8,bool Q9, bool Q10, bool Q11, bool Q12, bool Q13, bool Q14);//红外巡线偏离角度计算算法
long getdis(char Q1, char Q2, char Q3, char Q4, char Q5, char Q6, char Q7, char Q8,char Q9, char Q10, char Q11, char Q12, char Q13, char Q14);//红外巡线偏离角度计算算法
void TrackingLR(TrackingDir dir);//90度左右行走，根据红外传感器，实时调整轮子角度
void Tracking(TrackingDir dir);//直行前进后退，根据红外传感器，实时调整轮子角度
void LeftWheel(double turnpos);//左面转向电机走位置旋转
void RightWheel(double turnpos);//右面转向电机走位置旋转
void BackwardWheel(double turnpos);//后面转向电机走位置旋转
void ForwardWheel(double turnpos);//前面转向电机走位置旋转
float PidFlashAvrB(void);
float PidFlashAvrF(void);
bool isFrontBlimited(void);  //前限位
bool isBackFlimited(void);	  //后限位	
bool isLeftRlimited(void);   //左限位
bool isRightLlimited(void);  //右限位
void OnReadIR(void);
void ClearFpidBpid(void);

u8 Barrier(u16 wpos1 , u16 wpos2 ,u8 lpos1 , u8 lpos2);
#endif

