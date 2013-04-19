/******************************************************************************
----------------文件信息-----------------
名称：SYS.c
所属模块：配液板
功能:时钟\中断\定时器\串口配置
创建：刘旭东
创建日期：2009.2.13
当前版本: Vser1.0
修订记录:

******************************************************************************/
//#include"stm32f10x_lib.h"
#include "stm32f10x.h"
/******************************************************************************
函数原形:void SetupClock (void)
功能:	 系统各时钟配置
入口参数:无
出口参数:无
******************************************************************************/
void SetupClock (void)
{
  ErrorStatus HSEStartUpStatus;
  RCC_DeInit();              //时钟系统复位
  RCC_HSEConfig(RCC_HSE_ON); //开外部高速时钟
  HSEStartUpStatus = RCC_WaitForHSEStartUp();
  if(HSEStartUpStatus == SUCCESS)
  {
    RCC_HCLKConfig(RCC_SYSCLK_Div1); 
    
    RCC_PCLK2Config(RCC_HCLK_Div1); 
    
    RCC_PCLK1Config(RCC_HCLK_Div2);
    
    FLASH_SetLatency(FLASH_Latency_2);
    
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
    
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);	//8X9=72MHz
    
    RCC_PLLCmd(ENABLE);
    
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    {
    }
    
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
    
    while(RCC_GetSYSCLKSource() != 0x08)
    {
    }
  }
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2|RCC_APB1Periph_TIM6, ENABLE);    //TIM6-判断485数据的帧
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2|RCC_APB1Periph_TIM3|RCC_APB1Periph_TIM4|RCC_APB1Periph_TIM5, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1|RCC_APB2Periph_TIM8, ENABLE); 

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC,ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOE,ENABLE);
  //Enable AFIO clocks    -- 引脚重映射需要的RCC,如果没有使用到引脚重映射，可以注释掉
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
   // ****************** TIM2,3,4引脚重映射************************
  GPIO_PinRemapConfig(GPIO_FullRemap_TIM2|GPIO_FullRemap_TIM3|GPIO_Remap_TIM4,ENABLE); //重映射TIM3引脚

   /* Setup SysTick Timer for 100ms interrupts  */
  if (SysTick_Config(SystemCoreClock / 100))  // 100->10ms, 1000->1ms, 400->2.5ms, /10000->0.1ms
  { 
    /* Capture error */ 
    while (1);
  }
}

/*******************************************************************************
函数原形: void SetNVIC(void)
功能:	  非向量中断配置
入口参数: 无
出口参数: 无
*******************************************************************************/
void SetNVIC(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

    //配置UART2中断 - 上位机通信RS485，
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;//USART2_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;//0
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
  NVIC_Init(&NVIC_InitStructure);       
  //配置TIM2中断  - 上位机通信RS485，帧判断
  NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  //配置TIM3中断 - UFP_PWM
  NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
 
  //配置TIM4中断  BPP_PWM
  NVIC_InitStructure.NVIC_IRQChannel = TIM8_TRG_COM_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  //配置TIM4中断  SBP_PWM
  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
    //配置TIM4中断  SFP_PWM
  NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
    //配置TIM4中断  HP_PWM
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

}

/**************************************************************************
函数原形:void GPIO_Config(void)
功能:	 端口引脚模式配置
入口参数: 无
出口参数: 无
**************************************************************************/
void GPIO_Config(void)
{
  GPIO_InitTypeDef GPIO_Fig;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC,ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOE,ENABLE);
  
  //指示灯: PE1,2,3,4,5,6;PD15
  GPIO_Fig.GPIO_Pin=GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6;
  GPIO_Fig.GPIO_Mode=GPIO_Mode_Out_PP;
  GPIO_Fig.GPIO_Speed=GPIO_Speed_50MHz;
  GPIO_Init(GPIOE,&GPIO_Fig);
  
  GPIO_Fig.GPIO_Pin=GPIO_Pin_15;
  GPIO_Fig.GPIO_Mode=GPIO_Mode_Out_PP;
  GPIO_Fig.GPIO_Speed=GPIO_Speed_50MHz;
  GPIO_Init(GPIOD,&GPIO_Fig);
 
  //肝素泵检测：HS2(PC12),HS3(PD0),HS4(PC11): 为针管大小检测的3个光电开关。
  //            HS6(PD1):为安装状态检测
  GPIO_Fig.GPIO_Pin=GPIO_Pin_11|GPIO_Pin_12;
  GPIO_Fig.GPIO_Mode=GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOC,&GPIO_Fig);
  
  GPIO_Fig.GPIO_Pin=GPIO_Pin_0|GPIO_Pin_1;
  GPIO_Fig.GPIO_Mode=GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOD,&GPIO_Fig);
  
 
}


/***********************************************************************
函数原型：void TIM1_Configuration(void)
函数功能：高级定时器TIM1配置
入口参数：无
出口参数：无
***********************************************************************/
void TIM1_Configuration(void)
{
 
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
 
  TIM_TimeBaseStructure.TIM_Prescaler=71;	//72M/72=1M
  TIM_TimeBaseStructure.TIM_Period = 5000; //5000/1MX1000=5mS       
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;    
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseStructure.TIM_RepetitionCounter=0; 
  TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
  //清中断，以免一启用中断后立即产生中断
  TIM_ClearFlag(TIM1,TIM_FLAG_Update);
  //使能TIM1中断源
  // TIM1_ITConfig(TIM1_IT_Update,ENABLE);	 //中断使能
  // TIM1_Cmd(ENABLE);
  
} 



/***************************************************************************
函数原型：void SPI_Config(void)
功能：    SPI配置(从机）
入口参数：无
出口参数：无
***************************************************************************/
void SPI_Config(void)
{ 
  SPI_InitTypeDef   SPI_InitStructure;
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;//全双工
  SPI_InitStructure.SPI_Mode = SPI_Mode_Slave;          //从模式
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;	   //8bit/帧
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High ;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Hard;			  //片选置为硬件方式
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(SPI2, &SPI_InitStructure);
  SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_RXNE , ENABLE);         //使能SPI中断
  SPI_Cmd(SPI2, ENABLE);
}



/************************************************************************
函数原形:void System_Init(void)
功能:系统初始化
入口参数:无
出口参数:无
************************************************************************/
void System_Init(void)
{
  SetupClock ();
  GPIO_Config();
  SetNVIC();
  SPI_Config();
 // TIM1_Configuration();

}

