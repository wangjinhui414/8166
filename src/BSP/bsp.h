#ifndef _BSP_H_
#define _BSP_H_

#include "stm32f10x.h"
#include 	<string.h>
#include 	<stdio.h>
#include  <time.h>
#include "os_cpu.h"
#include "ucos_ii.h"
#include "stdarg.h"
#include "stm32_eth.h"
#include "main.h"
#include "cfg.h"
/*
*********************************************************************************************************
*                                                 DEFINES
*********************************************************************************************************
*/
/************************COMMON DEFINE***********************/ 
#define	 DEF_ENABLE										(1)
#define	 DEF_DISABLE									(0)
#define	 DEF_SET											(1)
#define	 DEF_CLR											(0)

/*ETH*/
#define  DP83848_PHY            /* Ethernet pins mapped on STM32F107 Board */
#define  PHY_ADDRESS       0x01 /* Relative to STM32F107 Board */
//#define MII_MODE              // STM32F107 connect PHY using MII mode
#define  RMII_MODE              // STM32F107 connect PHY using RMII mode
/************************WDG DEFINE**************************/ 



/* CPU ID num addr define */
#define  DEF_CPU_FLASHID_ADDR    			(0x1FFFF7E8) 		/*CPU inter flash addr*/


typedef enum {FALSE = 0, TRUE = 1} bool;

typedef struct TYPEDEF_PARA
{
	u8  upflag;    //升级标志
	char version[6];   //应用程序版本号
	u32 size;      //应用程序大小
	char spiversion[6];//升级存储区程序版本号
	u32 spisize;	 //升级存储区程序大小
	float km;      //寻线系数
	
}TYPEDEF_PARA;
extern TYPEDEF_PARA   g_system_para;
/************************I/O DEFINES(For GT Series hardware V2.0 100 pin)****************************/
																											
