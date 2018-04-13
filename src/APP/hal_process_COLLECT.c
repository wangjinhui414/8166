#include "hal_process_COLLECT.h"
#include "hal_process_SERVES.h"
//全局采集数据变量
u32                                g_movelimitcout = 0;
bool                               g_bedlimitflag = FALSE;
TYPEDEF_ACOUSTO_OPTIC              g_data_acousto_optic = {0};//声光板数据

//u8                                 g_data_volage[3] = {0};

u8 g_receive11[19];
//数据采样频率
#define ULTRASONIC_FRE       500
#define BATVOLTAGE_FRE       1000
#define LASER4_FRE           200
#define MOVELIMIT_FRE        30
#define BEDLIMIT_FRE         30
#define BEDLINE_FRE          50

//串口采集指令
const u8 cmd_ultrasonic[4] = {0x2a,0x2a,0x2a,0x06};//查询超声
const u8 cmd_voltage[4] = {0x50,0x50,0x50,0xaa};//查询电压
const u8 cmd_laser_v1[] = "###a";
const u8 cmd_laser_v2[] = "$$$b";
const u8 cmd_laser_h1[] = "%%%c";
const u8 cmd_laser_h2[] = "&&&d";


/*
***********************************************************************************
                     传感器串口数据采集主任务

***********************************************************************************
*/
void hal_process_collect(void)
{
	  u8 err = 0;
	  u8 lastmove_limit = 1;
		static u32 ultrasonic_time = 0;
		static u32 voltage_time = 0;
	  static u32 laser4_time = 0;
	  static u32 movelimit_time = 0;
	  static u32 bedlimit_time = 0;
    static u32 bedline_time = 0;
	  static u32 updatatime = 0; 
		TYPEDEF_USART_BUFF *pusart_rx = NULL;
	
	   
    OSTmr_Count_Start(&updatatime);	
		OSTmr_Count_Start(&ultrasonic_time);
		OSTmr_Count_Start(&voltage_time);
	  OSTmr_Count_Start(&laser4_time);
	  OSTmr_Count_Start(&movelimit_time);
	  OSTmr_Count_Start(&bedlimit_time);
    OSTmr_Count_Start(&bedline_time);

		while(1)
		{
			  /*拉伸传感器紧急抱闸*/
			  if(OSTmr_Count_Get(&bedline_time) > BEDLINE_FRE)
        {
						if(ABS(ABS((short)TIM_GetCounter(TIM3)) - ABS((short)TIM_GetCounter(TIM4))) > 500)
						{
							 DEF_DISENABLE_KOER;
						}							
						OSTmr_Count_Start(&bedline_time);
			  }

        /*床板限位采集*/	
			  if(OSTmr_Count_Get(&bedlimit_time) > BEDLIMIT_FRE)
        {				
						if(BSP_Read_DIN1_8(BSP_GPIOE_DIN2))
						{
							 g_bedlimitflag = TRUE;
						}
						else
						{
							 g_bedlimitflag = FALSE;
						}
						OSTmr_Count_Start(&bedlimit_time);
			  } 
				/*行走限位计数*/
			  if(OSTmr_Count_Get(&movelimit_time) > MOVELIMIT_FRE)
        {				
						if(BSP_Read_DIN1_8(BSP_GPIOE_DIN3) && !lastmove_limit)//上次限位为0  这次限位为1 则计数一次限位值
						{
							 g_movelimitcout++;
						}
						lastmove_limit = BSP_Read_DIN1_8(BSP_GPIOE_DIN3);
						//myPrintf("movelimit:%d\r\n",g_movelimitcout);
						OSTmr_Count_Start(&movelimit_time);						
			  }				
        
			  /*USART1 USART2 232 声光数据采集*/
				if(OSTmr_Count_Get(&ultrasonic_time) > ULTRASONIC_FRE) 
				{
						BSP_RS232_SendData(USART1 , (u8*)cmd_ultrasonic ,4);
						pusart_rx = NULL;
						pusart_rx = ( TYPEDEF_USART_BUFF * )OSMboxPend( USART1Mbox , 200 , &err);
						if(pusart_rx != NULL)
						{
								AvoidObstacleData1(pusart_rx);
						}
						BSP_RS232_SendData(USART2 , (u8*)cmd_ultrasonic ,4);
						pusart_rx = NULL;
						pusart_rx = ( TYPEDEF_USART_BUFF * )OSMboxPend( USART2Mbox , 200 , &err);
						if(pusart_rx != NULL)
						{
								AvoidObstacleData2(pusart_rx);
						}					
						OSTmr_Count_Start(&ultrasonic_time);
				}
				/*USART3 485总线电压数据采集*/
				if(OSTmr_Count_Get(&voltage_time) > BATVOLTAGE_FRE) 
				{
						BSP_RS485_SendData(USART3 , (u8*)cmd_voltage,4);
						pusart_rx = NULL;
						pusart_rx = ( TYPEDEF_USART_BUFF * )OSMboxPend( USART3Mbox , 200 , &err);
						if(pusart_rx != NULL)
						{
							 PatrolDataDispose(pusart_rx);
						}					
						OSTmr_Count_Start(&voltage_time);
				}
				/*USART3 485总线巡线数据采集*/
				if(OSTmr_Count_Get(&laser4_time) > LASER4_FRE)
				{
						BSP_RS485_SendData(USART3 , (u8*)cmd_laser_v1,4);
						pusart_rx = NULL;
						pusart_rx = ( TYPEDEF_USART_BUFF * )OSMboxPend( USART3Mbox , 200 , &err);
						if(pusart_rx != NULL)
						{
							 PatrolDataDispose(pusart_rx);
						}
						
						BSP_RS485_SendData(USART3 , (u8*)cmd_laser_v2,4);
						pusart_rx = NULL;
						pusart_rx = ( TYPEDEF_USART_BUFF * )OSMboxPend( USART3Mbox , 200 , &err);
						if(pusart_rx != NULL)
						{
							 PatrolDataDispose(pusart_rx);
						}
						
						BSP_RS485_SendData(USART3 , (u8*)cmd_laser_h1,4);
						pusart_rx = NULL;
						pusart_rx = ( TYPEDEF_USART_BUFF * )OSMboxPend( USART3Mbox , 200 , &err);
						if(pusart_rx != NULL)
						{
							 PatrolDataDispose(pusart_rx);
						}
						
						BSP_RS485_SendData(USART3 , (u8*)cmd_laser_h2,4);
						pusart_rx = NULL;
						pusart_rx = ( TYPEDEF_USART_BUFF * )OSMboxPend( USART3Mbox , 200 , &err);
						if(pusart_rx != NULL)
						{
							 PatrolDataDispose(pusart_rx);
						}					
					
						OSTmr_Count_Start(&laser4_time);
				}
				if(OSTmr_Count_Get(&updatatime) > 1000) 
				{
						u8 tmp[30] = {0x01,0x30};
						u8 tbuf[50] = {0};
						u8 slen = 0;
						memcpy(&tmp[2],(u8*)&g_data_acousto_optic , 17);
						slen = PackData(tmp , 19 ,tbuf);//组包应答 准备就绪
						Mute_Net_Write(tbuf , slen );
					  //OSTimeDlyHMSM(0,0,0,50);				
            tmp[0] = 0x02;
						memcpy(&tmp[2],(u8*)&g_receive11 , 19);
						slen = PackData(tmp , 21 ,tbuf);//组包应答 准备就绪
						Mute_Net_Write(tbuf , slen );
						OSTmr_Count_Start(&updatatime);						
			  }
				OSTimeDlyHMSM(0,0,0,20);
		}
}

