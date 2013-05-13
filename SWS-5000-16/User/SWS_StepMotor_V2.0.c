/*************************** (C) COPYRIGHT 2008 SWS ****************************

* File Name          : stepmotor_config.c
* Author             : ���ѱ�
* Version            : V1.0
* Date               : 2008.9.19
* Description        : ���������Ӳ������
δ����
   1.�������
   2.����/����λ�ü��
   3.ת�ټ��(HALL������̵�)
   4.ALERT��NFȱ��ȱ�����Ϣ���

*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "stm32f10x_tim.h"
#include "sws_stepmotor_v2.h"


StepMotor_WorkInfo_Def StepMotor_Data[MOTOR_TOTAL]; //������в��������ݸ������Ϊ4��
u32 StepMotor_ADJ_Cnt = 0;
u16 NF_Cnt[MOTOR_TOTAL] = {0};
u16 Hall_Speed[MOTOR_TOTAL] = {0};

/*******************************************************************************
* Function Name  : StepMotor_Init
* Description    : �����������������ʼ������GPIO����
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void StepMotor_Init(void)
{
  u8 i;
  
  for(i = 0; i < MOTOR_TOTAL; i++)  //��չ���״̬
  {
    StepMotor_Data[i].Enable = DISABLE;
    StepMotor_Data[i].Status = DISABLE;  //����Ӧ��ʼ��ΪDISABLE�������ʼ��ΪENABLE��Ŀ�����ô�������ͷŵ��
    StepMotor_Data[i].SetDir = 0;
    StepMotor_Data[i].OldDir = 0;
    StepMotor_Data[i].SetSpeed = 0;
    StepMotor_Data[i].CurrSpeed = 0;
    StepMotor_Data[i].ToBottom = 0;
    StepMotor_Data[i].ToTop = 0;
    StepMotor_Data[i].Alert = 0;
    StepMotor_Data[i].NF = 0;
    StepMotor_Data[i].Braking = 0;
    StepMotor_Data[i].RunSta_Brak = 0;
    StepMotor_Data[i].LockRotor = 0;
    StepMotor_Data[i].Flow = 0;
    StepMotor_Data[i].Pluse = 0;
    StepMotor_Data[i].LockRotorSpeed = 0;
    StepMotor_Data[i].FlowLockRotor = 0;
    StepMotor_Data[i].RatioSwitch = 0; 
    StepMotor_Data[i].Down_Mode = 0; 
    StepMotor_Data[i].Ratio = 0; 
    StepMotor_Data[i].SetFlow = 0;
  }
  
  for(i = 0; i < MOTOR_TOTAL; i++) //����GPIO
  {  
   StepMotor_PWM_Pin_Configuration((StepMotor_TypeDef)i);     
   StepMotor_DIR_Pin_Configuration((StepMotor_TypeDef)i);
   StepMotor_ENABLE_Pin_Configuration((StepMotor_TypeDef)i);
   StepMotor_RESET_Pin_Configuration((StepMotor_TypeDef)i);
   StepMotor_DCY1_Pin_Configuration((StepMotor_TypeDef)i);
   StepMotor_DCY2_Pin_Configuration((StepMotor_TypeDef)i);
   StepMotor_ALERT_Pin_Configuration((StepMotor_TypeDef)i);
   StepMotor_NF_Pin_Configuration((StepMotor_TypeDef)i);
   StepMotor_TOP_Pin_Configuration((StepMotor_TypeDef)i);
   StepMotor_BOTTOM_Pin_Configuration((StepMotor_TypeDef)i);
   StepMotor_BRAKING_Pin_Configuration((StepMotor_TypeDef)i);
   StepMotor_REAL_Pin_EXTI_Configuration((StepMotor_TypeDef)i);
  } 
}

/*******************************************************************************
* Function Name  : StepMotor_RCC_Configuration
* Description    : ���ò������������ʹ�õ�ϵͳʱ��(����TIM��GPIO)
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
//void StepMotor_RCC_Configuration(void)
//{ 
//  //ϵͳʱ����Ƶ����
//  // SystemInit();
// 
//   // TIM1 TIM8 clock enable 
//  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1|RCC_APB2Periph_TIM8, ENABLE);
//  
//  // TIM2 TIM3 TIM4 TIM5  clock enable 
//  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2|RCC_APB1Periph_TIM3
//          |RCC_APB1Periph_TIM4|RCC_APB1Periph_TIM5, ENABLE);
//  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
//
//  // GPIOA GPIOB GPIOC GPIOD GPIOE clock enable 
//  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB
//       |RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOE, ENABLE);
//  
//  //Enable AFIO clocks    -- ������ӳ����Ҫ��RCC,���û��ʹ�õ�������ӳ�䣬����ע�͵�
// // RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
//  
//   // ****************** TIM3������ӳ��************************
// // GPIO_PinRemapConfig(GPIO_FullRemap_TIM3,ENABLE); //��ӳ��TIM3����
//
//}

/*******************************************************************************
* Function Name  : StepMotor_PWM_Pin_Configuration
* Description    : ����MCU������������������PWM�ź�����
* �������       : Motorx - ������: ö���е�ֵ(0~5: ��Ӧ STEPMOTOR1~STEPMOTOR6)
* Output         : None
* Return         : None
*******************************************************************************/
void StepMotor_PWM_Pin_Configuration(StepMotor_TypeDef Motorx)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_TypeDef *StepMotor_PORT[6] = {MOTOR1_PWM_PORT,MOTOR2_PWM_PORT,MOTOR3_PWM_PORT,MOTOR4_PWM_PORT,MOTOR5_PWM_PORT,MOTOR6_PWM_PORT};   //�˿�
  uint16_t   Motor_PWM_PINx[6] =    {MOTOR1_PWM_PIN,MOTOR2_PWM_PIN,MOTOR3_PWM_PIN,MOTOR4_PWM_PIN,MOTOR5_PWM_PIN,MOTOR6_PWM_PIN};    //Pin
  
  //  ����CLK(PWM)�ţ������������ 
  if(StepMotor_PORT[Motorx])
  {  
    GPIO_InitStructure.GPIO_Pin = Motor_PWM_PINx[Motorx];
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(StepMotor_PORT[Motorx], &GPIO_InitStructure);  
  }
}
/*******************************************************************************
* Function Name  : StepMotor_DIR_Pin_Configuration
* Description    : ����MCU������������������CW/CCW��������ź�����
* �������       : Motorx - ������: ö���е�ֵ(0~5: ��Ӧ STEPMOTOR1~STEPMOTOR6)
* Output         : None
* Return         : None
*******************************************************************************/
void StepMotor_DIR_Pin_Configuration(StepMotor_TypeDef Motorx)
{
#ifdef MOTOR_DIR_EN 
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_TypeDef *StepMotor_PORT[6] = {MOTOR1_DIR_PORT,MOTOR2_DIR_PORT,MOTOR3_DIR_PORT,MOTOR4_DIR_PORT,MOTOR5_DIR_PORT,MOTOR6_DIR_PORT};   //�˿�
  uint16_t   Motor_DIR_PINx[6] =    {MOTOR1_DIR_PIN,MOTOR2_DIR_PIN,MOTOR3_DIR_PIN,MOTOR4_DIR_PIN,MOTOR5_DIR_PIN,MOTOR6_DIR_PIN};    //Pin
  
  //  ����CW/CCW���ţ��������  
  if(StepMotor_PORT[Motorx])
  {  
    GPIO_InitStructure.GPIO_Pin = Motor_DIR_PINx[Motorx];
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(StepMotor_PORT[Motorx], &GPIO_InitStructure);  
  }
#endif  
}
/*******************************************************************************
* Function Name  : StepMotor_ENABLE_Pin_Configuration
* Description    : ����MCU������������������ENABLE�ź�����
* �������       : Motorx - ������: ö���е�ֵ(0~5: ��Ӧ STEPMOTOR1~STEPMOTOR6)
* Output         : None
* Return         : None
*******************************************************************************/
void StepMotor_ENABLE_Pin_Configuration(StepMotor_TypeDef Motorx)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_TypeDef *StepMotor_PORT[6] = {MOTOR1_ENABLE_PORT,MOTOR2_ENABLE_PORT,MOTOR3_ENABLE_PORT,MOTOR4_ENABLE_PORT,MOTOR5_ENABLE_PORT,MOTOR6_ENABLE_PORT};   //�˿�
  uint16_t   Motor_ENABLE_PINx[6] = {MOTOR1_ENABLE_PIN,MOTOR2_ENABLE_PIN,MOTOR3_ENABLE_PIN,MOTOR4_ENABLE_PIN,MOTOR5_ENABLE_PIN,MOTOR6_ENABLE_PIN};    //Pin
 
   // ����ENABLE�ţ��������  
  if(StepMotor_PORT[Motorx])
  {  
    GPIO_InitStructure.GPIO_Pin = Motor_ENABLE_PINx[Motorx];
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(StepMotor_PORT[Motorx], &GPIO_InitStructure);  
  }  
}
/*******************************************************************************
* Function Name  : StepMotor_RESET_Pin_Configuration
* Description    : ����MCU������������������RESET�ź�����
* �������       : Motorx - ������: ö���е�ֵ(0~5: ��Ӧ STEPMOTOR1~STEPMOTOR6)
* Output         : None
* Return         : None
*******************************************************************************/
void StepMotor_RESET_Pin_Configuration(StepMotor_TypeDef Motorx)
{
#ifdef MOTOR_RESET_EN 
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_TypeDef *StepMotor_PORT[6] = {MOTOR1_RESET_PORT,MOTOR2_RESET_PORT,MOTOR3_RESET_PORT,MOTOR4_RESET_PORT,MOTOR5_RESET_PORT,MOTOR6_RESET_PORT};   //�˿�
  uint16_t   Motor_RESET_PINx[6] =  {MOTOR1_RESET_PIN,MOTOR2_RESET_PIN,MOTOR3_RESET_PIN,MOTOR4_RESET_PIN,MOTOR5_RESET_PIN,MOTOR6_RESET_PIN};    //Pin
 
   // ����RESET�ţ��������  
  if(StepMotor_PORT[Motorx])
  {  
    GPIO_InitStructure.GPIO_Pin = Motor_RESET_PINx[Motorx];
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(StepMotor_PORT[Motorx], &GPIO_InitStructure);  
  }
#endif
}
/*******************************************************************************
* Function Name  : StepMotor_DCY1_Pin_Configuration
* Description    : ����MCU������������������DCY1�ź�����
* �������       : Motorx - ������: ö���е�ֵ(0~5: ��Ӧ STEPMOTOR1~STEPMOTOR6)
* Output         : None
* Return         : None
*******************************************************************************/
void StepMotor_DCY1_Pin_Configuration(StepMotor_TypeDef Motorx)
{
#ifdef MOTOR_DCY_EN  
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_TypeDef *StepMotor_PORT[6] = {MOTOR1_DCY1_PORT,MOTOR2_DCY1_PORT,MOTOR3_DCY1_PORT,MOTOR4_DCY1_PORT,MOTOR5_DCY1_PORT,MOTOR6_DCY1_PORT};   //�˿�
  uint16_t   Motor_DCY1_PINx[6] =   {MOTOR1_DCY1_PIN,MOTOR2_DCY1_PIN,MOTOR3_DCY1_PIN,MOTOR4_DCY1_PIN,MOTOR5_DCY1_PIN,MOTOR6_DCY1_PIN};    //Pin
 
   // ����DCY1�ţ��������  
  if(StepMotor_PORT[Motorx])
  {  
    GPIO_InitStructure.GPIO_Pin = Motor_DCY1_PINx[Motorx];
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(StepMotor_PORT[Motorx], &GPIO_InitStructure);  
  }
#endif
}
/*******************************************************************************
* Function Name  : StepMotor_DCY2_Pin_Configuration
* Description    : ����MCU������������������DCY2�ź�����
* �������       : Motorx - ������: ö���е�ֵ(0~5: ��Ӧ STEPMOTOR1~STEPMOTOR6)
* Output         : None
* Return         : None
*******************************************************************************/
void StepMotor_DCY2_Pin_Configuration(StepMotor_TypeDef Motorx)
{
#ifdef MOTOR_DCY_EN  
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_TypeDef *StepMotor_PORT[6] = {MOTOR1_DCY2_PORT,MOTOR2_DCY2_PORT,MOTOR3_DCY2_PORT,MOTOR4_DCY2_PORT,MOTOR5_DCY2_PORT,MOTOR6_DCY2_PORT};   //�˿�
  uint16_t   Motor_DCY2_PINx[6] =   {MOTOR1_DCY2_PIN,MOTOR2_DCY2_PIN,MOTOR3_DCY2_PIN,MOTOR4_DCY2_PIN,MOTOR5_DCY2_PIN,MOTOR6_DCY2_PIN};    //Pin
 
   // ����DCY2�ţ��������  
  if(StepMotor_PORT[Motorx])
  {  
    GPIO_InitStructure.GPIO_Pin = Motor_DCY2_PINx[Motorx];
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(StepMotor_PORT[Motorx], &GPIO_InitStructure);  
  }
#endif
}
/*******************************************************************************
* Function Name  : StepMotor_ALERT_Pin_Configuration
* Description    : ����MCU��ⲽ�������������ALERT�ź���������
* �������       : Motorx - ������: ö���е�ֵ(0~5: ��Ӧ STEPMOTOR1~STEPMOTOR6)
* Output         : None
* Return         : None
*******************************************************************************/
void StepMotor_ALERT_Pin_Configuration(StepMotor_TypeDef Motorx)
{
 #ifdef MOTOR_ALERT_EN
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_TypeDef *StepMotor_PORT[6] = {MOTOR1_ALERT_PORT,MOTOR2_ALERT_PORT,MOTOR3_ALERT_PORT,MOTOR4_ALERT_PORT,MOTOR5_ALERT_PORT,MOTOR6_ALERT_PORT};   //�˿�
  uint16_t   Motor_ALERT_PINx[6] =  {MOTOR1_ALERT_PIN,MOTOR2_ALERT_PIN,MOTOR3_ALERT_PIN,MOTOR4_ALERT_PIN,MOTOR5_ALERT_PIN,MOTOR6_ALERT_PIN};    //Pin
 
   // ����ALERT�ţ���������  
  if(StepMotor_PORT[Motorx])
  {  
    GPIO_InitStructure.GPIO_Pin = Motor_ALERT_PINx[Motorx];
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(StepMotor_PORT[Motorx], &GPIO_InitStructure);  
  }
#endif
}
/*******************************************************************************
* Function Name  : StepMotor_NF_Pin_Configuration
* Description    : ����MCU��ⲽ�������������NF(ȱ��)�ź���������
* �������       : Motorx - ������: ö���е�ֵ(0~5: ��Ӧ STEPMOTOR1~STEPMOTOR6)
* Output         : None
* Return         : None
*******************************************************************************/
void StepMotor_NF_Pin_Configuration(StepMotor_TypeDef Motorx)
{
#ifdef MOTOR_NF_EN 
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_TypeDef *StepMotor_PORT[6] = {MOTOR1_NF_PORT,MOTOR2_NF_PORT,MOTOR3_NF_PORT,MOTOR4_NF_PORT,MOTOR5_NF_PORT,MOTOR6_NF_PORT};   //�˿�
  uint16_t   Motor_NF_PINx[6] =    {MOTOR1_NF_PIN,MOTOR2_NF_PIN,MOTOR3_NF_PIN,MOTOR4_NF_PIN,MOTOR5_NF_PIN,MOTOR6_NF_PIN};    //Pin
 
   // ����NF�ţ���������  
  if(StepMotor_PORT[Motorx])
  {  
    GPIO_InitStructure.GPIO_Pin = Motor_NF_PINx[Motorx];
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(StepMotor_PORT[Motorx], &GPIO_InitStructure);  
  }
#endif
} 
/*******************************************************************************
* Function Name  : StepMotor_TOP_Pin_Configuration
* Description    : ����MCU��ⲽ���������(TOP)�ź���������
* �������       : Motorx - ������: ö���е�ֵ(0~5: ��Ӧ STEPMOTOR1~STEPMOTOR6)
* Output         : None
* Return         : None
*******************************************************************************/
void StepMotor_TOP_Pin_Configuration(StepMotor_TypeDef Motorx)
{
#ifdef MOTOR_TOP_CHECK_EN 
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_TypeDef *StepMotor_PORT[6] = {MOTOR1_TOP_PORT,MOTOR2_TOP_PORT,MOTOR3_TOP_PORT,MOTOR4_TOP_PORT,MOTOR5_TOP_PORT,MOTOR6_TOP_PORT};   //�˿�
  uint16_t   Motor_TOP_PINx[6] =    {MOTOR1_TOP_PIN,MOTOR2_TOP_PIN,MOTOR3_TOP_PIN,MOTOR4_TOP_PIN,MOTOR5_TOP_PIN,MOTOR6_TOP_PIN};    //Pin
 
   // ����TOP�ţ���������  
  if(StepMotor_PORT[Motorx])
  {  
    GPIO_InitStructure.GPIO_Pin = Motor_TOP_PINx[Motorx];
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(StepMotor_PORT[Motorx], &GPIO_InitStructure);  
  }
#endif
} 
/*******************************************************************************
* Function Name  : StepMotor_BOTTOM_Pin_Configuration
* Description    : ����MCU��ⲽ���������(BOTTOM)�ź���������
* �������       : Motorx - ������: ö���е�ֵ(0~5: ��Ӧ STEPMOTOR1~STEPMOTOR6)
* Output         : None
* Return         : None
*******************************************************************************/
void StepMotor_BOTTOM_Pin_Configuration(StepMotor_TypeDef Motorx)
{
#ifdef MOTOR_BOTTOM_CHECK_EN 
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_TypeDef *StepMotor_PORT[6] = {MOTOR1_BOTTOM_PORT,MOTOR2_BOTTOM_PORT,MOTOR3_BOTTOM_PORT,MOTOR4_BOTTOM_PORT,MOTOR5_BOTTOM_PORT,MOTOR6_BOTTOM_PORT};   //�˿�
  uint16_t   Motor_BOTTOM_PINx[6] =    {MOTOR1_BOTTOM_PIN,MOTOR2_BOTTOM_PIN,MOTOR3_BOTTOM_PIN,MOTOR4_BOTTOM_PIN,MOTOR5_BOTTOM_PIN,MOTOR6_BOTTOM_PIN};    //Pin
 
   // ����BOTTOM�ţ���������  
  if(StepMotor_PORT[Motorx])
  {  
    GPIO_InitStructure.GPIO_Pin = Motor_BOTTOM_PINx[Motorx];
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(StepMotor_PORT[Motorx], &GPIO_InitStructure);  
  }
#endif
} 
/*******************************************************************************
* Function Name  : StepMotor_BOTTOM_Pin_Configuration
* Description    : ����MCU���ǿ���ƶ�(_PC)�ź���������
* �������       : Motorx - ������: ö���е�ֵ(0~5: ��Ӧ STEPMOTOR1~STEPMOTOR6)
* Output         : None
* Return         : None
*******************************************************************************/
void StepMotor_BRAKING_Pin_Configuration(StepMotor_TypeDef Motorx)
{
#ifdef MOTOR_BRAKING_EN 
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_TypeDef *StepMotor_PORT[6]  = {MOTOR1_BRAKING_PORT,MOTOR2_BRAKING_PORT,MOTOR3_BRAKING_PORT,MOTOR4_BRAKING_PORT,MOTOR5_BRAKING_PORT,MOTOR6_BRAKING_PORT};   //�˿�
  uint16_t   Motor_BRAKING_PINx[6] = {MOTOR1_BRAKING_PIN,MOTOR2_BRAKING_PIN,MOTOR3_BRAKING_PIN,MOTOR4_BRAKING_PIN,MOTOR5_BRAKING_PIN,MOTOR6_BRAKING_PIN};    //Pin
 
   // ����BRAKING�ţ���������  
  if(StepMotor_PORT[Motorx])
  {  
    GPIO_InitStructure.GPIO_Pin = Motor_BRAKING_PINx[Motorx];
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(StepMotor_PORT[Motorx], &GPIO_InitStructure);  
  }
#endif
} 
/*******************************************************************************
* Function Name  : StepMotor_REAL_Pin_EXTI_Configuration
* Description    : ���ö�ת����GPIO����EXTI�ж�
* �������       : Motorx - ������: ö���е�ֵ(0~5: ��Ӧ STEPMOTOR1~STEPMOTOR6)
* Output         : None
* Return         : None
*******************************************************************************/
void StepMotor_REAL_Pin_EXTI_Configuration(StepMotor_TypeDef Motorx)
{
#ifdef MOTOR_REALSPEED_CHECK_EN 
  GPIO_InitTypeDef GPIO_InitStructure;
  EXTI_InitTypeDef EXTI_InitStructure;
  GPIO_TypeDef *StepMotor_PORT[6] = {MOTOR1_REAL_PORT,MOTOR2_REAL_PORT,MOTOR3_REAL_PORT,MOTOR4_REAL_PORT,MOTOR5_REAL_PORT,MOTOR6_REAL_PORT};   //�˿�
  uint16_t    Motor_REAL_PINx[6] =  {MOTOR1_REAL_PIN,MOTOR2_REAL_PIN,MOTOR3_REAL_PIN,MOTOR4_REAL_PIN,MOTOR5_REAL_PIN,MOTOR6_REAL_PIN};    //Pin
  uint16_t    Source_PORT[6] =     {MOTOR1_REAL_SourcePORT,MOTOR2_REAL_SourcePORT,MOTOR3_REAL_SourcePORT,MOTOR4_REAL_SourcePORT,MOTOR5_REAL_SourcePORT,MOTOR6_REAL_SourcePORT};   //�˿�
  uint16_t    Source_PINx[6] =     {MOTOR1_REAL_SourcePIN,MOTOR2_REAL_SourcePIN,MOTOR3_REAL_SourcePIN,MOTOR4_REAL_SourcePIN,MOTOR5_REAL_SourcePIN,MOTOR6_REAL_SourcePIN};    //Pin
  uint32_t    EXTI_Linex[6]  =     {MOTOR1_REAL_EXTI_Line,MOTOR2_REAL_EXTI_Line,MOTOR3_REAL_EXTI_Line,MOTOR4_REAL_EXTI_Line,MOTOR5_REAL_EXTI_Line,MOTOR6_REAL_EXTI_Line};    //Pin
 
   // ����REAL�ţ���������  
  if(StepMotor_PORT[Motorx])
  {  
    GPIO_InitStructure.GPIO_Pin = Motor_REAL_PINx[Motorx];
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(StepMotor_PORT[Motorx], &GPIO_InitStructure);  
    
    GPIO_EXTILineConfig(Source_PORT[Motorx], Source_PINx[Motorx]);
  
    /* Configure EXTI Linex to generate an interrupt on  edge */  
    EXTI_InitStructure.EXTI_Line = EXTI_Linex[Motorx];
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = MOTOR_REAL_EXTI_TRIGGER;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
  }
#endif
} 
/******************************************************************************
������:    StepMotor_Set_DCY
����ԭ��:  void StepMotor_Set_DCY(void)
��������:  ����ת��(��λ:1rpm)����DCY1,DCY2�ĵ�ƽֵ          
�������:  Motorx - ������: ö���е�ֵ(0~5: ��Ӧ STEPMOTOR1~STEPMOTOR6)
            speed - ת��(��λ:0.1RPM)
�������:  �� 
����ֵ:    --
******************************************************************************/
void StepMotor_Set_DCY(StepMotor_TypeDef Motorx,u16 speed)
{
#ifdef MOTOR_DCY_EN
  GPIO_TypeDef *DCY1_PORT[6] = {MOTOR1_DCY1_PORT,MOTOR2_DCY1_PORT,MOTOR3_DCY1_PORT,MOTOR4_DCY1_PORT,MOTOR5_DCY1_PORT,MOTOR6_DCY1_PORT};   //�˿�
  uint16_t      DCY1_PINx[6] = {MOTOR1_DCY1_PIN,MOTOR2_DCY1_PIN,MOTOR3_DCY1_PIN,MOTOR4_DCY1_PIN,MOTOR5_DCY1_PIN,MOTOR6_DCY1_PIN};    //Pin
  GPIO_TypeDef *DCY2_PORT[6] = {MOTOR1_DCY2_PORT,MOTOR2_DCY2_PORT,MOTOR3_DCY2_PORT,MOTOR4_DCY2_PORT,MOTOR5_DCY2_PORT,MOTOR6_DCY2_PORT};   //�˿�
  uint16_t      DCY2_PINx[6] = {MOTOR1_DCY2_PIN,MOTOR2_DCY2_PIN,MOTOR3_DCY2_PIN,MOTOR4_DCY2_PIN,MOTOR5_DCY2_PIN,MOTOR6_DCY2_PIN};    //Pin

  if(DCY1_PORT[Motorx] && DCY2_PORT[Motorx])  
  {  
    if(MOTOR_DCY_INVERTER == 1) //�����ƽ��Ҫ����
    {
      if(speed < DCY_VAL1)  //42���С��80RPM 20%��˥��
        {  //DCY2,DCY1=00
         GPIO_SetBits(DCY2_PORT[Motorx], DCY2_PINx[Motorx]);  //���1������������Ϊ0
         GPIO_SetBits(DCY1_PORT[Motorx], DCY1_PINx[Motorx]);  //���1������������Ϊ0
        }
      else if(speed < DCY_VAL2) //40%��˥��
        {  //DCY2,DCY1=01
         GPIO_SetBits(DCY2_PORT[Motorx], DCY2_PINx[Motorx]);  //���1������������Ϊ0
         GPIO_ResetBits(DCY1_PORT[Motorx], DCY1_PINx[Motorx]);  //���0������������Ϊ1
        }
      else if(speed < DCY_VAL3) //60%��˥��
        {  //DCY2,DCY1=10
         GPIO_ResetBits(DCY2_PORT[Motorx], DCY2_PINx[Motorx]);  //���0������������Ϊ1
         GPIO_SetBits(DCY1_PORT[Motorx], DCY1_PINx[Motorx]);  //���1������������Ϊ0
        }
      else   //80%��˥��
        {  //DCY2,DCY1=11
         GPIO_ResetBits(DCY2_PORT[Motorx], DCY2_PINx[Motorx]);  //���0������������Ϊ1
         GPIO_ResetBits(DCY1_PORT[Motorx], DCY1_PINx[Motorx]);  //���0������������Ϊ1
        }
    }
    else
    {
      if(speed < DCY_VAL1)  //42���С��80RPM 20%��˥��
        {  //DCY2,DCY1=00
         GPIO_ResetBits(DCY2_PORT[Motorx], DCY2_PINx[Motorx]);  //���0
         GPIO_ResetBits(DCY1_PORT[Motorx], DCY1_PINx[Motorx]);  //���0
        }
      else if(speed < DCY_VAL2) //40%��˥��
        {  //DCY2,DCY1=01
         GPIO_ResetBits(DCY2_PORT[Motorx], DCY2_PINx[Motorx]);  //���0
         GPIO_SetBits(DCY1_PORT[Motorx], DCY1_PINx[Motorx]);  //���1
        }
      else if(speed < DCY_VAL3) //60%��˥��
        {  //DCY2,DCY1=10
         GPIO_SetBits(DCY2_PORT[Motorx], DCY2_PINx[Motorx]);  //���1
         GPIO_ResetBits(DCY1_PORT[Motorx], DCY1_PINx[Motorx]);  //���0
        }
      else   //80%��˥��
        {  //DCY2,DCY1=11
         GPIO_SetBits(DCY2_PORT[Motorx], DCY2_PINx[Motorx]);  //���1
         GPIO_SetBits(DCY1_PORT[Motorx], DCY1_PINx[Motorx]);  //���1
        }
    }
  } 
#endif  
}
/*******************************************************************************
* Function Name  : StepMotor_Enable_Cmd
* Description    : ��ʹ�ܻ�ʧ�ܿ��� (����оƬRESET��ENABLE����Ϊ�ߵ�ƽ)
* �������       : Motorx - ������: ö���е�ֵ(0~5: ��Ӧ STEPMOTOR1~STEPMOTOR6)
*                  NewState - �������ENABLE��DISABLE
* Output         : None
* Return         : None
*******************************************************************************/
void StepMotor_Enable_Cmd(StepMotor_TypeDef Motorx,FunctionalState NewState)
{
  GPIO_TypeDef *ENABLE_PORT[6] = {MOTOR1_ENABLE_PORT,MOTOR2_ENABLE_PORT,MOTOR3_ENABLE_PORT,MOTOR4_ENABLE_PORT,MOTOR5_ENABLE_PORT,MOTOR6_ENABLE_PORT};   //�˿�
  uint16_t      ENABLE_PINx[6] = {MOTOR1_ENABLE_PIN,MOTOR2_ENABLE_PIN,MOTOR3_ENABLE_PIN,MOTOR4_ENABLE_PIN,MOTOR5_ENABLE_PIN,MOTOR6_ENABLE_PIN};    //Pin
  //���Ƶ�����RESET�������
#ifdef MOTOR_RESET_EN 
  GPIO_TypeDef *RESET_PORT[6] =  {MOTOR1_RESET_PORT,MOTOR2_RESET_PORT,MOTOR3_RESET_PORT,MOTOR4_RESET_PORT,MOTOR5_RESET_PORT,MOTOR6_RESET_PORT};   //�˿�
  uint16_t      RESET_PINx[6] =  {MOTOR1_RESET_PIN,MOTOR2_RESET_PIN,MOTOR3_RESET_PIN,MOTOR4_RESET_PIN,MOTOR5_RESET_PIN,MOTOR6_RESET_PIN};    //Pin
  if(RESET_PORT[Motorx])
  {  
    if(NewState == DISABLE) //��ֹ������ ����оƬRESET��Ϊ�͵�ƽ-��λ״̬
    {
      if(MOTOR_RESET_INVERTER == 1) //�з�����
      { GPIO_SetBits(RESET_PORT[Motorx], RESET_PINx[Motorx]); } //���1������������Ϊ0
      else  //�޷�����
      { GPIO_ResetBits(RESET_PORT[Motorx], RESET_PINx[Motorx]); } //���0
    }
    else    //ʹ�ܹ����� ����оƬRESET��Ϊ�ߵ�ƽ
    {
       if(MOTOR_RESET_INVERTER == 1) //�з�����
       {GPIO_ResetBits(RESET_PORT[Motorx], RESET_PINx[Motorx]); } //���0������������Ϊ1
       else
       {GPIO_SetBits(RESET_PORT[Motorx], RESET_PINx[Motorx]);}  //���1
    }
  }
#endif
  if(ENABLE_PORT[Motorx])
  {  
    //����ENABLE����(����RESET��֮����)
    if(NewState == DISABLE) //��ֹ������ ����оƬENABLE��Ϊ�͵�ƽ
    {
      if(MOTOR_ENABLE_INVERTER == 1)
      { GPIO_SetBits(ENABLE_PORT[Motorx], ENABLE_PINx[Motorx]); } //���1������������Ϊ0
      else
      { GPIO_ResetBits(ENABLE_PORT[Motorx], ENABLE_PINx[Motorx]); }  //���0
     }
    else    //ʹ�ܹ����� ����оƬENABLE��Ϊ�ߵ�ƽ
    {
      if(MOTOR_ENABLE_INVERTER == 1)
      { GPIO_ResetBits(ENABLE_PORT[Motorx], ENABLE_PINx[Motorx]); } //���0������������Ϊ1
      else
      { GPIO_SetBits(ENABLE_PORT[Motorx], ENABLE_PINx[Motorx]);   }//���1
    }
  }
}
/*******************************************************************************
* Function Name  : StepMotor_TIM_Cmd
* Description    : ��ʹ�õ�TIM����(ʹ�ܻ�ʧ��)
* �������       : Motorx - ������: ö���е�ֵ(0~5: ��Ӧ STEPMOTOR1~STEPMOTOR6)
*                  NewState - �������ENABLE��DISABLE
* Output         : None
* Return         : None
*******************************************************************************/
void StepMotor_TIM_Cmd(StepMotor_TypeDef Motorx,FunctionalState NewState)   
{
 TIM_TypeDef *StepMotor_Tim[6] = {MOTOR1_TIM,MOTOR2_TIM,MOTOR3_TIM,MOTOR4_TIM,MOTOR5_TIM,MOTOR6_TIM};   //����ʹ�õ�TIMx
// u8  StepMotor_Tim_IRQ[6] = {MOTOR1_TIM_IRQ,MOTOR2_TIM_IRQ,MOTOR3_TIM_IRQ,MOTOR4_TIM_IRQ,MOTOR5_TIM_IRQ,MOTOR6_TIM_IRQ};   //1=�ж���Ч
 
 if(StepMotor_Tim[Motorx])
  {  
     TIM_Cmd(StepMotor_Tim[Motorx],NewState);
     TIM_CtrlPWMOutputs(StepMotor_Tim[Motorx],NewState);
     
#ifdef MOTOR_PWM_IRQ_EN   //����PWM��TIM�ж���Ч����Ҫ�������������ۼ�

    TIM_ClearITPendingBit(StepMotor_Tim[Motorx], TIM_IT_Update );
    TIM_ITConfig(StepMotor_Tim[Motorx], TIM_IT_Update, NewState);

#endif
  }
}

