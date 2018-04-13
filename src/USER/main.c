/*
*********************************************************************************************************
*
*                                            	MAIN CODE
*
*                                             main_c.c
*
* Filename      : main_c.c
* Version       : V1.00
* Programmer(s) : wjh
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/
#include "main.h"
#include "hal_process_SERVES.h"
#include "hal_process_COLLECT.h"
#include "hal_process_CONTROL.h"
#include "hal_process_DEBUG.h"
#include "LwIP.h"

/*
*********************************************************************************************************
*                                          GLOBAL VARIABLES
*********************************************************************************************************
*/
OS_EVENT							*DBGMutexSem;							// DBG printf输出调试信息互斥信号量
OS_EVENT							*NetMutexSem;             // 网络输出互斥量
OS_EVENT							*USART1Mbox;						  // 串口1邮箱
OS_EVENT							*USART2Mbox;						  // 串口2邮箱
OS_EVENT							*USART3Mbox;						  // 串口3邮箱
OS_EVENT							*UART4Mbox;						    // 串口4邮箱
OS_EVENT							*UART5Mbox;						    // 串口5邮箱
OS_EVENT							*CANItMbox;						    // can中断消息
OS_EVENT							*NetCmdMbox;						  // 网络命令
OS_EVENT							*NetOutMbox;						  // 网络命令
OS_EVENT							*DebugMbox;						    // 调试
/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/
static  OS_STK        App_TaskSTARTStk[APP_CFG_TASK_START_STK_SIZE];
static  OS_STK        App_TaskLWIPStk[APP_CFG_TASK_LWIP_STK_SIZE];
static  OS_STK        App_TaskToggleLEDStk[APP_CFG_TASK_ToggleLED_STK_SIZE];
static  OS_STK        App_TaskDataCollectStk[APP_CFG_TASK_DataCollect_STK_SIZE];
static  OS_STK        App_TaskControlStk[APP_CFG_TASK_CONTROL_STK_SIZE];
static  OS_STK        App_TaskDebugStk[APP_CFG_TASK_DEBUG_STK_SIZE];

/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/
static void HAL_INIT_ALL(void);
static void App_EventCreate(void);                                          
static void App_TaskCreate(void);
static void App_TaskSTART (void *p_arg);
static void App_TaskLWIP (void *p_arg);
static void App_TaskToggleLED (void *p_arg);
static void App_TaskDataCollect(void *p_arg);
static void AppTaskControl (void *p_arg);
static void AppTaskDebug (void *p_arg);
/*
*********************************************************************************************************
*                                             MAIN()
*********************************************************************************************************
*/


int main(void)
{
	
	  BSP_CPU_DisAllInterupt();

	  HAL_INIT_ALL();

    OSInit();                                                   /* Initialize "uC/OS-II, The Real-Time Kernel".         */

    OSTaskCreateExt((void (*)(void *)) App_TaskSTART,           /* Create the start task.                               */
									 (void          * ) 0,
									 (OS_STK        * )&App_TaskSTARTStk[APP_CFG_TASK_START_STK_SIZE - 1],
									 (INT8U           ) APP_CFG_TASK_START_PRIO,
									 (INT16U          ) APP_CFG_TASK_START_PRIO,
									 (OS_STK        * )&App_TaskSTARTStk[0],
									 (INT32U          ) APP_CFG_TASK_START_STK_SIZE,
									 (void          * )0,
									 (INT16U          )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));
    OSStart();                                                  /* Start multitasking (i.e. give control to uC/OS-II).  */

    return ( 0 );
}