/*************************************************************************************************
函数功能：超声避障板1数据处理函数
data：接收数据首地址
*************************************************************************************************/
void AvoidObstacleData1(TYPEDEF_USART_BUFF *data)
{
		u16 check;
		check= SumCheck16Pro(data->_buf , data->cnt - 3);
		if( check == (data->_buf[data->cnt - 3]<<8 | data->_buf[data->cnt - 2]) )
		{
			if((data->_buf[0]==0x2b)&(data->_buf[1]==0x36))
			{
				g_data_acousto_optic._wave[0] = data->_buf[2]<<8 | data->_buf[3];
				g_data_acousto_optic._wave[1] = data->_buf[4]<<8 | data->_buf[5];
				g_data_acousto_optic._wave[2] = data->_buf[6]<<8 | data->_buf[7];
				g_data_acousto_optic._wave[3] = data->_buf[8]<<8 | data->_buf[9];
				g_data_acousto_optic._led &= 0xF0;
				g_data_acousto_optic._led |=  (data->_buf[10] & 0x0F);
			}
		}
#if DEF_DEBUG_CFG & DEF_DEBUG_232		
		myPrintf("waveled1-----1:%04X  2:%04X  3:%04X  4:%04X  LED1:%02X\r\n" , g_data_acousto_optic._wave[0] , g_data_acousto_optic._wave[1] , 
		                                                                       g_data_acousto_optic._wave[2] , g_data_acousto_optic._wave[3] , g_data_acousto_optic._led&0x0F);
#endif		
		memset(data->_buf , 0 ,data->cnt);
		data->cnt = 0;
}
/*************************************************************************************************
函数功能：超声避障板2数据处理函数
data：接收数据首地址
*************************************************************************************************/
void AvoidObstacleData2(TYPEDEF_USART_BUFF *data)
{
		u16 check;
		check= SumCheck16Pro(data->_buf , data->cnt - 3);
		if( check == (data->_buf[data->cnt - 3]<<8 | data->_buf[data->cnt - 2]) )
		{
			if((data->_buf[0]==0x2b)&(data->_buf[1]==0x36))
			{
				g_data_acousto_optic._wave[4] = data->_buf[2]<<8 | data->_buf[3];
				g_data_acousto_optic._wave[5] = data->_buf[4]<<8 | data->_buf[5];
				g_data_acousto_optic._wave[6] = data->_buf[6]<<8 | data->_buf[7];
				g_data_acousto_optic._wave[7] = data->_buf[8]<<8 | data->_buf[9];
				g_data_acousto_optic._led &= 0x0F;
				g_data_acousto_optic._led |=  (data->_buf[10]<<4 & 0xF0);
			}
		}
#if DEF_DEBUG_CFG & DEF_DEBUG_232			
		myPrintf("waveled2-----1:%04X  2:%04X  3:%04X  4:%04X  LED2:%02X\r\n" , g_data_acousto_optic._wave[4] , g_data_acousto_optic._wave[5] , 
		                                                                        g_data_acousto_optic._wave[6] , g_data_acousto_optic._wave[7] , g_data_acousto_optic._led&0xF0);
#endif		
		memset(data->_buf , 0 ,data->cnt);
		data->cnt = 0;	
}