/*******************************************************************************
* Function Name  : StepMotor_Set_DIR
* Description    : ������������������CW/CCW��������ź�
* �������       : Motorx - ������: ö���е�ֵ(0~5: ��Ӧ STEPMOTOR1~STEPMOTOR6)
*                  newdir - ����:0=��ת(����); 1=��ת(����) 
* Output         : None
* Return         : None
*******************************************************************************/
void StepMotor_Set_DIR(StepMotor_TypeDef Motorx, u8 newdir)
{
#ifdef MOTOR_DIR_EN 
  GPIO_TypeDef *StepMotor_PORT[6] = {MOTOR1_DIR_PORT,MOTOR2_DIR_PORT,MOTOR3_DIR_PORT,MOTOR4_DIR_PORT,MOTOR5_DIR_PORT,MOTOR6_DIR_PORT};   //�˿�
  uint16_t   Motor_DIR_PINx[6] =    {MOTOR1_DIR_PIN,MOTOR2_DIR_PIN,MOTOR3_DIR_PIN,MOTOR4_DIR_PIN,MOTOR5_DIR_PIN,MOTOR6_DIR_PIN};    //Pin
  
  //  ����CW/CCW���ţ��������  
  if(StepMotor_PORT[Motorx])
  {  
      if(newdir == 0)  //��ת - 0
    {
      if(MOTOR_DIR_INVERTER == 1)
      {    GPIO_SetBits(StepMotor_PORT[Motorx], Motor_DIR_PINx[Motorx]);  } //���1,�������Ϊ0
      else
      {    GPIO_ResetBits(StepMotor_PORT[Motorx], Motor_DIR_PINx[Motorx]);  }//���0
    }
    else  //��ת  -1
    {
      if(MOTOR_DIR_INVERTER == 1)
      {    GPIO_ResetBits(StepMotor_PORT[Motorx], Motor_DIR_PINx[Motorx]);  } //���0,�������Ϊ1
      else
      {    GPIO_SetBits(StepMotor_PORT[Motorx], Motor_DIR_PINx[Motorx]);  }//���1
    }
    if(StepMotor_Data[Motorx].OldDir != newdir) //����������л������ٶȽ�Ϊ0,Ȼ�����µ���. // ����ʱ
    { 
      StepMotor_Data[Motorx].CurrSpeed = 0;
    }
    StepMotor_Data[Motorx].OldDir = newdir;
  }
#endif  
}
/*******************************************************************************
* Function Name  : StepMotor_ALERT_Read
* Description    : ��ȡALERT�źţ�����״ֵ̬д��ṹ��alert������
* �������       : Motorx - ������: ö���е�ֵ(0~5: ��Ӧ STEPMOTOR1~STEPMOTOR6)
* Output         : None
* Return         : None
*******************************************************************************/
void StepMotor_ALERT_Read(StepMotor_TypeDef Motorx)
{
 #ifdef MOTOR_ALERT_EN
  GPIO_TypeDef *StepMotor_PORT[6] = {MOTOR1_ALERT_PORT,MOTOR2_ALERT_PORT,MOTOR3_ALERT_PORT,MOTOR4_ALERT_PORT,MOTOR5_ALERT_PORT,MOTOR6_ALERT_PORT};   //�˿�
  uint16_t   Motor_ALERT_PINx[6] =    {MOTOR1_ALERT_PIN,MOTOR2_ALERT_PIN,MOTOR3_ALERT_PIN,MOTOR4_ALERT_PIN,MOTOR5_ALERT_PIN,MOTOR6_ALERT_PIN};    //Pin
  static u16 cnt[6]={0}; //ȥ��������
  u8 pinval;
  if(StepMotor_PORT[Motorx])
  {  
    if(StepMotor_Data[Motorx].Status == ENABLE) //�������ʱ���ż��ALERT�ź�
     {  
      pinval = GPIO_ReadInputDataBit(StepMotor_PORT[Motorx], Motor_ALERT_PINx[Motorx]);
      if(MOTOR_ALERT_INVERTER == 1) //GPIO��Ϊ�ߵ�ƽ����
      {
        if(pinval != StepMotor_Data[Motorx].Alert)
        {
          cnt[Motorx]++;
          if(cnt[Motorx] > STEPMOTOR_IN_DEBOUNCE)
          {
            StepMotor_Data[Motorx].Alert = pinval;
          }
        }
        else
        { cnt[Motorx] = 0; }
      } 
      else  //GPIO��Ϊ�͵�ƽ����
      {
        if(pinval == StepMotor_Data[Motorx].Alert)
        {
          cnt[Motorx]++;
          if(cnt[Motorx] > STEPMOTOR_IN_DEBOUNCE)
          {
            StepMotor_Data[Motorx].Alert = !pinval;
          }
        }
        else
        { cnt[Motorx] = 0; }
      }
     }
  }

#endif
}
/*******************************************************************************
* Function Name  : StepMotor_NF_Read
* Description    : ��ȡNF�źţ�����״ֵ̬д��ṹ��nfa������
* �������       : Motorx - ������: ö���е�ֵ(0~5: ��Ӧ STEPMOTOR1~STEPMOTOR6)
* Output         : None
* Return         : None
ȱ���ⷽ���� 10S��һֱ���ֱ�����ƽ���򱨾�
*******************************************************************************/
void StepMotor_NF_Read(StepMotor_TypeDef Motorx)
{
 #ifdef MOTOR_NF_EN
  GPIO_TypeDef *StepMotor_PORT[6] = {MOTOR1_NF_PORT,MOTOR2_NF_PORT,MOTOR3_NF_PORT,MOTOR4_NF_PORT,MOTOR5_NF_PORT,MOTOR6_NF_PORT};   //�˿�
  uint16_t   Motor_NF_PINx[6] =    {MOTOR1_NF_PIN,MOTOR2_NF_PIN,MOTOR3_NF_PIN,MOTOR4_NF_PIN,MOTOR5_NF_PIN,MOTOR6_NF_PIN};    //Pin  
  u8 pinval;
  if(StepMotor_PORT[Motorx])
  {  
     if(StepMotor_Data[Motorx].Status == ENABLE) //�������ʱ���ż��NF�ź�
     {  
      pinval = GPIO_ReadInputDataBit(StepMotor_PORT[Motorx], Motor_NF_PINx[Motorx]);
      if(pinval == MOTOR_NF_INVERTER) //GPIO��Ϊ������ƽ
      {
          if(NF_Cnt[Motorx] > STEPMOTOR_NF_CHECK_10S) //Ҫ��>10S
          {
            StepMotor_Data[Motorx].NF = 1;
          }
      }
      else  //Ϊ������ƽ���������������ʱ��
      { 
          StepMotor_Data[Motorx].NF = 0;
          NF_Cnt[Motorx] = 0; 
      } 
     }
//     else   //
//     {
//          StepMotor_Data[Motorx].NF = 0;
//          NF_Cnt[Motorx] = 0; 
//     }
  }
#endif
}

