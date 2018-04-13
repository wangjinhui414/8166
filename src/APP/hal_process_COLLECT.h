#ifndef  _HAL_PROCESS_COLLECT_
#define  _HAL_PROCESS_COLLECT_

#include "bsp.h"
#include "ucos_ii.h"
#include "bsp_usart.h"
typedef struct  TYPEDEF_ACOUSTO_OPTIC
{
	 u16 _wave[8];
	 u8  _led;

}TYPEDEF_ACOUSTO_OPTIC;

typedef struct  TYPEDEF_LINE_BOARD
{
	 u16 _board1[4];
	 u16 _board2[4];
	 u16  _board3[4];
	 u16  _board4[4];	
}TYPEDEF_LINE_BOARD;



 //全局采集数据变量
extern u32                                g_movelimitcout;
extern bool                               g_bedlimitflag ;
extern TYPEDEF_ACOUSTO_OPTIC              g_data_acousto_optic;//声光板数据
extern u8 g_receive11[19];

void hal_process_collect(void);

 void AvoidObstacleData1(TYPEDEF_USART_BUFF *data);
 void AvoidObstacleData2(TYPEDEF_USART_BUFF *data);
 void PatrolDataDispose(TYPEDEF_USART_BUFF *data);
#endif


