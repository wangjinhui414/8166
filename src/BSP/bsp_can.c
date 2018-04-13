#include "bsp_can.h"


CanRxMsg g_canrecv_msg = {0};

/*
*********************************************************************************************************
*                                             BSP_CANX_Init()
*
* Description : can init
*
* Argument(s) : canx , u32CanBaud
*
* Return(s)   : none.
*
* Note(s)     : 
*********************************************************************************************************
*/
void BSP_CANX_Init( CAN_TypeDef * canx , u32 u32CanBaud )
{
    CAN_InitTypeDef        CAN_InitStructure;

    CAN_StructInit( &CAN_InitStructure );
    CAN_InitStructure.CAN_TTCM = DISABLE;
    CAN_InitStructure.CAN_ABOM = DISABLE;
    CAN_InitStructure.CAN_AWUM = DISABLE;
    CAN_InitStructure.CAN_NART = DISABLE;
    CAN_InitStructure.CAN_RFLM = DISABLE;
    CAN_InitStructure.CAN_TXFP = DISABLE;
    CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;
    CAN_InitStructure.CAN_SJW = u32CanBaud >> 24;
    CAN_InitStructure.CAN_BS1 = ( u32CanBaud >> 16 ) & 0xff;
    CAN_InitStructure.CAN_BS2 = ( u32CanBaud >> 8 ) & 0xff;
    CAN_InitStructure.CAN_Prescaler = u32CanBaud & 0xff;;
    CAN_Init( canx, &CAN_InitStructure );
}
/*
*********************************************************************************************************
*                                             BSP_CAN_Filter()
*
* Description : BSP_CAN_Filter
*
* Argument(s) : u8FilterGroup , u32FilterId , idmask
*
* Return(s)   : none.
*
* Note(s)     : 
*********************************************************************************************************
*/
void BSP_CAN_Filter(u8 u8FilterGroup, u32 u32FilterId ,u32 idmask)
{
    CAN_FilterInitTypeDef  CAN_FilterInitStructure;
    CAN1->FA1R &= ~( 0x1 << u8FilterGroup );
    CAN_FilterInitStructure.CAN_FilterNumber = u8FilterGroup;
    CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
    CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
    CAN_FilterInitStructure.CAN_FilterIdHigh =
        ( u32FilterId > 0x7ff ) ? ( ( ( u32FilterId << 3 ) & 0xFFFF0000 ) >> 16 ) : ( ( ( u32FilterId << 21 ) & 0xffff0000 ) >> 16 ) ;
    CAN_FilterInitStructure.CAN_FilterIdLow =
        ( u32FilterId > 0x7ff ) ? ( ( ( u32FilterId << 3 ) | CAN_ID_EXT | CAN_RTR_DATA ) & 0xFFFF ) : ( ( ( u32FilterId << 21 ) | CAN_ID_STD | CAN_RTR_DATA ) & 0xffff );
    CAN_FilterInitStructure.CAN_FilterMaskIdHigh = (idmask<<3)>>16;
    CAN_FilterInitStructure.CAN_FilterMaskIdLow = idmask<<3;
    CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FIFO0;
    CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
    CAN_FilterInit( &CAN_FilterInitStructure );
}


bool BSP_CAN_SendMsg(u32 sendid , u32 recvid , u8 *data)
{
  u32 timecout = 0;
	CanTxMsg cantxmsg;
	
  cantxmsg.DLC = 8;
	cantxmsg.StdId = sendid;
	cantxmsg.ExtId = sendid;
	cantxmsg.IDE = ( sendid > 0X7FF ) ? 4 : 0;
	cantxmsg.RTR = 0;
	memcpy(cantxmsg.Data , data , 8);
	
  CANItMbox->OSEventPtr = NULL;//清除邮箱	
	memset(g_canrecv_msg.Data , 0 ,8);
	g_canrecv_msg.StdId = 0;
	BSP_CAN_Filter(0 , recvid , 0xFFFFFFFF);//设置过滤id	
  CAN_ITConfig( CAN1, CAN_IT_FMP0, ENABLE );  //开中断 
  OSTmr_Count_Start(&timecout);
  CAN_Transmit(CAN1, &cantxmsg);	
//  while(0 == CAN_Transmit(CAN1, &cantxmsg))
//	{
//		 if(OSTmr_Count_Get(&timecout) > 2000)
//			  return FALSE;
//	}
#if DEF_DEBUG_CFG & DEF_DEBUG_CAN 
	 myPrintf("Send->  ID:%04X  DATA:%02X %02X %02X %02X %02X %02X %02X %02X \r\n",cantxmsg.StdId , cantxmsg.Data[0] , cantxmsg.Data[1] , cantxmsg.Data[2] , cantxmsg.Data[3] , 
					                                                                      cantxmsg.Data[4] , cantxmsg.Data[5] , cantxmsg.Data[6] , cantxmsg.Data[7]  );
#endif	
  return TRUE;		
}


void CAN1_RX0_IRQHandler(void)
{
		OS_CPU_SR  cpu_sr;
		OS_ENTER_CRITICAL();
		OSIntNesting++;
		OS_EXIT_CRITICAL();		
	
		if( CAN_GetITStatus( CAN1, CAN_IT_FMP0 ) != RESET )
    {
        CAN_Receive( CAN1, CAN_FIFO0, &g_canrecv_msg );
			 CAN_ClearITPendingBit(CAN1,CAN_FLAG_FF0);
			  CAN_ITConfig( CAN1, CAN_IT_FMP0, DISABLE );
        OSMboxPost(CANItMbox, (void *)&g_canrecv_msg);
    }	
	
	  OSIntExit();
}