/*****************************************************************************************
函数功能：485接收数据处理
*data：485接收数据buf
*****************************************************************************************/

#define  CHENK485(x)   (x<0x0a?x+0x30:x+0x37)
void PatrolDataDispose(TYPEDEF_USART_BUFF *data)
{
		u8 check;
	  //assert_param(data->cnt == 8);
		check=XorCheck16Pro(data->_buf , 6);
		if( CHENK485(check/16)==data->_buf[6]  &&  CHENK485(check%16)==data->_buf[7]  &&  data->_buf[0]==0x40)
		{
				switch(data->_buf[1])
				{
			case 0x31:		// 第一块巡线板的数据
						   g_receive11[0]=data->_buf[2];	
						   g_receive11[1]=data->_buf[3];
						   g_receive11[2]=data->_buf[4];
						   g_receive11[3]=data->_buf[5];
#if DEF_DEBUG_CFG & DEF_DEBUG_485				
			         myPrintf("broad1-----%02X %02X %02X  %02X\r\n" , data->_buf[2] , data->_buf[3] , data->_buf[4] , data->_buf[5]);
#endif			
					break;
			case 0x32:		//第二块巡线板的数据
						   g_receive11[4]=data->_buf[2];	
						   g_receive11[5]=data->_buf[3];
						   g_receive11[6]=data->_buf[4];
						   g_receive11[7]=data->_buf[5];
#if DEF_DEBUG_CFG & DEF_DEBUG_485			
   			       myPrintf("broad2-----%02X %02X %02X  %02X\r\n" , data->_buf[2] , data->_buf[3] , data->_buf[4] , data->_buf[5]);
#endif			
					break;
			case 0x33:		//第三块巡线板的数据
						   g_receive11[8]=data->_buf[2];
						   g_receive11[9]=data->_buf[3];
						   g_receive11[10]=data->_buf[4];
						   g_receive11[11]=data->_buf[5];
#if DEF_DEBUG_CFG & DEF_DEBUG_485			
			         myPrintf("broad3-----%02X %02X %02X  %02X\r\n" , data->_buf[2] , data->_buf[3] , data->_buf[4] , data->_buf[5]);
#endif			
					break;
			case 0x34:		//第四块巡线板的数据
						   g_receive11[12]=data->_buf[2];	
						   g_receive11[13]=data->_buf[3];
						   g_receive11[14]=data->_buf[4];
						   g_receive11[15]=data->_buf[5];
#if DEF_DEBUG_CFG & DEF_DEBUG_485				
			         myPrintf("broad4-----%02X %02X %02X  %02X\r\n" , data->_buf[2] , data->_buf[3] , data->_buf[4] , data->_buf[5]);
#endif			
					break;					
					case 0x35:		//电量显示板的数据
						  memcpy(&g_receive11[16] , &data->_buf[3] , 3);
#if DEF_DEBUG_CFG & DEF_DEBUG_485					
              myPrintf("volage-----%d%d.%d V\r\n" , g_receive11[16],g_receive11[17],g_receive11[18]);
#endif					
							break;
					case 0x36:		//查看电池阈值
							break;
					case 0x37:		//设定电池电量报警阈值
							break;
						default:    //数据错误
							break;
				}
	  }
		memset(data->_buf , 0 ,data->cnt);
		data->cnt = 0;		
}
