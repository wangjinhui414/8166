#include "bsp_usart.h"
#include "hal_process_COLLECT.h"

#define RS485_TX     GPIO_SetBits(GPIOB, BSP_GPIOB_DIR);
#define RS485_RX     GPIO_ResetBits(GPIOB, BSP_GPIOB_DIR);

//u8 RS485_RX_BUF[100] = {0};
//u8 RS485_RX_CNT = 0;
TYPEDEF_USART_BUFF   usart3_rx = {0};
TYPEDEF_USART_BUFF   usart1_rx = {0};
TYPEDEF_USART_BUFF   usart2_rx = {0};
TYPEDEF_USART_BUFF   uart4_rx = {0};
TYPEDEF_USART_BUFF   uart5_rx = {0};
/*
*********************************************************************************************************
*                                             BSP_USART_Config()
*
* Description : USART Config
*
* Argument(s) : USARTx , baud
*
* Return(s)   : none.
*
* Note(s)     : 
*********************************************************************************************************
*/
void BSP_USART_Config(USART_TypeDef* USARTx,u32 baud)
{
    USART_InitTypeDef USART_InitStructure;

	USART_ClockInitTypeDef  usart_clk_init;
	    usart_clk_init.USART_Clock           = USART_Clock_Disable;
    usart_clk_init.USART_CPOL            = USART_CPOL_Low;
    usart_clk_init.USART_CPHA            = USART_CPHA_2Edge;
    usart_clk_init.USART_LastBit         = USART_LastBit_Disable;
	
	
    USART_InitStructure.USART_BaudRate = baud;					
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;	
    USART_InitStructure.USART_StopBits = USART_StopBits_1;			
    USART_InitStructure.USART_Parity = USART_Parity_No;				
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;   
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;		

    USART_ClockInit(USARTx, &usart_clk_init);	
    /* Configure USART1 */
    USART_Init(USARTx, &USART_InitStructure);						 
    /* Enable USART1 Receive and Transmit interrupts */
    USART_ITConfig(USARTx, USART_IT_RXNE, ENABLE);                   
    /* Enable the USART1 */

	
				USART_ClearITPendingBit(USARTx, USART_IT_RXNE);
				USART_ClearITPendingBit(USARTx, USART_IT_TXE);
    USART_Cmd(USARTx, ENABLE);	
}


/*
*********************************************************************************************************
*                                  PUTCHAR_PROTOTYPE
*
* Description: 
*
* Argument(s): none.
*
* Return(s)  : none.
*********************************************************************************************************
*/
int fputc(int ch, FILE *f) 	// Printf输出到MDK Printf窗口时使用
{
		while(USART_GetFlagStatus(UART5,USART_FLAG_TC)==RESET){}; 
    USART_SendData(UART5,(uint8_t)ch);   
	  return ch;
}




/*
**********************************************************************
串口5中断函数
*********************************************************************/
u32 g_u5time = 0;
void UART5_IRQHandler(void)
{
		OS_CPU_SR  cpu_sr;
		OS_ENTER_CRITICAL();
		OSIntNesting++;
		OS_EXIT_CRITICAL();
	 
	  g_u5time = OSTimeGet();
		if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET) //接收到数据
		{   
				uart5_rx._buf[uart5_rx.cnt++] = USART_ReceiveData(UART5);	//记录接收到的值

		}	
		OSIntExit();	
}
void ClearUart5Buff(void)
{
	memset(uart5_rx._buf , 0 , MAX_BUFF_SIZE );
	uart5_rx.cnt = 0;
}


/*
**********************************************************************
串口1中断函数
*********************************************************************/      
void USART1_IRQHandler(void)
{
	  u8 res;
		OS_CPU_SR  cpu_sr;
		OS_ENTER_CRITICAL();
		OSIntNesting++;
		OS_EXIT_CRITICAL();

	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) //接收到数据
	{   
		res =USART_ReceiveData(USART1);                    //读取接收到的数据
		if(usart1_rx.cnt<14)
		{
			usart1_rx._buf[usart1_rx.cnt]=res;		//记录接收到的值
			if(usart1_rx._buf[0]==0x2b)
			{
				if((usart1_rx._buf[1]==0x36)||(usart1_rx._buf[1]==0x37))
				{
					if((usart1_rx.cnt==13)&(res==0x0d))//接收完成
					{
						OSMboxPost(USART1Mbox, (void *)&usart1_rx);
					}
				}
				else
				{
					usart1_rx.cnt=0;	
				}
				usart1_rx.cnt++;						//接收数据增加1
			}
			else if((usart1_rx._buf[0]==0x31)||(usart1_rx._buf[0]==0x32))
			{
			
				if((usart1_rx.cnt==3)&(res==0x3a))//接收完成
				{
					usart1_rx.cnt=0;	
				}
				else
				{
					usart1_rx.cnt=0;	
				}
				usart1_rx.cnt++;					
			}
			else
			{
				 usart1_rx.cnt=0;
			}
		}
		else
		{
			 usart1_rx.cnt=0;			
		}
		   
	}	
	OSIntExit();
                          
}