#define  BSP_GPIOA_ADC1					      GPIO_Pin_0			
#define  BSP_GPIOA_ETH_REF_CLK        GPIO_Pin_1      /*ETH_REF_CLK            */	 
#define  BSP_GPIOA_ETH_MDIO           GPIO_Pin_2      /*ETH_MDIO               */	
#define  BSP_GPIOA_ADC2               GPIO_Pin_3    	
#define  BSP_GPIOA_DAC1				        GPIO_Pin_4    	
#define  BSP_GPIOA_DAC2						    GPIO_Pin_5    	
#define  BSP_GPIOA_ADC3					      GPIO_Pin_6    	
#define  BSP_GPIOA_ETH_CRS_DV					GPIO_Pin_7      /*ETH_CRS_DV             */	 	
#define  BSP_GPIOA_ETH_MCO					  GPIO_Pin_8      /*ETH_MCO                */	
#define  BSP_GPIOA_USB_VBUS					  GPIO_Pin_9      /*USB_VBUS               */	
#define  BSP_GPIOA_IO11					      GPIO_Pin_10    	
#define  BSP_GPIOA_USB_DM						  GPIO_Pin_11     /*USB_DM	               */		
#define  BSP_GPIOA_USB_DP  					  GPIO_Pin_12     /*USB_DP	               */		
#define  BSP_GPIOA_JLINK_SWDIO				GPIO_Pin_13     /*Jlink data port        */
#define  BSP_GPIOA_JLINK_SWCLK				GPIO_Pin_14     /*Jlink clock port       */
#define  BSP_GPIOA_SPIFLASH_CS	  		GPIO_Pin_15    	/*SPIFALSH CS            */

                                                      
#define  BSP_GPIOB_ADC4							  GPIO_Pin_0    	
#define  BSP_GPIOB_ADC5							  GPIO_Pin_1    	
#define  BSP_GPIOB_LED_RUN       		  GPIO_Pin_2    	/*LED RUN                */
#define  BSP_GPIOB_SPI1_SCK						GPIO_Pin_3      /*H595 SPI  SCK          */	
#define  BSP_GPIOB_SPI1_MISO					GPIO_Pin_4    	/*H595 SPI  MISO         */
#define  BSP_GPIOB_SPI1_MOSI					GPIO_Pin_5    	/*H595 SPI  MOSI         */
#define  BSP_GPIOB_TXD1						    GPIO_Pin_6    	/*USART1 232 TXD         */
#define  BSP_GPIOB_RXD1						    GPIO_Pin_7    	/*USART1 232 RXD         */
#define  BSP_GPIOB_I2C1_SCL						GPIO_Pin_8    	
#define  BSP_GPIOB_I2C2_SDA						GPIO_Pin_9    	
#define  BSP_GPIOB_IGT					      GPIO_Pin_10    	
#define  BSP_GPIOB_ETH_TX_EN					GPIO_Pin_11    	/*ETH_TX_EN              */
#define  BSP_GPIOB_ETH_TXD0						GPIO_Pin_12    	/*ETH_TXD0               */
#define  BSP_GPIOB_ETH_TXD1						GPIO_Pin_13    	/*ETH_TXD1               */
#define  BSP_GPIOB_EDIN3							GPIO_Pin_14    	
#define  BSP_GPIOB_DIR  					    GPIO_Pin_15     /*USART3 485 DIR port    */	


                                                      
#define  BSP_GPIOC_ADC6						    GPIO_Pin_0    	
#define  BSP_GPIOC_ETH_MDC						GPIO_Pin_1      /*ETH_MDC                */   	
#define  BSP_GPIOC_ADC7					      GPIO_Pin_2    	
#define  BSP_GPIOC_ADC8							  GPIO_Pin_3    	
#define  BSP_GPIOC_ETH_RXD0						GPIO_Pin_4    	/*ETH_RXD0               */  
#define  BSP_GPIOC_ETH_RXD1						GPIO_Pin_5    	/*ETH_RXD1               */  
#define	 BSP_GPIOC_TIM3_IN1						GPIO_Pin_6	    /*TIM3 input capture 1   */		
#define  BSP_GPIOC_TIM3_IN2					  GPIO_Pin_7      /*TIM3 input capture 2   */	
#define  BSP_GPIOC_EDIN7					    GPIO_Pin_8    	
#define  BSP_GPIOC_EDIN8 					    GPIO_Pin_9    	  
#define  BSP_GPIOC_TXD4						    GPIO_Pin_10    	/*UART4 485 TXD          */
#define  BSP_GPIOC_RXD4						    GPIO_Pin_11    	/*UART4 485 TXD          */
#define  BSP_GPIOC_TXD5						    GPIO_Pin_12     /*UART5 TXD  debug com   */		
#define  BSP_GPIOC_TAMPER_RTC					GPIO_Pin_13    	
#define  BSP_GPIOC_OSC32_IN						GPIO_Pin_14    	/*OSC32_IN               */
#define  BSP_GPIOC_OSC32_OUT					GPIO_Pin_15    	/*OSC32_OUT              */

                                                      
#define  BSP_GPIOD_CAN1RX							GPIO_Pin_0    	/*CAN BUS TXD            */
#define  BSP_GPIOD_CAN1TX						  GPIO_Pin_1    	/*CAN BUS RXD            */
#define  BSP_GPIOD_RXD5						    GPIO_Pin_2      /*UART5 RXD  debug com   */	
#define  BSP_GPIOD_NET_RESET					GPIO_Pin_3    	
#define  BSP_GPIOD_USB_PWR						GPIO_Pin_4    	/*USB_PWR                */	
#define  BSP_GPIOD_TXD2							  GPIO_Pin_5    	/*USART2 232 TXD         */
#define  BSP_GPIOD_RXD2							  GPIO_Pin_6    	/*USART2 232 RXD         */
#define  BSP_GPIOD_H595EN							GPIO_Pin_7    	/*74HC595EN  port         */
#define  BSP_GPIOD_TXD3							  GPIO_Pin_8    	/*UART3 485 TXD          */
#define  BSP_GPIOD_RXD3				        GPIO_Pin_9    	/*UART3 485 TXD          */
#define  BSP_GPIOD_HC597_RLCK			    GPIO_Pin_10    	/*74HC597 RLCK port      */	  	
#define  BSP_GPIOD_HC597_LOADIN				GPIO_Pin_11    	/*74HC597 LOADIN         */	  		
#define  BSP_GPIOD_TIM4_IN1						GPIO_Pin_12     /*TIM4 input capture 1   */	  	
#define  BSP_GPIOD_TIM4_IN2						GPIO_Pin_13    	/*TIM4 input capture 1   */	
#define  BSP_GPIOD_SPISD_CS			  		GPIO_Pin_14    	/*SPISD FALSH            */		
#define  BSP_GPIOD_EDIN2							GPIO_Pin_15    	

																											
#define  BSP_GPIOE_H595RCLK						GPIO_Pin_0    	/*74HC595RCLK  port      */
#define  BSP_GPIOE_S2							    GPIO_Pin_1    	
#define  BSP_GPIOE_ALARM1							GPIO_Pin_2			
#define  BSP_GPIOE_DIR							  GPIO_Pin_3			
#define	 BSP_GPIOE_EDIN6							GPIO_Pin_4			
#define  BSP_GPIOE_SPI2_CS2						GPIO_Pin_5			
#define  BSP_GPIOE_ALARM							GPIO_Pin_6     /*ALARM                   */ 	
#define  BSP_GPIOE_DIN1							  GPIO_Pin_7     /*MCU data input port1    */																				
#define  BSP_GPIOE_DIN2							  GPIO_Pin_8     /*MCU data input port2    */
#define  BSP_GPIOE_NULL							  GPIO_Pin_9       	
#define  BSP_GPIOE_DIN3							  GPIO_Pin_10    /*MCU data input port3    */
#define  BSP_GPIOE_DIN4					      GPIO_Pin_11    /*MCU data input port4    */	
#define  BSP_GPIOE_DIN5							  GPIO_Pin_12    /*MCU data input port5    */
#define  BSP_GPIOE_DIN6						    GPIO_Pin_13    /*MCU data input port6    */
#define  BSP_GPIOE_DIN7							  GPIO_Pin_14    /*MCU data input port7    */
#define  BSP_GPIOE_DIN8					      GPIO_Pin_15    /*MCU data input port8    */

