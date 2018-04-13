#include "bsp_eth.h"

/*
*********************************************************************************************************
*                                             BSP_Ethernet_Configuration()
*
* Description : BSP_Ethernet_Configuration
*
* Argument(s) : void
*
* Return(s)   : none.
*
* Note(s)     : 
*********************************************************************************************************
*/
void BSP_Ethernet_Configuration(void)
{

	 ETH_InitTypeDef ETH_InitStructure;

   /* MII/RMII Media interface selection ------------------------------------------*/
#ifdef MII_MODE /* Mode MII with STM3210C-EVAL  */
   GPIO_ETH_MediaInterfaceConfig(GPIO_ETH_MediaInterface_MII);

   /* Get HSE clock = 25MHz on PA8 pin (MCO) */
   RCC_MCOConfig(RCC_MCO_HSE);

#elif defined RMII_MODE  /* Mode RMII with STM3210C-EVAL */
   GPIO_ETH_MediaInterfaceConfig(GPIO_ETH_MediaInterface_RMII);

   /* Set PLL3 clock output to 50MHz (25MHz /5 *10 =50MHz) */
   RCC_PLL3Config(RCC_PLL3Mul_10);
   /* Enable PLL3 */
   RCC_PLL3Cmd(ENABLE);
   /* Wait till PLL3 is ready */
   while (RCC_GetFlagStatus(RCC_FLAG_PLL3RDY) == RESET)
   {}

  /* Get PLL3 clock on PA8 pin (MCO) */
   RCC_MCOConfig(RCC_MCO_PLL3CLK);
#endif

    /* Reset ETHERNET on AHB Bus */
    ETH_DeInit();

    /* Software reset */
    ETH_SoftwareReset();

    /* Wait for software reset */
    while (ETH_GetSoftwareResetStatus() == SET);

    /* ETHERNET Configuration ------------------------------------------------------*/
    /* Call ETH_StructInit if you don't like to configure all ETH_InitStructure parameter */
    ETH_StructInit(&ETH_InitStructure);

    /* Fill ETH_InitStructure parametrs */
    /*------------------------ ETH_MACCR-----------------------------------*/
    //参数是否自动配置，选择disable需要自动配置默写参数（auto模式在网线没插上时会导致超时，初始化失败）
 	  ETH_InitStructure.ETH_AutoNegotiation = ETH_AutoNegotiation_Disable;
    ETH_InitStructure.ETH_Watchdog = ETH_Watchdog_Disable;                            //关闭看门狗定时器，允许接收超长帧
    ETH_InitStructure.ETH_Jabber = ETH_Jabber_Disable;                                //关闭jabber定时器，允许发送超长帧
    ETH_InitStructure.ETH_InterFrameGap = ETH_InterFrameGap_40Bit;          			//发送帧间间隙
    ETH_InitStructure.ETH_Speed = ETH_Speed_100M;                                     //快速以太网速度
    ETH_InitStructure.ETH_LoopbackMode = ETH_LoopbackMode_Disable;                    //不启用自循环模式
    ETH_InitStructure.ETH_Mode = ETH_Mode_FullDuplex;                                 //全双工模式
  
    /*自动填充/CRC剥离处理不执行，转发所有帧*/
    ETH_InitStructure.ETH_AutomaticPadCRCStrip = ETH_AutomaticPadCRCStrip_Disable;   
#if CHECKSUM_BY_HARDWARE
 	ETH_InitStructure.ETH_ChecksumOffload = ETH_ChecksumOffload_Enable; //IPV4头文件硬件校验
#endif

    /*------------------------ ETH_MACFFR----------------------------------*/
 	ETH_InitStructure.ETH_ReceiveAll = ETH_ReceiveAll_Disable;                                   //MAC过滤只接受通过源目的地址的数据
    ETH_InitStructure.ETH_SourceAddrFilter = ETH_SourceAddrFilter_Normal_Enable;                 //MAC过滤源地址错误帧？
    ETH_InitStructure.ETH_PassControlFrames = ETH_PassControlFrames_BlockAll;                    //MAC不转发任何控制帧
    ETH_InitStructure.ETH_BroadcastFramesReception = ETH_BroadcastFramesReception_Enable;        //接收广播帧
    ETH_InitStructure.ETH_DestinationAddrFilter = ETH_DestinationAddrFilter_Normal;              //目的地址过滤结果正常
    ETH_InitStructure.ETH_PromiscuousMode = ETH_PromiscuousMode_Disable;                         //混杂模式，启用帧过滤
    ETH_InitStructure.ETH_MulticastFramesFilter = ETH_MulticastFramesFilter_Perfect;             //过滤器正常工作，不传送控制帧
    ETH_InitStructure.ETH_UnicastFramesFilter = ETH_UnicastFramesFilter_Perfect;                 //单播帧目的地址完美过滤

   /*------------------------   DMA  ETH_DMAOMR -----------------------------------*/  
   /* When we use the Checksum offload feature, we need to enable the Store and Forward mode: 
   the store and forward guarantee that a whole frame is stored in the FIFO, so the MAC can insert/verify the checksum, 
   if the checksum is OK the DMA can handle the frame otherwise the frame is dropped */
    /*丢弃校验错误帧不执行(因为未进行硬件校验)*/
    ETH_InitStructure.ETH_DropTCPIPChecksumErrorFrame = ETH_DropTCPIPChecksumErrorFrame_Disable;
    ETH_InitStructure.ETH_ReceiveStoreForward = ETH_ReceiveStoreForward_Enable;              //接收数据超过阈值转发
    ETH_InitStructure.ETH_FlushReceivedFrame = ETH_FlushReceivedFrame_Enable;                 //描述符被占用和接收FIFO不可用时清空FIFO(解决堵塞)
    ETH_InitStructure.ETH_TransmitStoreForward = ETH_TransmitStoreForward_Enable;            //发送数据完整帧转发
    ETH_InitStructure.ETH_TransmitThresholdControl = ETH_TransmitThresholdControl_64Bytes;    //发送阈值为64Bytes
    ETH_InitStructure.ETH_ForwardErrorFrames = ETH_ForwardErrorFrames_Disable;                //接收FIFO丢弃所有错误帧
    
    /*接收FIFO上传长度不够的好帧*/
	  ETH_InitStructure.ETH_ForwardUndersizedGoodFrames = ETH_ForwardUndersizedGoodFrames_Enable;  
	  ETH_InitStructure.ETH_ReceiveThresholdControl = ETH_ReceiveThresholdControl_64Bytes;       //接收阈值为64Bytes
    ETH_InitStructure.ETH_SecondFrameOperate = ETH_SecondFrameOperate_Enable;                  //DMA直接发送第二个帧，不需要之前帧回复
    
    /*------------------------   DMA  ETH_DMABMR -----------------------------------*/      
    ETH_InitStructure.ETH_AddressAlignedBeats = ETH_AddressAlignedBeats_Enable;                //传输地址对齐
    ETH_InitStructure.ETH_FixedBurst = ETH_FixedBurst_Enable;                                  //固定的突发
    ETH_InitStructure.ETH_RxDMABurstLength = ETH_RxDMABurstLength_32Beat;                              
    ETH_InitStructure.ETH_TxDMABurstLength = ETH_TxDMABurstLength_32Beat;
    ETH_InitStructure.ETH_DMAArbitration = ETH_DMAArbitration_RoundRobin_RxTx_2_1;             //发送和接收比例(上传数据时重要) 2:1

//  /* Fill ETH_InitStructure parametrs */
//  /*------------------------   MAC   -----------------------------------*/
//  ETH_InitStructure.ETH_AutoNegotiation = ETH_AutoNegotiation_Disable  ;
//  	ETH_InitStructure.ETH_Speed = ETH_Speed_100M;
//	ETH_InitStructure.ETH_Mode = ETH_Mode_FullDuplex;
//  ETH_InitStructure.ETH_LoopbackMode = ETH_LoopbackMode_Disable;
//  ETH_InitStructure.ETH_RetryTransmission = ETH_RetryTransmission_Disable;
//  ETH_InitStructure.ETH_AutomaticPadCRCStrip = ETH_AutomaticPadCRCStrip_Disable;
//  //ETH_InitStructure.ETH_ReceiveAll = ETH_ReceiveAll_Disable;
//	ETH_InitStructure.ETH_ReceiveAll = ETH_ReceiveAll_Enable;
//  //ETH_InitStructure.ETH_BroadcastFramesReception = ETH_BroadcastFramesReception_Enable;
//  	ETH_InitStructure.ETH_BroadcastFramesReception = ETH_BroadcastFramesReception_Disable;
//  ETH_InitStructure.ETH_PromiscuousMode = ETH_PromiscuousMode_Disable;
//  ETH_InitStructure.ETH_MulticastFramesFilter = ETH_MulticastFramesFilter_Perfect;
//  ETH_InitStructure.ETH_UnicastFramesFilter = ETH_UnicastFramesFilter_Perfect;
//#ifdef CHECKSUM_BY_HARDWARE
//  ETH_InitStructure.ETH_ChecksumOffload = ETH_ChecksumOffload_Enable;
//#endif
//
//  /*------------------------   DMA   -----------------------------------*/  
//  
//  /* When we use the Checksum offload feature, we need to enable the Store and Forward mode: 
//  the store and forward guarantee that a whole frame is stored in the FIFO, so the MAC can insert/verify the checksum, 
//  if the checksum is OK the DMA can handle the frame otherwise the frame is dropped */
//  ETH_InitStructure.ETH_DropTCPIPChecksumErrorFrame = ETH_DropTCPIPChecksumErrorFrame_Enable; 
//  ETH_InitStructure.ETH_ReceiveStoreForward = ETH_ReceiveStoreForward_Enable;         
//  ETH_InitStructure.ETH_TransmitStoreForward = ETH_TransmitStoreForward_Enable;     
// 
//  ETH_InitStructure.ETH_ForwardErrorFrames = ETH_ForwardErrorFrames_Disable;       
//  ETH_InitStructure.ETH_ForwardUndersizedGoodFrames = ETH_ForwardUndersizedGoodFrames_Disable;   
//  ETH_InitStructure.ETH_SecondFrameOperate = ETH_SecondFrameOperate_Enable;                                                          
//  ETH_InitStructure.ETH_AddressAlignedBeats = ETH_AddressAlignedBeats_Enable;      
//  ETH_InitStructure.ETH_FixedBurst = ETH_FixedBurst_Enable;                
//  ETH_InitStructure.ETH_RxDMABurstLength = ETH_RxDMABurstLength_32Beat;          
//  ETH_InitStructure.ETH_TxDMABurstLength = ETH_TxDMABurstLength_32Beat;                                                                 
//  ETH_InitStructure.ETH_DMAArbitration = ETH_DMAArbitration_RoundRobin_RxTx_2_1;

    /* Configure Ethernet */
    ETH_Init(&ETH_InitStructure, PHY_ADDRESS);

    /* Enable the Ethernet Rx Interrupt */
    //ETH_DMAITConfig(ETH_DMA_IT_NIS | ETH_DMA_IT_R, ENABLE);
    ETH_DMAITConfig(ETH_DMA_IT_NIS | ETH_DMA_IT_R, ENABLE);/* Enable DMA Receive interrupt (need to enable in this case Normal interrupt) */

}
void BSP_EthernetInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
		
		/* Enable ETHERNET clock  */
	  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_ETH_MAC | RCC_AHBPeriph_ETH_MAC_Tx |
                        RCC_AHBPeriph_ETH_MAC_Rx, ENABLE);

  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC |
                         RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE | RCC_APB2Periph_AFIO , ENABLE);



  	/* Configure PA2 as alternate function push-pull */
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  	GPIO_Init(GPIOA, &GPIO_InitStructure);

  	/* Configure PC1, PC2 and PC3 as alternate function push-pull */
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  	GPIO_Init(GPIOC, &GPIO_InitStructure);

  	/* Configure PB5, PB8, PB11, PB12 and PB13 as alternate function push-pull */
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_8 | GPIO_Pin_11 |
                                GPIO_Pin_12 | GPIO_Pin_13;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  	GPIO_Init(GPIOB, &GPIO_InitStructure);


		/* Configure PA0, PA1 and PA3 as input */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1 | GPIO_Pin_3|GPIO_Pin_7;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_Init(GPIOA, &GPIO_InitStructure);

		/* Configure PB10 as input */
		GPIO_InitStructure.GPIO_Pin =GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_10;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_Init(GPIOB, &GPIO_InitStructure);

		/* Configure PC3 as input */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_Init(GPIOC, &GPIO_InitStructure);

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		//Ethernet_Configuration();
}