/*
**********************************************************************
串口2中断函数
*********************************************************************/ 
void USART2_IRQHandler(void)
{
	  u8 res;
		OS_CPU_SR  cpu_sr;
		OS_ENTER_CRITICAL();
		OSIntNesting++;
		OS_EXIT_CRITICAL();

	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) //接收到数据
	{   
		res =USART_ReceiveData(USART2);                    //读取接收到的数据
		if(usart2_rx.cnt<14)
		{
			usart2_rx._buf[usart2_rx.cnt]=res;		//记录接收到的值
			if(usart2_rx._buf[0]==0x2b)
			{
				if((usart2_rx._buf[1]==0x36)||(usart2_rx._buf[1]==0x37))
				{
					if((usart2_rx.cnt==13)&(res==0x0d))//接收完成
					{
						OSMboxPost(USART2Mbox, (void *)&usart2_rx);
					}
				}
				else
				{
					usart2_rx.cnt=0;	
				}
				usart2_rx.cnt++;						//接收数据增加1
			}
			else if((usart2_rx._buf[0]==0x31)||(usart2_rx._buf[0]==0x32))
			{
			
				if((usart2_rx.cnt==3)&(res==0x3a))//接收完成
				{
					usart2_rx.cnt=0;	
				}
				else
				{
					usart2_rx.cnt=0;	
				}
				usart2_rx.cnt++;					
			}
			else
			{
				 usart2_rx.cnt=0;
			}
		}
		else
		{
			usart2_rx.cnt=0;
		}
		   
	}	
	OSIntExit();
		
}

void USART3_IRQHandler(void)
{
		OS_CPU_SR  cpu_sr;
		OS_ENTER_CRITICAL();
		OSIntNesting++;
		OS_EXIT_CRITICAL();	
    	
		if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) //接收到数据
		{	 
			u8 res = 0;		 
			res =USART_ReceiveData(USART3); 	//读取接收到的数据
			//40 31 00 00 00 00 00 00 0d
			if(usart3_rx.cnt<9)
			{
				usart3_rx._buf[usart3_rx.cnt]=res;		//记录接收到的值
				if(usart3_rx._buf[0]==0x40)
				{
					if((usart3_rx._buf[1]==0x31)||(usart3_rx._buf[1]==0x32)||(usart3_rx._buf[1]==0x33)||(usart3_rx._buf[1]==0x34)||(usart3_rx._buf[1]==0x35)||(usart3_rx._buf[1]==0x36))
					{
						if((usart3_rx.cnt==8)&(res==0x0d))//接收完成
						{
							//usart3_rx.cnt=0;
							OSMboxPost(USART3Mbox, (void *)&usart3_rx);
							 //PatrolDataDispose(&usart3_rx);
						}
					}
					else
					{
						usart3_rx.cnt=0;	
					}
					usart3_rx.cnt++;						//接收数据增加1
				}
				else
				{
					 usart3_rx.cnt=0;
				}
			}
      else
			{
					usart3_rx.cnt=0;
			}				
		}  

		OSIntExit();	 											 
}

void BSP_RS485_SendData (USART_TypeDef* USARTx , u8 *src ,u8 len )    
{    
	  RS485_TX;
    while (0 != len--)   
    {    
        USART_SendData(USARTx, *src++);  
        while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET){}       
    } 
	  RS485_RX;				
} 

void BSP_RS232_SendData (USART_TypeDef* USARTx , u8 *src ,u8 len)    
{    
    while (0 != len--)    
    {    
        USART_SendData(USARTx, *src++);  
        while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET){}      
    } 			
} 