#define HC595_DOUT_ALL    0xFFFFFF
#define HC595_DOUT_1      0x000001
#define HC595_DOUT_2      0x000002
#define HC595_DOUT_3      0x000004
#define HC595_DOUT_4      0x000008
#define HC595_DOUT_5      0x000010
#define HC595_DOUT_6      0x000020
#define HC595_DOUT_7      0x000040
#define HC595_DOUT_8      0x000080
#define HC595_DOUT_9      0x000100
#define HC595_DOUT_10     0x000200
#define HC595_DOUT_11     0x000400
#define HC595_DOUT_12     0x000800
#define HC595_DOUT_13     0x001000
#define HC595_DOUT_14     0x002000
#define HC595_DOUT_15     0x004000
#define HC595_DOUT_16     0x008000
#define HC595_DOUT_17     0x010000
#define HC595_DOUT_18     0x020000
#define HC595_DOUT_19     0x040000
#define HC595_DOUT_20     0x080000
#define HC595_DOUT_21     0x100000
#define HC595_DOUT_22     0x200000
#define HC595_DOUT_23     0x400000
#define HC595_DOUT_24     0x800000


#define HC597_DIN_9      0x0001
#define HC597_DIN_10     0x0002
#define HC597_DIN_11     0x0004
#define HC597_DIN_12     0x0008
#define HC597_DIN_13     0x0010
#define HC597_DIN_14     0x0020
#define HC597_DIN_15     0x0040
#define HC597_DIN_16     0x0080
#define HC597_DIN_17     0x0100
#define HC597_DIN_18     0x0200
#define HC597_DIN_19     0x0400
#define HC597_DIN_20     0x0800
#define HC597_DIN_21     0x1000
#define HC597_DIN_22     0x2000
#define HC597_DIN_23     0x4000
#define HC597_DIN_24     0x8000

/**************************************************************************************************************/

