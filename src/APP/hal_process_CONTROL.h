#ifndef _HAL_PROCESS_CONTROL_
#define _HAL_PROCESS_CONTROL_
#include "bsp.h"


/*电机CAN网络ID配置*/
#define ID_TX_MOTOR_TURN1   0x601
#define ID_TX_MOTOR_TURN3   0x603
#define ID_TX_MOTOR_TURN5   0x605
#define ID_TX_MOTOR_TURN7   0x607

#define ID_RX_MOTOR_TURN1   0x581
#define ID_RX_MOTOR_TURN3   0x583
#define ID_RX_MOTOR_TURN5   0x585
#define ID_RX_MOTOR_TURN7   0x587

#define ID_TX_MOTOR_MOVE2   0x602
#define ID_TX_MOTOR_MOVE4   0x604
#define ID_TX_MOTOR_MOVE6   0x606
#define ID_TX_MOTOR_MOVE8   0x608

#define ID_RX_MOTOR_MOVE2   0x582
#define ID_RX_MOTOR_MOVE4   0x584
#define ID_RX_MOTOR_MOVE6   0x586
#define ID_RX_MOTOR_MOVE8   0x588

#define ID_TX_MOTOR_BED9     0x609
#define ID_TX_MOTOR_BED10    0x60a

#define ID_RX_MOTOR_BED9     0x589
#define ID_RX_MOTOR_BED10    0x58a

#define ERRVALUE0   200
#define ERRVALUE1   50

#define MaxMovingSpeed    2000    //电机走最快速度
#define TRUNERR  20

#define WheelRoundLenght  691.0000  //690    轮子周长定义
#define WheelRoundCount   100000.0000 //201400   轮子一周马盘基数  减速比1:50 轮子转一周500*4*50




typedef enum{MODE_POS = 0, MODE_SPEED , MODE_HOME}TYPEDEF_MOTER_MODE;
typedef struct TYPEDEF_TURN_MOTRO_POS
{
		long pos1;
	  long pos3;
	  long pos5;
	  long pos7;
}TYPEDEF_TURN_MOTRO_POS;
typedef struct TYPEDEF_MOVE_MOTRO_POS
{
		long pos2;
	  long pos4;
	  long pos6;
	  long pos8;
}TYPEDEF_MOVE_MOTRO_POS;
void hal_process_control(void);


/*
******************************************************************************************************
                                    电机控制基本函数
******************************************************************************************************
*/
bool MotorReadIO(u32 sendid , u32 filterid , u32* pvalue);
bool MotorStop(u32 sendid , u32 filterid);
bool MotorSetMode(u32 sendid , u32 filterid , TYPEDEF_MOTER_MODE  mode);

bool MotorEnable(u32 sendid , u32 filterid);
bool MotorDisEnable(u32 sendid , u32 filterid );

bool MotorReadEncoder (u32 sendid , u32 filterid , long* rpos);
bool MotorClearEncoder(u32 sendid , u32 filterid);

bool MotorVelocityRun(u32 sendid , u32 filterid , long speed);
bool MotorPositionRun(u32 sendid , u32 filterid , long position , long speed);
bool MotorHomeRun(u32 sendid , u32 filterid);

bool MotorSetACC(u32 sendid , u32 filterid ,long acc);
bool MotorSetDCC(u32 sendid , u32 filterid ,long dcc);

bool MotorReadError(u32 sendid , u32 filterid , u32 *perr);
bool MotorClearError(u32 sendid , u32 filterid);

bool MotorBedStop(u32 sendid , u32 filterid);
bool MotroBedPositionRun(u32 sendid , u32 filterid , long position,long speed);


bool ElmoMotorHome(u32 sendid , u32 filterid);

/*
******************************************************************************************************
                                    功能函数
******************************************************************************************************
*/
/*床板控制运动函数*/
void ResetZeroBedUp(void);
void ResetZeroBedDown(void);
void StopBed(void);
long ReadBedPos(void);
void RunBed(long pos);
void CMDReadBed(void);
void CMDClearBed(void);
bool  ContrlBedUpDown(u32 position);
/*前后不寻迹运动函数*/
void MoveFBSpeedSet(long speed);
void MoveFBSpeedChange(long speed);
void TurnToAngleVal(u16 angle);
void ZeroMoveMotorEncoder(void);
void RevTurnPosition(TYPEDEF_TURN_MOTRO_POS * turnpos);
void RevMovePosition(TYPEDEF_MOVE_MOTRO_POS * movepos);
bool CheckTurnAngleVal(u16 angle);
long DecelerationDistance(long maxspeed);
bool  ContrlRunFB(s8 dir , u32 runlength , u16 maxspeed , u16 minspeed);
/*左右寻迹运动函数*/
void MoveLRSpeedSet(long speed);
void MoveLRSpeedChange(long speed);	
bool  ContrlRunLR(s8 dir , u32 runlength , u16 maxspeed , u16 minspeed);
/*旋转运动函数*/
void MoveRorateSpeedSet(long speed);
void MoveRorateSpeedChange(long speed);
bool ContrlRunRorate(s8 dir , u32 runlength , s32 maxspeed , s32 minspeed);


/*前后寻迹到下一限位运动函数*/
bool  ContrlRunLimitedFB(s8 dir , u32 runlength , s32 maxspeed , s32 minspeed ,u16 golimit);
bool  ContrlRunLimitedLR(s8 dir , u32 runlength , s32 maxspeed , s32 minspeed ,u16 golimit);

/*左右寻迹到下一限位运动函数*/




void ForwardWheel(double turnpos);  
void BackwardWheel(double turnpos);	
void RightWheel(double turnpos);
void LeftWheel(double turnpos);
void MotorMoveStop(void);
void MotorTurnStop(void);
void MotorStopAll(void);
void MotorDisableAll(void);

void TestTurnMotor(void);
void TestMoveMotor(void);
void TestBedMotor(void);
void ResetZeroWheel(void);


extern bool g_stopflag;
extern bool g_activerdy;
#endif



