/******************************************************************************
----------------�ļ���Ϣ-----------------
���ƣ�SYS.c
����ģ�飺��Һ��
����:ʱ��\�ж�\��ʱ��\��������
����������
�������ڣ�2009.2.13
��ǰ�汾: Vser1.0
�޶���¼:

******************************************************************************/
//#include"stm32f10x_lib.h"
#include "stm32f10x.h"
/******************************************************************************
����ԭ��:void SetupClock (void)
����:	 ϵͳ��ʱ������
��ڲ���:��
���ڲ���:��
******************************************************************************/
void SetupClock (void)
{
  ErrorStatus HSEStartUpStatus;
  RCC_DeInit();              //ʱ��ϵͳ��λ
  RCC_HSEConfig(RCC_HSE_ON); //���ⲿ����ʱ��
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
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2|RCC_APB1Periph_TIM6, ENABLE);    //TIM6-�ж�485���ݵ�֡
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2|RCC_APB1Periph_TIM3|RCC_APB1Periph_TIM4|RCC_APB1Periph_TIM5, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1|RCC_APB2Periph_TIM8, ENABLE); 

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC,ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOE,ENABLE);
  //Enable AFIO clocks    -- ������ӳ����Ҫ��RCC,���û��ʹ�õ�������ӳ�䣬����ע�͵�
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
   // ****************** TIM2,3,4������ӳ��************************
  GPIO_PinRemapConfig(GPIO_FullRemap_TIM2|GPIO_FullRemap_TIM3|GPIO_Remap_TIM4,ENABLE); //��ӳ��TIM3����

   /* Setup SysTick Timer for 100ms interrupts  */
  if (SysTick_Config(SystemCoreClock / 100))  // 100->10ms, 1000->1ms, 400->2.5ms, /10000->0.1ms
  { 
    /* Capture error */ 
    while (1);
  }
}

/*******************************************************************************
����ԭ��: void SetNVIC(void)
����:	  �������ж�����
��ڲ���: ��
���ڲ���: ��
*******************************************************************************/
void SetNVIC(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

    //����UART2�ж� - ��λ��ͨ��RS485��
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;//USART2_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;//0
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
  NVIC_Init(&NVIC_InitStructure);       
  //����TIM2�ж�  - ��λ��ͨ��RS485��֡�ж�
  NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  //����TIM3�ж� - UFP_PWM
  NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
 
  //����TIM4�ж�  BPP_PWM
  NVIC_InitStructure.NVIC_IRQChannel = TIM8_TRG_COM_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  //����TIM4�ж�  SBP_PWM
  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
    //����TIM4�ж�  SFP_PWM
  NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
    //����TIM4�ж�  HP_PWM
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

}

/**************************************************************************
����ԭ��:void GPIO_Config(void)
����:	 �˿�����ģʽ����
��ڲ���: ��
���ڲ���: ��
**************************************************************************/
void GPIO_Config(void)
{
  GPIO_InitTypeDef GPIO_Fig;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC,ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOE,ENABLE);
  
  //ָʾ��: PE1,2,3,4,5,6;PD15
  GPIO_Fig.GPIO_Pin=GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6;
  GPIO_Fig.GPIO_Mode=GPIO_Mode_Out_PP;
  GPIO_Fig.GPIO_Speed=GPIO_Speed_50MHz;
  GPIO_Init(GPIOE,&GPIO_Fig);
  
  GPIO_Fig.GPIO_Pin=GPIO_Pin_15;
  GPIO_Fig.GPIO_Mode=GPIO_Mode_Out_PP;
  GPIO_Fig.GPIO_Speed=GPIO_Speed_50MHz;
  GPIO_Init(GPIOD,&GPIO_Fig);
 
  //���رü�⣺HS2(PC12),HS3(PD0),HS4(PC11): Ϊ��ܴ�С����3����翪�ء�
  //            HS6(PD1):Ϊ��װ״̬���
  GPIO_Fig.GPIO_Pin=GPIO_Pin_11|GPIO_Pin_12;
  GPIO_Fig.GPIO_Mode=GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOC,&GPIO_Fig);
  
  GPIO_Fig.GPIO_Pin=GPIO_Pin_0|GPIO_Pin_1;
  GPIO_Fig.GPIO_Mode=GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOD,&GPIO_Fig);
  
 
}


/***********************************************************************
����ԭ�ͣ�void TIM1_Configuration(void)
�������ܣ��߼���ʱ��TIM1����
��ڲ�������
���ڲ�������
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
  //���жϣ�����һ�����жϺ����������ж�
  TIM_ClearFlag(TIM1,TIM_FLAG_Update);
  //ʹ��TIM1�ж�Դ
  // TIM1_ITConfig(TIM1_IT_Update,ENABLE);	 //�ж�ʹ��
  // TIM1_Cmd(ENABLE);
  
} 



/***************************************************************************
����ԭ�ͣ�void SPI_Config(void)
���ܣ�    SPI����(�ӻ���
��ڲ�������
���ڲ�������
***************************************************************************/
void SPI_Config(void)
{ 
  SPI_InitTypeDef   SPI_InitStructure;
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;//ȫ˫��
  SPI_InitStructure.SPI_Mode = SPI_Mode_Slave;          //��ģʽ
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;	   //8bit/֡
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High ;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Hard;			  //Ƭѡ��ΪӲ����ʽ
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(SPI2, &SPI_InitStructure);
  SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_RXNE , ENABLE);         //ʹ��SPI�ж�
  SPI_Cmd(SPI2, ENABLE);
}



/************************************************************************
����ԭ��:void System_Init(void)
����:ϵͳ��ʼ��
��ڲ���:��
���ڲ���:��
************************************************************************/
void System_Init(void)
{
  SetupClock ();
  GPIO_Config();
  SetNVIC();
  SPI_Config();
 // TIM1_Configuration();

}