#define   DEF_ENABLE_KOER               BSP_Write_DOUT1_24(HC595_DOUT_9 | HC595_DOUT_10 ,0)
#define   DEF_DISENABLE_KOER            BSP_Write_DOUT1_24(HC595_DOUT_9 | HC595_DOUT_10 ,1)


#define  Is_0to9(c) 									((c >= '0') && (c <= '9'))
#define  Is_AtoF(c) 									((c >= 'A') && (c <= 'F'))
#define  Is_atof(c) 									((c >= 'a') && (c <= 'f'))
#define  Is_AtoZ(c) 									((c >= 'A') && (c <= 'Z'))
#define  Is_atoz(c) 									((c >= 'a') && (c <= 'z'))

#define   ALLOWERR( val , std ,err)     ( val<=std+err && val>=std-err )
#define   BELONGTO( val , min ,max)     ( val<max && val>=min )
#define   ABS(x)                        ( (x)>0?(x):-(x) ) 


/*
*********************************************************************************************************
*                                               DATA TYPES
*********************************************************************************************************
*/

typedef	struct tag_Date_Time
{
	u16		year;									/* year */	
	u8		month;								/* month */
	u8		day;									/* day */
	u8		hour;			 						/* hour */
	u8		minute;								/* minute */
	u8		second;								/* second */

}SYS_DATETIME;



/*
*********************************************************************************************************
*                                           MAIN SERVICES
*********************************************************************************************************
*/

void				BSP_NVIC_Configuration			(void);

/*
*********************************************************************************************************
*                                           CPU SERVICES
*********************************************************************************************************
*/
void				BSP_CPU_DisAllInterupt			(void);
void				BSP_CPU_Reset								(void);

/*
*********************************************************************************************************
*                                     PERIPHERAL POWER/CLOCK SERVICES
*********************************************************************************************************
*/
void				BSP_RCC_Configuration				(void);
void 				BSP_SYSCLKConfig_FromStop 	(void);

/*
*********************************************************************************************************
*                                           I/O SERVICES
*********************************************************************************************************
*/

void  			BSP_GPIO_Init 							(void);


/*
*********************************************************************************************************
*                                              WDT SERVICES
*********************************************************************************************************
*/

void				BSP_WDTDOG_Init							(void);

/*
*********************************************************************************************************
*                                              PVD SERVICES
*********************************************************************************************************
*/
void				BSP_PVDEXIT_IT_CON 					(u8 newSta);
void				BSP_PVD_Init 								(u32 PvdLevel);
u8					BSP_PVD_GetSta 							(void);
void				BSP_PVD_IRQHook 						(void);
/*
*********************************************************************************************************
*                                              ADC SERVICES
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                              RTC SERVICES
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                              LED SERVICES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                              PB SERVICES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                              USART SERVICES
*********************************************************************************************************
*/
void        BSP_USART_Config            (USART_TypeDef* USARTx,u32 baud);
void        BSP_RS485_SendData          (USART_TypeDef* USARTx , u8 *src ,u8 len );  
void        BSP_RS232_SendData          (USART_TypeDef* USARTx , u8 *src ,u8 len);
s8					myPrintf 										(const char * format, ...);

/*
*********************************************************************************************************
*                                              SPI SERVICES
*********************************************************************************************************
*/
void        BSP_SPI1_Init               (void);
void        BSP_74HC595_Out             (u8* src , u8 len);
void        BSP_Write_DOUT1_24          (u32 data , u8 state);

u8          BSP_Read_DIN1_8             (u16 pin);
u16         BSP_Read_DIN9_24            (void);

u32         BSP_SPI_FLASH_ReadID            (void);
void        BSP_SPI_FLASH_SectorErase       (u32 SectorAddr);
void        BSP_SPI_FLASH_BulkErase         (void);
void        BSP_SPI_FLASH_PageWrite         (u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite);
void        BSP_SPI_FLASH_BufferWrite       (u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite);
void        BSP_SPI_FLASH_BufferRead        (u8* pBuffer, u32 ReadAddr, u16 NumByteToRead);
/*
*********************************************************************************************************
*                                              SPI SERVICES
*********************************************************************************************************
*/
void				BSP_IIC_Init 								(u8	IIC_ID);

