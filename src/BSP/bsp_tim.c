#include "bsp_tim.h"

/*
*********************************************************************************************************
*                                             BSP_TIM3_Init()
*
* Description : TIM3 init  output PWM
*
* Argument(s) : none
*
* Return(s)   : none.
*
* Note(s)     : 
*********************************************************************************************************
*/
void BSP_TIM3_Output_PWM_Init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;//根据TIM_OCInitStruct中指定的参数初始化外设TIMx

	TIM_TimeBaseInitStructure.TIM_Period = 900;	   //不分频,PWM 频率=72000/900=8Khz//设置自动重装载寄存器周期的值
	TIM_TimeBaseInitStructure.TIM_Prescaler = 0;//设置用来作为TIMx时钟频率预分频值，100Khz计数频率
	TIM_TimeBaseInitStructure.TIM_ClockDivision = 0;//设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;	//TIM向上计数模式
	TIM_TimeBaseInit(TIM3, & TIM_TimeBaseInitStructure);

	TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable;//PWM输出使能
	TIM_OCInitStructure.TIM_OCPolarity=TIM_OCPolarity_Low;

	TIM_OC1Init(TIM3,&TIM_OCInitStructure);
	TIM_OC2Init(TIM3,&TIM_OCInitStructure);
	
  TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);//使能或者失能TIMx在CCR1上的预装载寄存器 
	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);//使能或者失能TIMx在CCR2上的预装载寄存器
	
	TIM_SetCompare1(TIM3, 100);
	TIM_SetCompare2(TIM3, 300);
	
	TIM_Cmd(TIM3,ENABLE);//使能或者失能TIMx外设	
}
/*
*********************************************************************************************************
*                                             BSP_TIM4_Init()
*
* Description : TIM5 init  catch input
*
* Argument(s) : USARTx , baud
*
* Return(s)   : none.
*
* Note(s)     : 
*********************************************************************************************************
*/
void BSP_TIM4_Input_Capture_Init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;//声明一个结构体变量，用来初始化定时器
	TIM_ICInitTypeDef TIM_ICInitStructure;

	TIM_ClearITPendingBit(TIM4,TIM_IT_Update | TIM_IT_CC1 | TIM_IT_CC2); //清除中断和捕获标志位

	TIM_TimeBaseInitStructure.TIM_Period = 0xffff;	 //设定计数器自动重装值 	
	TIM_TimeBaseInitStructure.TIM_Prescaler = 71;   //以1Mhz的频率计数 一次即是1us
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;	  //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;	 //TIM向上计数模式
	TIM_TimeBaseInit(TIM5,&TIM_TimeBaseInitStructure);//根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
	
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_1 | TIM_Channel_2; //选择输入端 IC1映射到TI1上 
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising; //上升沿捕获 
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //映射到TI1上 
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1; //配置输入分频,不分频 
	TIM_ICInitStructure.TIM_ICFilter = 0x00; //IC1F=0000 配置输入滤波器 不滤波 
	TIM_ICInit(TIM4, &TIM_ICInitStructure); //初始化TIM5输入捕获通道1	
	
	TIM_Cmd(TIM4,ENABLE); //使能或者失能TIMx外设
	TIM_ITConfig(TIM4, TIM_IT_Update | TIM_IT_CC1 | TIM_IT_CC2, ENABLE );	//使能或者失能指定的TIM中断
		
}

/*******************************************************************************
函数功能：初始化定时器3
*******************************************************************************/
void BSP_TIM3_Init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_ICInitTypeDef TIM_ICInitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;//输入
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOC,&GPIO_InitStructure);
	GPIO_ResetBits(GPIOC,GPIO_Pin_6);//下拉
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;//输入
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOC,&GPIO_InitStructure);
	GPIO_ResetBits(GPIOC,GPIO_Pin_7);//下拉	
	GPIO_PinRemapConfig(GPIO_FullRemap_TIM3,ENABLE);
  /* Timer configuration in Encoder mode */
  TIM_DeInit(TIM3);
  TIM_TimeBaseStructInit(&TIM_TimeBaseStructure); 
  TIM_TimeBaseStructure.TIM_Prescaler = 0x0;  // No prescaling 
  TIM_TimeBaseStructure.TIM_Period =0xffff-1;     //(4*ENCODER_PPR)-1
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;   
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); 
  TIM_EncoderInterfaceConfig(TIM3, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
  TIM_ICStructInit(&TIM_ICInitStructure);
  TIM_ICInitStructure.TIM_ICFilter = 6;
  TIM_ICInit(TIM3, &TIM_ICInitStructure);   
  TIM3->CNT = 0; 
  TIM_Cmd(TIM3, ENABLE); 
}
/*******************************************************************************
函数功能：初始化定时器4
*******************************************************************************/
void BSP_TIM4_Init(void)			  
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_ICInitTypeDef TIM_ICInitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;//输入
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOD,&GPIO_InitStructure);
	GPIO_ResetBits(GPIOD,GPIO_Pin_12);//下拉

	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;//输入
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOD,&GPIO_InitStructure);
	GPIO_ResetBits(GPIOD,GPIO_Pin_13);//下拉
	GPIO_PinRemapConfig(GPIO_Remap_TIM4,ENABLE);
  /* Timer configuration in Encoder mode */
  TIM_DeInit(TIM4);
  TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
  
  TIM_TimeBaseStructure.TIM_Prescaler = 0x0;  // No prescaling 
  TIM_TimeBaseStructure.TIM_Period =0xffff-1;     //(4*ENCODER_PPR)-1
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;   
  TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
 
  TIM_EncoderInterfaceConfig(TIM4, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
  TIM_ICStructInit(&TIM_ICInitStructure);
  TIM_ICInitStructure.TIM_ICFilter = 6;
  TIM_ICInit(TIM4, &TIM_ICInitStructure);
  
  TIM4->CNT = 0; 
  TIM_Cmd(TIM4, ENABLE); 
                         
}