static void App_TaskSTART (void *p_arg)
{
    OS_CPU_SysTickInit();                                       /* Initialize the SysTick.*/
	  
#if (OS_TASK_STAT_EN > 0)
    OSStatInit();                                               /* Determine CPU capacity*/
#endif
    //IWDG_ReloadCounter();	
    App_EventCreate();                                          /* Create application events*/
	  Init_LwIP();
    App_TaskCreate();                                           /* Create application tasks.*/ 
}
	
	
static void App_EventCreate(void)
{
	  DBGMutexSem	=	OSSemCreate(1);									// DBG printf输出调试信息互斥信号量
	  NetMutexSem	=	OSSemCreate(1);									// DBG printf输出调试信息互斥信号量	
	
	  USART1Mbox = OSMboxCreate((void *)0);
	  USART2Mbox = OSMboxCreate((void *)0);
	  USART3Mbox = OSMboxCreate((void *)0);
	  UART4Mbox = OSMboxCreate((void *)0);
	  UART5Mbox = OSMboxCreate((void *)0);
	  CANItMbox =	OSMboxCreate((void *)0);
    NetCmdMbox = 	OSMboxCreate((void *)0);
	  NetOutMbox = 	OSMboxCreate((void *)0);
		DebugMbox = 	OSMboxCreate((void *)0);
}
static void App_TaskCreate(void)
{
	  OS_CPU_SR cpu_sr=0;
	  OS_ENTER_CRITICAL();			//进入临界区(无法被中断打断)

  	OSTaskCreateExt((void (*)(void *)) App_TaskLWIP,                   /* Create the start task                                    */
                    (void           *) 0,
                    (OS_STK         *)&App_TaskLWIPStk[APP_CFG_TASK_LWIP_STK_SIZE-1],
                    (INT8U           ) APP_CFG_TASK_LWIP_PRIO,
                    (INT16U          ) APP_CFG_TASK_LWIP_PRIO,
                    (OS_STK         *)&App_TaskLWIPStk[0],
                    (INT32U          ) APP_CFG_TASK_LWIP_STK_SIZE,
                    (void           *) 0,
                    (INT16U          )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));
										
    OSTaskCreateExt((void (*)(void *)) App_TaskToggleLED,             /* Create the start task                                    */
                    (void           *) 0,
                    (OS_STK         *)&App_TaskToggleLEDStk[APP_CFG_TASK_ToggleLED_STK_SIZE - 1],
                    (INT8U           ) APP_CFG_TASK_TOGGLELED_PRIO,
                    (INT16U          ) APP_CFG_TASK_TOGGLELED_PRIO,
                    (OS_STK         *)&App_TaskToggleLEDStk[0],
                    (INT32U          ) APP_CFG_TASK_ToggleLED_STK_SIZE,
                    (void           *) 0,
                    (INT16U          )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));
										
    OSTaskCreateExt((void (*)(void *)) App_TaskDataCollect,                /* Create the start task                                    */
                    (void           *) 0,
                    (OS_STK         *)&App_TaskDataCollectStk[APP_CFG_TASK_DataCollect_STK_SIZE-1],
                    (INT8U           ) APP_CFG_TASK_DATACOLLECT_PRIO,
                    (INT16U          ) APP_CFG_TASK_DATACOLLECT_PRIO,
                    (OS_STK         *)&App_TaskDataCollectStk[0],
                    (INT32U          ) APP_CFG_TASK_DataCollect_STK_SIZE,
                    (void           *) 0,
                    (INT16U          )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));

    OSTaskCreateExt((void (*)(void *)) AppTaskControl,                /* Create the start task                                    */
                    (void           *) 0,
                    (OS_STK         *)&App_TaskControlStk[APP_CFG_TASK_CONTROL_STK_SIZE-1],
                    (INT8U           ) APP_CFG_TASK_CONTROL_PRIO,
                    (INT16U          ) APP_CFG_TASK_CONTROL_PRIO,
                    (OS_STK         *)&App_TaskControlStk[0],
                    (INT32U          ) APP_CFG_TASK_CONTROL_STK_SIZE,
                    (void           *) 0,
                    (INT16U          )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));
																		
    OSTaskCreateExt((void (*)(void *)) AppTaskDebug,                /* Create the start task                                    */
                    (void           *) 0,
                    (OS_STK         *)&App_TaskDebugStk[APP_CFG_TASK_DEBUG_STK_SIZE-1],
                    (INT8U           ) APP_CFG_TASK_DEBUG_PRIO,
                    (INT16U          ) APP_CFG_TASK_DEBUG_PRIO,
                    (OS_STK         *)&App_TaskDebugStk[0],
                    (INT32U          ) APP_CFG_TASK_DEBUG_STK_SIZE,
                    (void           *) 0,
                    (INT16U          )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));
										
    OSTaskDel(APP_CFG_TASK_START_PRIO);
	  OS_EXIT_CRITICAL();				//退出临界区(可以被中断打断)
	
}
static void App_TaskLWIP (void *p_arg)
{
		tcp_server_process();
}