/*
*********************************************************************************************************
*                                              CAN SERVICES
*********************************************************************************************************
*/
//#define DEF_CAN500K	0x00070206
#define DEF_CAN500K	0x00050406
#define DEF_CAN250K	0x000D0208
#define DEF_CAN125K	0x000D0216
#define DEF_CAN833K	0x000a0510
#define DEF_CAN50K	0x00090420
void        BSP_CANX_Init               (CAN_TypeDef * canx , u32 u32CanBaud );
void        BSP_CAN_Filter              (u8 u8FilterGroup, u32 u32FilterId ,u32 idmask);
bool        BSP_CAN_SendMsg              (u32 sendid , u32 recvid , u8 *data);

/*
*********************************************************************************************************
*                                             TIM SERVICES
*********************************************************************************************************
*/
void				BSP_TIM3_Output_PWM_Init								  (void);
void				BSP_TIM4_Input_Capture_Init								(void);

void        BSP_TIM3_Init                             (void);
void        BSP_TIM4_Init                             (void);

void	      OSTmr_Count_Start                         (vu32	*pCounter);
u32	        OSTmr_Count_Get                           (vu32	*pCounter);
/*
*********************************************************************************************************
*                                             ETH SERVICES
*********************************************************************************************************
*/
void       BSP_Ethernet_Configuration(void);
/*
*********************************************************************************************************
*                                             MEMORY SERVICES
*********************************************************************************************************
*/

u8         BSP_Write_Para               (TYPEDEF_PARA* para);
void       BSP_Read_Para                (TYPEDEF_PARA* para);
/*
*********************************************************************************************************
*                                             OTHER SERVICES
*********************************************************************************************************
*/



/*
*********************************************************************************************************
*                                           	TOOLS
*********************************************************************************************************
*/
void				GetCpuHwID 									(u8 *idBuf);
void				SoftDelayMS									(u32 Nms);
u8				 	Asc2Hex											(u8	*buf);
u8 					Hex2BCD 										(u8 hex_data);
u16					IsA2ZString 								(u8 *pBuf,	u16	len);
u16					IsIntString 								(u8 *pBuf,	u16	len);
u16					IsHexString 								(u8 *pBuf,	u16	len);
u16					IsDotIntString 							(u8 *pBuf,	u16	len);
u16					StringAdd 									(u8	*pStr,	u8	*pStrF);
u16					String2HexArray 						(u8	*pSrc,	u16	SrcLen,	u8 *pObj);
u16					SNCode2barCode 							(u32 *srcID,	u32 *objID);
s8					CheckPhoneFormat 						(u8 *pSrc,	u16 srcLen,	u8	*pObj1,	u16 *obj1Len,	u8	*pObj2,	u16 *obj2Len, u8	*pObj3,	u16 *obj3Len);
s8					CheckIsNum 									(u8 *pBuf,	u16 len);
u16					ADCalculateProcess					(u16 *pBuf,	u16 len); 
u16					MakeStrNum2USC2 						(u8 *pSrc,	u16 srcLen,	u8	*pObj);
u16					StringConnect 							(u8	*pStr,	u8	*pStrF,		u8	*pStrB); 
u16 				StringSearch								(u8 *pSource,u8 *pObject,u16 SrcLen,u16 ObjLen);
u32 				Mktime 											(u32 year, u32 mon,u32 day, u32 hour,u32 min, u32 sec);
void 				Gettime 										(u32 count,	SYS_DATETIME *cnt);
u16					Hex2StringArray 						(u8	*pSrc,	u16	SrcLen,	u8 *pObj);
u16	 				SumCheck16Pro 							(u8 *srcBuf,	u16	srcLen);
u8          XorCheck16Pro               (u8 *srcBuf,	u16	srcLen);
s8					CheckIsVerb 								(u8 *pVerb);

/*
*********************************************************************************************************
*                                           	EXTI 
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                           	OTHER 
*********************************************************************************************************
*/
/*
*********************************************************************************************************
*                                               MODULE END
*********************************************************************************************************
*/
void BSP_EthernetInit(void);


#endif