/*******************************************************************************
* Function Name  : StepMotor_TOP_Read
* Description    : ��ȡ�����źţ�����״ֵ̬д��ṹ��ToTop������
* �������       : Motorx - ������: ö���е�ֵ(0~5: ��Ӧ STEPMOTOR1~STEPMOTOR6)
* Output         : None
* Return         : None
*******************************************************************************/
void StepMotor_TOP_Read(StepMotor_TypeDef Motorx)
{
 #ifdef MOTOR_TOP_CHECK_EN
  GPIO_TypeDef *StepMotor_PORT[6] = {MOTOR1_TOP_PORT,MOTOR2_TOP_PORT,MOTOR3_TOP_PORT,MOTOR4_TOP_PORT,MOTOR5_TOP_PORT,MOTOR6_TOP_PORT};   //�˿�
  uint16_t   Motor_TOP_PINx[6] =    {MOTOR1_TOP_PIN,MOTOR2_TOP_PIN,MOTOR3_TOP_PIN,MOTOR4_TOP_PIN,MOTOR5_TOP_PIN,MOTOR6_TOP_PIN};    //Pin
  static u16 cnt[6]={0}; //ȥ��������
  u8 pinval;

  if(StepMotor_PORT[Motorx])
  {  
     if(StepMotor_Data[Motorx].Status == ENABLE) //�������ʱ���ż��top�ź�
     {  
      pinval = GPIO_ReadInputDataBit(StepMotor_PORT[Motorx], Motor_TOP_PINx[Motorx]);
      if(MOTOR_TOP_INVERTER == 1) //GPIO��Ϊ�ߵ�ƽ��λ
      {
        if(pinval != StepMotor_Data[Motorx].ToTop)
        {
          cnt[Motorx]++;
          if(cnt[Motorx] > STEPMOTOR_IN_DEBOUNCE)
          {
            StepMotor_Data[Motorx].ToTop = pinval;
          }
        }
        else
        { cnt[Motorx] = 0; }
      } 
      else  //GPIO��Ϊ�͵�ƽ��λ
      {
        if(pinval == StepMotor_Data[Motorx].ToTop)
        {
          cnt[Motorx]++;
          if(cnt[Motorx] > STEPMOTOR_IN_DEBOUNCE)
          {
            StepMotor_Data[Motorx].ToTop = !pinval;
          }
        }
        else
        { cnt[Motorx] = 0; }
      }
      if((StepMotor_Data[Motorx].ToTop==1)&&(StepMotor_Data[Motorx].SetDir==1)) //�����ҷ�ת����رյ��
      {StepMotor_Data[Motorx].Enable = DISABLE; }
     }
  }
#endif
}
/*******************************************************************************
* Function Name  : StepMotor_BOTTOM_Read
* Description    : ��ȡ�����źţ�����״ֵ̬д��ṹ��ToBottom������
* �������       : Motorx - ������: ö���е�ֵ(0~5: ��Ӧ STEPMOTOR1~STEPMOTOR6)
* Output         : None
* Return         : None
*******************************************************************************/
void StepMotor_BOTTOM_Read(StepMotor_TypeDef Motorx)
{
 #ifdef MOTOR_BOTTOM_CHECK_EN
  GPIO_TypeDef *StepMotor_PORT[6] = {MOTOR1_BOTTOM_PORT,MOTOR2_BOTTOM_PORT,MOTOR3_BOTTOM_PORT,MOTOR4_BOTTOM_PORT,MOTOR5_BOTTOM_PORT,MOTOR6_BOTTOM_PORT};   //�˿�
  uint16_t   Motor_BOTTOM_PINx[6] = {MOTOR1_BOTTOM_PIN,MOTOR2_BOTTOM_PIN,MOTOR3_BOTTOM_PIN,MOTOR4_BOTTOM_PIN,MOTOR5_BOTTOM_PIN,MOTOR6_BOTTOM_PIN};    //Pin
  static u16 cnt[6]={0}; //ȥ��������
  u8 pinval;
  if(StepMotor_PORT[Motorx])
  {  
     if(StepMotor_Data[Motorx].Status == ENABLE) //�������ʱ���ż��Bottom�ź�
     {  
      pinval = GPIO_ReadInputDataBit(StepMotor_PORT[Motorx], Motor_BOTTOM_PINx[Motorx]);
      if(MOTOR_BOTTOM_INVERTER == 1) //GPIO��Ϊ�ߵ�ƽ��λ
      {
        if(pinval != StepMotor_Data[Motorx].ToBottom)
        {
          cnt[Motorx]++;
          if(cnt[Motorx] > STEPMOTOR_IN_DEBOUNCE)
          {
            StepMotor_Data[Motorx].ToBottom = pinval;
          }
        }
        else
        { cnt[Motorx] = 0; }
      } 
      else  //GPIO��Ϊ�͵�ƽ��λ
      {
        if(pinval == StepMotor_Data[Motorx].ToBottom)
        {
          cnt[Motorx]++;
          if(cnt[Motorx] > STEPMOTOR_IN_DEBOUNCE)
          {
            StepMotor_Data[Motorx].ToBottom = !pinval;
          }
        }
        else
        { cnt[Motorx] = 0; }
      }
      if((StepMotor_Data[Motorx].ToBottom==1)&&(StepMotor_Data[Motorx].SetDir==0)&&(Motorx!=4)) //��������ת����رյ�������رó���
        {StepMotor_Data[Motorx].Enable = DISABLE; } 
     }
  }
#endif
}
/*******************************************************************************
* Function Name  : StepMotor_BOTTOM_Pin_Configuration
* Description    : ��ȡǿ���ƶ�(�øǵ�)��������źţ�����״ֵ̬д��ṹ��Braking������
* �������       : Motorx - ������: ö���е�ֵ(0~5: ��Ӧ STEPMOTOR1~STEPMOTOR6)
* Output         : None
* Return         : None
*******************************************************************************/
void StepMotor_BRAKING_Read(StepMotor_TypeDef Motorx)
{
#ifdef MOTOR_BRAKING_EN 
  GPIO_TypeDef *StepMotor_PORT[6]  = {MOTOR1_BRAKING_PORT,MOTOR2_BRAKING_PORT,MOTOR3_BRAKING_PORT,MOTOR4_BRAKING_PORT,MOTOR5_BRAKING_PORT,MOTOR6_BRAKING_PORT};   //�˿�
  uint16_t   Motor_BRAKING_PINx[6] = {MOTOR1_BRAKING_PIN,MOTOR2_BRAKING_PIN,MOTOR3_BRAKING_PIN,MOTOR4_BRAKING_PIN,MOTOR5_BRAKING_PIN,MOTOR6_BRAKING_PIN};    //Pin
 
  static u16 cnt[6]={0}; //ȥ��������
  u8 pinval;
  if(StepMotor_PORT[Motorx])  //ǿ���ƶ�(�øǵ�)������� ��Ч
  {  
      pinval = GPIO_ReadInputDataBit(StepMotor_PORT[Motorx], Motor_BRAKING_PINx[Motorx]);
      if(MOTOR_BRAKING_INVERTER == 1) //�øǴ�ʱ,GPIO��Ϊ�ߵ�ƽ
      {
        if(pinval != StepMotor_Data[Motorx].Braking)
        {
          cnt[Motorx]++;
          if(cnt[Motorx] > STEPMOTOR_IN_DEBOUNCE)
          {
            StepMotor_Data[Motorx].Braking = pinval;
          }
        }
        else
        { cnt[Motorx] = 0; }
      } 
      else  //�øǴ�ʱ,GPIO��Ϊ�͵�ƽ
      {
        if(pinval == StepMotor_Data[Motorx].Braking)
        {
          cnt[Motorx]++;
          if(cnt[Motorx] > STEPMOTOR_IN_DEBOUNCE)
          {
            StepMotor_Data[Motorx].Braking = !pinval;
          }
        }
        else
        { cnt[Motorx] = 0; }
      }
    if(StepMotor_Data[Motorx].Braking == 1)  //ǿ���ƶ���Ч���򱣴�����״̬����ֹͣ����
    {
      StepMotor_Data[Motorx].RunSta_Brak = StepMotor_Data[Motorx].Enable;
      StepMotor_Data[Motorx].Enable = 0;
    }
    else    //ǿ���ƶ���Ч�����Զ��ָ�����״̬
    {
      StepMotor_Data[Motorx].Enable = StepMotor_Data[Motorx].RunSta_Brak;
    }
    
  }
  else    //ǿ���ƶ�(�øǵ�)������� ��Ч����BrakingΪ0
  {
    StepMotor_Data[Motorx].Braking = 0;  
  }
#else    //ǿ���ƶ�(�øǵ�)������� ��Ч����BrakingΪ0
    StepMotor_Data[Motorx].Braking = 0;  
#endif
}
/******************************************************************************
������:    StepMotor_Speed_ADJ
����ԭ��:  void StepMotor_Speed_ADJ(void)
��������:  ����ת��(0.1rpm)����DCY1,DCY2�ĵ�ƽֵ          
�������:  Motorx - ������: ö���е�ֵ(0~5: ��Ӧ STEPMOTOR1~STEPMOTOR6)
            Turn_speed - ת��(��λ:1RPM)
�������:  �� 
����ֵ:    --
******************************************************************************/
void StepMotor_Speed_ADJ(StepMotor_TypeDef Motorx,float Turn_speed)   
{
 uint32_t Per,div;
 TIM_TypeDef *StepMotor_Tim[6] = {MOTOR1_TIM,MOTOR2_TIM,MOTOR3_TIM,MOTOR4_TIM,MOTOR5_TIM,MOTOR6_TIM};   //����ʹ�õ�TIMx
  u8   TIM_CHx[6] =              {MOTOR1_CH,MOTOR2_CH,MOTOR3_CH,MOTOR4_CH,MOTOR5_CH,MOTOR6_CH};    //����ʹ�õ�TIM��CHx
 
 TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
 TIM_OCInitTypeDef  TIM_OCInitStructure;

 if(StepMotor_Tim[Motorx] && TIM_CHx[Motorx])  //TIM��CH����Ч
 { 
     StepMotor_Set_DCY(Motorx,(u16)Turn_speed);
     
     /*************ת����Ƶ��������û����ϵ****************
      *��֪�����У�ϸ�ֶ�=64;�����=1.8��;SetSpeedΪת��(fload��),��λΪrpm;PWM��TIM�Ĺ���ʱ��Ϊ72Mhz���У�
      *   ÿ���ת�ٶ�Ϊ��SetSpeed/60(r/s) 
      *   ���תһȦ����Ҫ��������Ϊ: (360��/1.8��)*64=200*64=12800(������/r)   
      *��һ��ת���µ�����Ƶ��Ϊ: Frequency=(SetSpeed/60)*12800 (������/s) 
      *  ��:TIM_Period=72000000/Frequency = 72000000*60/(12800*SetSpeed)=337500/SetSpeed
     ******************************************************/
     Per=(uint32_t)(STEPMOTOR_COEFFICIENT/Turn_speed+0.5); 
     if(Per<=65535) //ת��С��5.15(r/min) 
      {  
        TIM_TimeBaseStructure.TIM_Period =Per-1;   //�Զ���װ��ֵ-PWM����          
        TIM_TimeBaseStructure.TIM_Prescaler =0;    //����Ƶ
        TIM_TimeBaseStructure.TIM_ClockDivision =TIM_CKD_DIV1;      //ʱ�ӱ�Ƶ����              
        TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // ���ϼ���ģʽ
      }
      else        //ת�ٴ���5.15(r/min) 
      {  
       // Per=(uint32_t)((Per/1000.0)+0.5); 
        div = (Per >> 16);  //��Ƶֵ 
        Per = Per/(div+1);
        TIM_TimeBaseStructure.TIM_Period =Per-1;    //�Զ���װ��ֵ-PWM����         
        TIM_TimeBaseStructure.TIM_Prescaler = div;   //��Ƶ (��Ƶϵ��-1)
        TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;      //ʱ�ӱ�Ƶ���ӣ����ʱ�ӹ���������������
        TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //���ϼ���ģʽ
      }
      TIM_TimeBaseInit(StepMotor_Tim[Motorx], &TIM_TimeBaseStructure); 
        //���� CHx��PWMģʽ  
      TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;             //����PWM���ģʽ
      TIM_OCInitStructure.TIM_Pulse = Per/2;    //����ռ�ձ�
      TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;    //ʹ�ܸ�ͨ�����
      TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;  //�߼���ʱ��-ʹ�ܻ��������
      TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;         //�����������
      TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;      //�߼���ʱ��-���û������������
      TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;       //�߼���ʱ��-���������״̬
      TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;    //�߼���ʱ��-�����󻥲������״̬
      
      switch(TIM_CHx[Motorx]) //ѡͨPWM��Ӧ��CHxͨ��
      {
        case 1:
            TIM_OC1Init(StepMotor_Tim[Motorx], &TIM_OCInitStructure); //ͨ��1ʹ��
        break;
        case 2:
            TIM_OC2Init(StepMotor_Tim[Motorx], &TIM_OCInitStructure); //ͨ��2ʹ��
        break;
        case 3:
            TIM_OC3Init(StepMotor_Tim[Motorx], &TIM_OCInitStructure); //ͨ��3ʹ��
        break;
        case 4:
            TIM_OC4Init(StepMotor_Tim[Motorx], &TIM_OCInitStructure); //ͨ��4ʹ��
        break;
      }
 }
}
/*******************************************************************************
* Function Name  : StepMotor_Work_Control
* Description    : �ù�������������:�ٶ��Զ�������״̬����
* �������       : 
* Output         : None
* Return         : None
* ʹ��           : ����ѭ�������е���
*******************************************************************************/
void StepMotor_Work_Control(void)
{
  u8 mi,adjsta,clrsta;
  u32 adjcnt;
  
  adjcnt = StepMotor_ADJ_Cnt;
  clrsta = 0;   //�Ƿ���0���ټ��ʱ��������ı�־λ,6���ù���
  for(mi = 0; mi < MOTOR_TOTAL; mi++)   //��ѯ������
  {
    adjsta = 0;
    if(StepMotor_Data[mi].SetSpeed == 0) 
      StepMotor_Data[mi].Enable = DISABLE;
    if(StepMotor_Data[mi].Enable == DISABLE) //�ػ�����
    {
      if(StepMotor_Data[mi].Status != DISABLE)  //��ǰΪ����״̬����ִ��ͣ������
      {
        StepMotor_Data[mi].Status = DISABLE;
        StepMotor_Data[mi].CurrSpeed = 0;
        StepMotor_Enable_Cmd((StepMotor_TypeDef)mi,DISABLE);
        StepMotor_TIM_Cmd((StepMotor_TypeDef)mi,DISABLE);
      }
    }
    else    //��������
    {
      if(StepMotor_Data[mi].Status != ENABLE) //��ǰΪͣ��״̬����������ʱ����
      {
        StepMotor_Data[mi].Status = ENABLE;
        StepMotor_Data[mi].CurrSpeed = 0;
        StepMotor_Set_DIR((StepMotor_TypeDef)mi,StepMotor_Data[mi].SetDir);
        StepMotor_Enable_Cmd((StepMotor_TypeDef)mi,ENABLE);
        if(StepMotor_Data[mi].CurrSpeed == 0) StepMotor_Data[mi].CurrSpeed+=STEPMOTOR_ADJSPEED_STEP;
        StepMotor_Speed_ADJ((StepMotor_TypeDef)mi, StepMotor_Data[mi].CurrSpeed);
        StepMotor_TIM_Cmd((StepMotor_TypeDef)mi,ENABLE);
        clrsta=1;
      }
      //�����ڼ�: ���÷���,��ȡ����״̬��
      StepMotor_Set_DIR((StepMotor_TypeDef)mi,StepMotor_Data[mi].SetDir);
      StepMotor_ALERT_Read((StepMotor_TypeDef)mi);
      StepMotor_NF_Read((StepMotor_TypeDef)mi);
      StepMotor_BOTTOM_Read((StepMotor_TypeDef)mi);
      StepMotor_TOP_Read((StepMotor_TypeDef)mi);
      StepMotor_BRAKING_Read((StepMotor_TypeDef)mi);
      
      if(StepMotor_Data[mi].SetSpeed > STEPMOTOR_MAX_SETSPEED) StepMotor_Data[mi].SetSpeed=STEPMOTOR_MAX_SETSPEED;
      //��ǰ�ٶ����趨�ٶȲ���ȣ��Ҵﵽһ����ʱ������,�ŵ�����ݼ����趨ת��
      if((StepMotor_Data[mi].CurrSpeed != StepMotor_Data[mi].SetSpeed)&&(adjcnt > STEPMOTOR_ADJSPEED_SPACE))   
      { 
        if(StepMotor_Data[mi].CurrSpeed < StepMotor_Data[mi].SetSpeed) //����
        {
          adjsta = 1;
           StepMotor_Data[mi].CurrSpeed += STEPMOTOR_ADJSPEED_STEP;
           if(StepMotor_Data[mi].CurrSpeed > StepMotor_Data[mi].SetSpeed)  //�������趨ֵ
           {
             StepMotor_Data[mi].CurrSpeed = StepMotor_Data[mi].SetSpeed;
           }
        }
        else //����
        {
             adjsta = 1;   
           if(StepMotor_Data[mi].CurrSpeed >= STEPMOTOR_ADJSPEED_STEP) 
           {
             StepMotor_Data[mi].CurrSpeed -= STEPMOTOR_ADJSPEED_STEP;   //�ݼ�
           }
           else 
           {
              StepMotor_Data[mi].CurrSpeed = 0;     //�ݼ���0
           }
           if(StepMotor_Data[mi].CurrSpeed < StepMotor_Data[mi].SetSpeed) //�ݼ����趨ֵ
           {
             StepMotor_Data[mi].CurrSpeed = StepMotor_Data[mi].SetSpeed; 
           }
        } 
        if(adjsta == 1)  //����ò���ɨ���У��б��ٶȵ���
         {
           StepMotor_Speed_ADJ((StepMotor_TypeDef)mi, StepMotor_Data[mi].CurrSpeed);
           clrsta = 1;
         }
      }
     } 
   }
   if(clrsta == 1)StepMotor_ADJ_Cnt = 0;   //6�����У�������һ����ִ�����ٶȵ��ڣ�����0������
}