static void App_TaskToggleLED (void *p_arg)
{
//    static u8 buf[1024] = {0};
//		const char *ptx = "wangjinhui";
	  static u32 updatatime = 0;  
    OSTmr_Count_Start(&updatatime);


  	while(1)
		{

//			 //myPrintf("TIMcnt:%d    TIM4cnt:%d\r\n" ,(short)TIM_GetCounter(TIM3),(short)TIM_GetCounter(TIM4));
			  //if( ABS( ABS(short)TIM_GetCounter(TIM3) - (short)TIM_GetCounter(TIM4)))
//			 BSP_Write_DOUT1_24(HC595_DOUT_9 | HC595_DOUT_10 ,1);
//			 OSTimeDly(2000);
//        BSP_Write_DOUT1_24(HC595_DOUT_9 | HC595_DOUT_10 ,0);
//			 OSTimeDly(2000);
//       BSP_Write_DOUT1_24(HC595_DOUT_9 | HC595_DOUT_10 ,0);
//			 OSTimeDly(2000);
//			BSP_Write_DOUT1_24(HC595_DOUT_ALL ,0);
	//		IWDG_ReloadCounter();
			//BSP_Write_DOUT1_24(HC595_DOUT_1 ,1);
			OSTimeDly(2000);
			myPrintf("App_TaskToggleLED\r\n" );
			//BSP_Write_DOUT1_24(HC595_DOUT_1 ,0);
//				if(OSTmr_Count_Get(&updatatime) > 1000) 
//				{
//						u8 tmp[30] = {0x01,0x30};
//						u8 tbuf[50] = {0};
//						u8 slen = 0;
//						memcpy(&tmp[2],(u8*)&g_data_acousto_optic , 17);
//						slen = PackData(tmp , 19 ,tbuf);//组包应答 准备就绪
//						Mute_Net_Write(tbuf , slen );
//						OSTimeDlyHMSM(0,0,0,500);				
//            tmp[0] = 0x02;
//						memcpy(&tmp[2],(u8*)&g_receive11 , 19);
//						slen = PackData(tmp , 21 ,tbuf);//组包应答 准备就绪
//						Mute_Net_Write(tbuf , slen );
//						OSTmr_Count_Start(&updatatime);						
//			  }			
		}
}


static void App_TaskDataCollect (void *p_arg)
{

		hal_process_collect();
}

static void AppTaskControl (void *p_arg)
{
	  hal_process_control();
}

static void AppTaskDebug (void *p_arg)
{
	  hal_process_debug();
}


void HAL_INIT_ALL(void)
{

		//初始化时钟源
		BSP_RCC_Configuration();		
		//初始化中断
		BSP_NVIC_Configuration();
		//初始化普通输入输出I/O
		BSP_GPIO_Init();
	  //看门狗初始化
	  BSP_WDTDOG_Init();
	  IWDG_ReloadCounter();
    //串口初始化
    BSP_USART_Config(USART1,19200);
	  BSP_USART_Config(USART2,19200);
	  BSP_USART_Config(USART3, 9600);
	  BSP_USART_Config(UART4 , 9600);
	  BSP_USART_Config(UART5 ,115200);
    //定时器初始化
	  BSP_TIM3_Init();
	  BSP_TIM4_Init();
    //CAN初始化
	  BSP_CANX_Init(CAN1,DEF_CAN500K);
		CAN_ITConfig( CAN1, CAN_IT_FMP0, ENABLE );  //开中断 
	  //SPI
		BSP_SPI1_Init();

	  //ETH初始化
		BSP_EthernetInit();
	  BSP_Ethernet_Configuration();
		//系统参数区读取参数
		BSP_Read_Para(&g_system_para);

}