/*******************************************************************************
* Function Name  : StepMotor_PWM_TIM_IRQ
* Description    : PWM��ʱ���жϴ������
* �������       : Motorx - ������: ö���е�ֵ(0~5: ��Ӧ STEPMOTOR1~STEPMOTOR6)
* Output         : None
* Return         : None
*******************************************************************************/
void StepMotor_PWM_TIM_IRQ(StepMotor_TypeDef Motorx)   
{
  static u16 pwmpulse_real[MOTOR_TOTAL] = {0};
  u8 ENCODER[6] = {MOTOR1_ENCODER,MOTOR2_ENCODER,MOTOR3_ENCODER,MOTOR4_ENCODER,MOTOR5_ENCODER,MOTOR6_ENCODER};  
  u16 pulse_encoder;
#ifdef MOTOR_PWM_IRQ_EN   //����PWM��TIM�ж���Ч����Ҫ�������������ۼ�
  TIM_TypeDef *StepMotor_Tim[6] = {MOTOR1_TIM,MOTOR2_TIM,MOTOR3_TIM,MOTOR4_TIM,MOTOR5_TIM,MOTOR6_TIM};   //����ʹ�õ�TIMx

 if (TIM_GetITStatus(StepMotor_Tim[Motorx], TIM_IT_Update) != RESET)
 { 
    TIM_ClearITPendingBit(StepMotor_Tim[Motorx], TIM_IT_Update );
    StepMotor_Data[Motorx].Flow++;      //�������ۼ�
#ifdef MOTOR_PWM_LOCKROTOR_CHECK_EN        //�ж��Ƿ��ת
    StepMotor_Data[Motorx].FlowLockRotor++;
    pulse_encoder = STEPMOTOR_PWMPULSE / ENCODER[Motorx];  //ÿһ����ת�����������Ӧ��PWM���������� = 12800/����
    if(StepMotor_Data[Motorx].Flow==StepMotor_Data[Motorx].Pluse)    //JK
    {
     StepMotor_Data[Motorx].Enable =  DISABLE;
    }
    if(StepMotor_Data[Motorx].FlowLockRotor > (pulse_encoder + (pulse_encoder >> 1))) //����ÿ����������50%���ж�һ���Ƿ��ת
    { //��ʱ����������� ��׼������1.5�����ж��Ƿ��ж�ת
      if(StepMotor_Data[Motorx].LockRotorSpeed == 0)
      {
        StepMotor_Data[Motorx].LockRotor = 1; //��ת
      }
      else
      {
        StepMotor_Data[Motorx].LockRotor = 0; //δ��ת
      }
        StepMotor_Data[Motorx].FlowLockRotor = 0;
        StepMotor_Data[Motorx].LockRotorSpeed = 0;
    }
#endif  
#ifdef MOTOR_REALSPEED_CHECK_EN        //��ʵת�ٲ���
    if(StepMotor_Data[Motorx].SetSpeed == StepMotor_Data[Motorx].CurrSpeed)
    {
      pwmpulse_real[Motorx]++;
      if(pwmpulse_real[Motorx] >= 12800)
      {
        StepMotor_Data[Motorx].RealSpeed = StepMotor_Data[Motorx].RealPulse / ENCODER[Motorx];
      }
    }
    else
    {
      pwmpulse_real[Motorx] = 0;
    }
#endif
 } 
#endif
}
/*******************************************************************************
* Function Name  : StepMotor_EXTI_IRQ
* Description    : �ٶȷ�����̖��EXTI�Д�̎�����
* �������       : Motorx - ������: ö���е�ֵ(0~5: ��Ӧ STEPMOTOR1~STEPMOTOR6)
* Output         : None
* Return         : None
*******************************************************************************/
void StepMotor_EXTI_IRQ(StepMotor_TypeDef Motorx)   
{
#ifdef MOTOR_REALSPEED_CHECK_EN    
 uint32_t    EXTI_Linex[6]  =  {MOTOR1_REAL_EXTI_Line,MOTOR2_REAL_EXTI_Line,MOTOR3_REAL_EXTI_Line,MOTOR4_REAL_EXTI_Line,MOTOR5_REAL_EXTI_Line,MOTOR6_REAL_EXTI_Line};    //Pin

if(EXTI_GetITStatus(EXTI_Linex[Motorx])!=RESET)//�ж�ĳ�����ϵ��ж��Ƿ��� 
  { 
   EXTI_ClearITPendingBit(EXTI_Linex[Motorx]);  //��� LINE �ϵ��жϱ�־λ    
   StepMotor_Data[Motorx].LockRotorSpeed++;     //�ۼ�ʵ�ʼ�⵽��ת���������   
   StepMotor_Data[Motorx].RealPulse++;          //�ۼ�ʵ�ʼ�⵽��ת���������   
  }   
#endif 
}



/******************* (C) COPYRIGHT 2008 SWS *****END OF FILE*******************/

