/*************************** (C) COPYRIGHT 2008 SWS ****************************

* File Name          : stepmotor_config.c
* Author             : 周友兵
* Version            : V1.0
* Date               : 2008.9.19
* Description        : 步进电机的硬件配置
未完成项：
   1.方向控制
   2.到顶/到底位置检测
   3.转速检测(HALL或编码盘等)
   4.ALERT和NF缺相等报警信息检测

*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "stm32f10x_tim.h"
#include "sws_stepmotor_v2.h"


StepMotor_WorkInfo_Def StepMotor_Data[MOTOR_TOTAL]; //电机运行参数，数据个数最多为4个
u32 StepMotor_ADJ_Cnt = 0;
u16 NF_Cnt[MOTOR_TOTAL] = {0};
u16 Hall_Speed[MOTOR_TOTAL] = {0};

/*******************************************************************************
* Function Name  : StepMotor_Init
* Description    : 步进电机驱动参数初始化及其GPIO配置
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void StepMotor_Init(void)
{
  u8 i;
  
  for(i = 0; i < MOTOR_TOTAL; i++)  //清空工作状态
  {
    StepMotor_Data[i].Enable = DISABLE;
    StepMotor_Data[i].Status = DISABLE;  //正常应初始化为DISABLE，这里初始化为ENABLE的目的是让处理程序释放电机
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
  
  for(i = 0; i < MOTOR_TOTAL; i++) //配置GPIO
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
* Description    : 配置步进电机驱动器使用的系统时钟(包括TIM、GPIO)
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
//void StepMotor_RCC_Configuration(void)
//{ 
//  //系统时钟主频设置
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
//  //Enable AFIO clocks    -- 引脚重映射需要的RCC,如果没有使用到引脚重映射，可以注释掉
// // RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
//  
//   // ****************** TIM3引脚重映射************************
// // GPIO_PinRemapConfig(GPIO_FullRemap_TIM3,ENABLE); //重映射TIM3引脚
//
//}

/*******************************************************************************
* Function Name  : StepMotor_PWM_Pin_Configuration
* Description    : 配置MCU输出步进电机驱动器的PWM信号引脚
* 输入参数       : Motorx - 电机编号: 枚举中的值(0~5: 对应 STEPMOTOR1~STEPMOTOR6)
* Output         : None
* Return         : None
*******************************************************************************/
void StepMotor_PWM_Pin_Configuration(StepMotor_TypeDef Motorx)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_TypeDef *StepMotor_PORT[6] = {MOTOR1_PWM_PORT,MOTOR2_PWM_PORT,MOTOR3_PWM_PORT,MOTOR4_PWM_PORT,MOTOR5_PWM_PORT,MOTOR6_PWM_PORT};   //端口
  uint16_t   Motor_PWM_PINx[6] =    {MOTOR1_PWM_PIN,MOTOR2_PWM_PIN,MOTOR3_PWM_PIN,MOTOR4_PWM_PIN,MOTOR5_PWM_PIN,MOTOR6_PWM_PIN};    //Pin
  
  //  配置CLK(PWM)脚：复用推挽输出 
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
* Description    : 配置MCU输出步进电机驱动器的CW/CCW方向控制信号引脚
* 输入参数       : Motorx - 电机编号: 枚举中的值(0~5: 对应 STEPMOTOR1~STEPMOTOR6)
* Output         : None
* Return         : None
*******************************************************************************/
void StepMotor_DIR_Pin_Configuration(StepMotor_TypeDef Motorx)
{
#ifdef MOTOR_DIR_EN 
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_TypeDef *StepMotor_PORT[6] = {MOTOR1_DIR_PORT,MOTOR2_DIR_PORT,MOTOR3_DIR_PORT,MOTOR4_DIR_PORT,MOTOR5_DIR_PORT,MOTOR6_DIR_PORT};   //端口
  uint16_t   Motor_DIR_PINx[6] =    {MOTOR1_DIR_PIN,MOTOR2_DIR_PIN,MOTOR3_DIR_PIN,MOTOR4_DIR_PIN,MOTOR5_DIR_PIN,MOTOR6_DIR_PIN};    //Pin
  
  //  配置CW/CCW引脚：推挽输出  
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
* Description    : 配置MCU输出步进电机驱动器的ENABLE信号引脚
* 输入参数       : Motorx - 电机编号: 枚举中的值(0~5: 对应 STEPMOTOR1~STEPMOTOR6)
* Output         : None
* Return         : None
*******************************************************************************/
void StepMotor_ENABLE_Pin_Configuration(StepMotor_TypeDef Motorx)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_TypeDef *StepMotor_PORT[6] = {MOTOR1_ENABLE_PORT,MOTOR2_ENABLE_PORT,MOTOR3_ENABLE_PORT,MOTOR4_ENABLE_PORT,MOTOR5_ENABLE_PORT,MOTOR6_ENABLE_PORT};   //端口
  uint16_t   Motor_ENABLE_PINx[6] = {MOTOR1_ENABLE_PIN,MOTOR2_ENABLE_PIN,MOTOR3_ENABLE_PIN,MOTOR4_ENABLE_PIN,MOTOR5_ENABLE_PIN,MOTOR6_ENABLE_PIN};    //Pin
 
   // 配置ENABLE脚：推挽输出  
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
* Description    : 配置MCU输出步进电机驱动器的RESET信号引脚
* 输入参数       : Motorx - 电机编号: 枚举中的值(0~5: 对应 STEPMOTOR1~STEPMOTOR6)
* Output         : None
* Return         : None
*******************************************************************************/
void StepMotor_RESET_Pin_Configuration(StepMotor_TypeDef Motorx)
{
#ifdef MOTOR_RESET_EN 
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_TypeDef *StepMotor_PORT[6] = {MOTOR1_RESET_PORT,MOTOR2_RESET_PORT,MOTOR3_RESET_PORT,MOTOR4_RESET_PORT,MOTOR5_RESET_PORT,MOTOR6_RESET_PORT};   //端口
  uint16_t   Motor_RESET_PINx[6] =  {MOTOR1_RESET_PIN,MOTOR2_RESET_PIN,MOTOR3_RESET_PIN,MOTOR4_RESET_PIN,MOTOR5_RESET_PIN,MOTOR6_RESET_PIN};    //Pin
 
   // 配置RESET脚：推挽输出  
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
* Description    : 配置MCU输出步进电机驱动器的DCY1信号引脚
* 输入参数       : Motorx - 电机编号: 枚举中的值(0~5: 对应 STEPMOTOR1~STEPMOTOR6)
* Output         : None
* Return         : None
*******************************************************************************/
void StepMotor_DCY1_Pin_Configuration(StepMotor_TypeDef Motorx)
{
#ifdef MOTOR_DCY_EN  
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_TypeDef *StepMotor_PORT[6] = {MOTOR1_DCY1_PORT,MOTOR2_DCY1_PORT,MOTOR3_DCY1_PORT,MOTOR4_DCY1_PORT,MOTOR5_DCY1_PORT,MOTOR6_DCY1_PORT};   //端口
  uint16_t   Motor_DCY1_PINx[6] =   {MOTOR1_DCY1_PIN,MOTOR2_DCY1_PIN,MOTOR3_DCY1_PIN,MOTOR4_DCY1_PIN,MOTOR5_DCY1_PIN,MOTOR6_DCY1_PIN};    //Pin
 
   // 配置DCY1脚：推挽输出  
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
* Description    : 配置MCU输出步进电机驱动器的DCY2信号引脚
* 输入参数       : Motorx - 电机编号: 枚举中的值(0~5: 对应 STEPMOTOR1~STEPMOTOR6)
* Output         : None
* Return         : None
*******************************************************************************/
void StepMotor_DCY2_Pin_Configuration(StepMotor_TypeDef Motorx)
{
#ifdef MOTOR_DCY_EN  
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_TypeDef *StepMotor_PORT[6] = {MOTOR1_DCY2_PORT,MOTOR2_DCY2_PORT,MOTOR3_DCY2_PORT,MOTOR4_DCY2_PORT,MOTOR5_DCY2_PORT,MOTOR6_DCY2_PORT};   //端口
  uint16_t   Motor_DCY2_PINx[6] =   {MOTOR1_DCY2_PIN,MOTOR2_DCY2_PIN,MOTOR3_DCY2_PIN,MOTOR4_DCY2_PIN,MOTOR5_DCY2_PIN,MOTOR6_DCY2_PIN};    //Pin
 
   // 配置DCY2脚：推挽输出  
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
* Description    : 配置MCU检测步进电机驱动器的ALERT信号输入引脚
* 输入参数       : Motorx - 电机编号: 枚举中的值(0~5: 对应 STEPMOTOR1~STEPMOTOR6)
* Output         : None
* Return         : None
*******************************************************************************/
void StepMotor_ALERT_Pin_Configuration(StepMotor_TypeDef Motorx)
{
 #ifdef MOTOR_ALERT_EN
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_TypeDef *StepMotor_PORT[6] = {MOTOR1_ALERT_PORT,MOTOR2_ALERT_PORT,MOTOR3_ALERT_PORT,MOTOR4_ALERT_PORT,MOTOR5_ALERT_PORT,MOTOR6_ALERT_PORT};   //端口
  uint16_t   Motor_ALERT_PINx[6] =  {MOTOR1_ALERT_PIN,MOTOR2_ALERT_PIN,MOTOR3_ALERT_PIN,MOTOR4_ALERT_PIN,MOTOR5_ALERT_PIN,MOTOR6_ALERT_PIN};    //Pin
 
   // 配置ALERT脚：浮空输入  
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
* Description    : 配置MCU检测步进电机驱动器的NF(缺相)信号输入引脚
* 输入参数       : Motorx - 电机编号: 枚举中的值(0~5: 对应 STEPMOTOR1~STEPMOTOR6)
* Output         : None
* Return         : None
*******************************************************************************/
void StepMotor_NF_Pin_Configuration(StepMotor_TypeDef Motorx)
{
#ifdef MOTOR_NF_EN 
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_TypeDef *StepMotor_PORT[6] = {MOTOR1_NF_PORT,MOTOR2_NF_PORT,MOTOR3_NF_PORT,MOTOR4_NF_PORT,MOTOR5_NF_PORT,MOTOR6_NF_PORT};   //端口
  uint16_t   Motor_NF_PINx[6] =    {MOTOR1_NF_PIN,MOTOR2_NF_PIN,MOTOR3_NF_PIN,MOTOR4_NF_PIN,MOTOR5_NF_PIN,MOTOR6_NF_PIN};    //Pin
 
   // 配置NF脚：浮空输入  
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
* Description    : 配置MCU检测步进电机到顶(TOP)信号输入引脚
* 输入参数       : Motorx - 电机编号: 枚举中的值(0~5: 对应 STEPMOTOR1~STEPMOTOR6)
* Output         : None
* Return         : None
*******************************************************************************/
void StepMotor_TOP_Pin_Configuration(StepMotor_TypeDef Motorx)
{
#ifdef MOTOR_TOP_CHECK_EN 
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_TypeDef *StepMotor_PORT[6] = {MOTOR1_TOP_PORT,MOTOR2_TOP_PORT,MOTOR3_TOP_PORT,MOTOR4_TOP_PORT,MOTOR5_TOP_PORT,MOTOR6_TOP_PORT};   //端口
  uint16_t   Motor_TOP_PINx[6] =    {MOTOR1_TOP_PIN,MOTOR2_TOP_PIN,MOTOR3_TOP_PIN,MOTOR4_TOP_PIN,MOTOR5_TOP_PIN,MOTOR6_TOP_PIN};    //Pin
 
   // 配置TOP脚：浮空输入  
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
* Description    : 配置MCU检测步进电机到底(BOTTOM)信号输入引脚
* 输入参数       : Motorx - 电机编号: 枚举中的值(0~5: 对应 STEPMOTOR1~STEPMOTOR6)
* Output         : None
* Return         : None
*******************************************************************************/
void StepMotor_BOTTOM_Pin_Configuration(StepMotor_TypeDef Motorx)
{
#ifdef MOTOR_BOTTOM_CHECK_EN 
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_TypeDef *StepMotor_PORT[6] = {MOTOR1_BOTTOM_PORT,MOTOR2_BOTTOM_PORT,MOTOR3_BOTTOM_PORT,MOTOR4_BOTTOM_PORT,MOTOR5_BOTTOM_PORT,MOTOR6_BOTTOM_PORT};   //端口
  uint16_t   Motor_BOTTOM_PINx[6] =    {MOTOR1_BOTTOM_PIN,MOTOR2_BOTTOM_PIN,MOTOR3_BOTTOM_PIN,MOTOR4_BOTTOM_PIN,MOTOR5_BOTTOM_PIN,MOTOR6_BOTTOM_PIN};    //Pin
 
   // 配置BOTTOM脚：浮空输入  
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
* Description    : 配置MCU检测强制制动(_PC)信号输入引脚
* 输入参数       : Motorx - 电机编号: 枚举中的值(0~5: 对应 STEPMOTOR1~STEPMOTOR6)
* Output         : None
* Return         : None
*******************************************************************************/
void StepMotor_BRAKING_Pin_Configuration(StepMotor_TypeDef Motorx)
{
#ifdef MOTOR_BRAKING_EN 
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_TypeDef *StepMotor_PORT[6]  = {MOTOR1_BRAKING_PORT,MOTOR2_BRAKING_PORT,MOTOR3_BRAKING_PORT,MOTOR4_BRAKING_PORT,MOTOR5_BRAKING_PORT,MOTOR6_BRAKING_PORT};   //端口
  uint16_t   Motor_BRAKING_PINx[6] = {MOTOR1_BRAKING_PIN,MOTOR2_BRAKING_PIN,MOTOR3_BRAKING_PIN,MOTOR4_BRAKING_PIN,MOTOR5_BRAKING_PIN,MOTOR6_BRAKING_PIN};    //Pin
 
   // 配置BRAKING脚：浮空输入  
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
* Description    : 配置堵转检测的GPIO及其EXTI中断
* 输入参数       : Motorx - 电机编号: 枚举中的值(0~5: 对应 STEPMOTOR1~STEPMOTOR6)
* Output         : None
* Return         : None
*******************************************************************************/
void StepMotor_REAL_Pin_EXTI_Configuration(StepMotor_TypeDef Motorx)
{
#ifdef MOTOR_REALSPEED_CHECK_EN 
  GPIO_InitTypeDef GPIO_InitStructure;
  EXTI_InitTypeDef EXTI_InitStructure;
  GPIO_TypeDef *StepMotor_PORT[6] = {MOTOR1_REAL_PORT,MOTOR2_REAL_PORT,MOTOR3_REAL_PORT,MOTOR4_REAL_PORT,MOTOR5_REAL_PORT,MOTOR6_REAL_PORT};   //端口
  uint16_t    Motor_REAL_PINx[6] =  {MOTOR1_REAL_PIN,MOTOR2_REAL_PIN,MOTOR3_REAL_PIN,MOTOR4_REAL_PIN,MOTOR5_REAL_PIN,MOTOR6_REAL_PIN};    //Pin
  uint16_t    Source_PORT[6] =     {MOTOR1_REAL_SourcePORT,MOTOR2_REAL_SourcePORT,MOTOR3_REAL_SourcePORT,MOTOR4_REAL_SourcePORT,MOTOR5_REAL_SourcePORT,MOTOR6_REAL_SourcePORT};   //端口
  uint16_t    Source_PINx[6] =     {MOTOR1_REAL_SourcePIN,MOTOR2_REAL_SourcePIN,MOTOR3_REAL_SourcePIN,MOTOR4_REAL_SourcePIN,MOTOR5_REAL_SourcePIN,MOTOR6_REAL_SourcePIN};    //Pin
  uint32_t    EXTI_Linex[6]  =     {MOTOR1_REAL_EXTI_Line,MOTOR2_REAL_EXTI_Line,MOTOR3_REAL_EXTI_Line,MOTOR4_REAL_EXTI_Line,MOTOR5_REAL_EXTI_Line,MOTOR6_REAL_EXTI_Line};    //Pin
 
   // 配置REAL脚：浮空输入  
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
函数名:    StepMotor_Set_DCY
函数原形:  void StepMotor_Set_DCY(void)
功能描述:  根据转速(单位:1rpm)设置DCY1,DCY2的电平值          
输入参数:  Motorx - 电机编号: 枚举中的值(0~5: 对应 STEPMOTOR1~STEPMOTOR6)
            speed - 转速(单位:0.1RPM)
输出参数:  无 
返回值:    --
******************************************************************************/
void StepMotor_Set_DCY(StepMotor_TypeDef Motorx,u16 speed)
{
#ifdef MOTOR_DCY_EN
  GPIO_TypeDef *DCY1_PORT[6] = {MOTOR1_DCY1_PORT,MOTOR2_DCY1_PORT,MOTOR3_DCY1_PORT,MOTOR4_DCY1_PORT,MOTOR5_DCY1_PORT,MOTOR6_DCY1_PORT};   //端口
  uint16_t      DCY1_PINx[6] = {MOTOR1_DCY1_PIN,MOTOR2_DCY1_PIN,MOTOR3_DCY1_PIN,MOTOR4_DCY1_PIN,MOTOR5_DCY1_PIN,MOTOR6_DCY1_PIN};    //Pin
  GPIO_TypeDef *DCY2_PORT[6] = {MOTOR1_DCY2_PORT,MOTOR2_DCY2_PORT,MOTOR3_DCY2_PORT,MOTOR4_DCY2_PORT,MOTOR5_DCY2_PORT,MOTOR6_DCY2_PORT};   //端口
  uint16_t      DCY2_PINx[6] = {MOTOR1_DCY2_PIN,MOTOR2_DCY2_PIN,MOTOR3_DCY2_PIN,MOTOR4_DCY2_PIN,MOTOR5_DCY2_PIN,MOTOR6_DCY2_PIN};    //Pin

  if(DCY1_PORT[Motorx] && DCY2_PORT[Motorx])  
  {  
    if(MOTOR_DCY_INVERTER == 1) //输出电平需要反相
    {
      if(speed < DCY_VAL1)  //42电机小于80RPM 20%快衰减
        {  //DCY2,DCY1=00
         GPIO_SetBits(DCY2_PORT[Motorx], DCY2_PINx[Motorx]);  //输出1，经反相器后为0
         GPIO_SetBits(DCY1_PORT[Motorx], DCY1_PINx[Motorx]);  //输出1，经反相器后为0
        }
      else if(speed < DCY_VAL2) //40%快衰减
        {  //DCY2,DCY1=01
         GPIO_SetBits(DCY2_PORT[Motorx], DCY2_PINx[Motorx]);  //输出1，经反相器后为0
         GPIO_ResetBits(DCY1_PORT[Motorx], DCY1_PINx[Motorx]);  //输出0，经反相器后为1
        }
      else if(speed < DCY_VAL3) //60%快衰减
        {  //DCY2,DCY1=10
         GPIO_ResetBits(DCY2_PORT[Motorx], DCY2_PINx[Motorx]);  //输出0，经反相器后为1
         GPIO_SetBits(DCY1_PORT[Motorx], DCY1_PINx[Motorx]);  //输出1，经反相器后为0
        }
      else   //80%快衰减
        {  //DCY2,DCY1=11
         GPIO_ResetBits(DCY2_PORT[Motorx], DCY2_PINx[Motorx]);  //输出0，经反相器后为1
         GPIO_ResetBits(DCY1_PORT[Motorx], DCY1_PINx[Motorx]);  //输出0，经反相器后为1
        }
    }
    else
    {
      if(speed < DCY_VAL1)  //42电机小于80RPM 20%快衰减
        {  //DCY2,DCY1=00
         GPIO_ResetBits(DCY2_PORT[Motorx], DCY2_PINx[Motorx]);  //输出0
         GPIO_ResetBits(DCY1_PORT[Motorx], DCY1_PINx[Motorx]);  //输出0
        }
      else if(speed < DCY_VAL2) //40%快衰减
        {  //DCY2,DCY1=01
         GPIO_ResetBits(DCY2_PORT[Motorx], DCY2_PINx[Motorx]);  //输出0
         GPIO_SetBits(DCY1_PORT[Motorx], DCY1_PINx[Motorx]);  //输出1
        }
      else if(speed < DCY_VAL3) //60%快衰减
        {  //DCY2,DCY1=10
         GPIO_SetBits(DCY2_PORT[Motorx], DCY2_PINx[Motorx]);  //输出1
         GPIO_ResetBits(DCY1_PORT[Motorx], DCY1_PINx[Motorx]);  //输出0
        }
      else   //80%快衰减
        {  //DCY2,DCY1=11
         GPIO_SetBits(DCY2_PORT[Motorx], DCY2_PINx[Motorx]);  //输出1
         GPIO_SetBits(DCY1_PORT[Motorx], DCY1_PINx[Motorx]);  //输出1
        }
    }
  } 
#endif  
}
/*******************************************************************************
* Function Name  : StepMotor_Enable_Cmd
* Description    : 泵使能或失能控制 (驱动芯片RESET和ENABLE引脚为高电平)
* 输入参数       : Motorx - 电机编号: 枚举中的值(0~5: 对应 STEPMOTOR1~STEPMOTOR6)
*                  NewState - 控制命令：ENABLE或DISABLE
* Output         : None
* Return         : None
*******************************************************************************/
void StepMotor_Enable_Cmd(StepMotor_TypeDef Motorx,FunctionalState NewState)
{
  GPIO_TypeDef *ENABLE_PORT[6] = {MOTOR1_ENABLE_PORT,MOTOR2_ENABLE_PORT,MOTOR3_ENABLE_PORT,MOTOR4_ENABLE_PORT,MOTOR5_ENABLE_PORT,MOTOR6_ENABLE_PORT};   //端口
  uint16_t      ENABLE_PINx[6] = {MOTOR1_ENABLE_PIN,MOTOR2_ENABLE_PIN,MOTOR3_ENABLE_PIN,MOTOR4_ENABLE_PIN,MOTOR5_ENABLE_PIN,MOTOR6_ENABLE_PIN};    //Pin
  //控制单独的RESET引脚输出
#ifdef MOTOR_RESET_EN 
  GPIO_TypeDef *RESET_PORT[6] =  {MOTOR1_RESET_PORT,MOTOR2_RESET_PORT,MOTOR3_RESET_PORT,MOTOR4_RESET_PORT,MOTOR5_RESET_PORT,MOTOR6_RESET_PORT};   //端口
  uint16_t      RESET_PINx[6] =  {MOTOR1_RESET_PIN,MOTOR2_RESET_PIN,MOTOR3_RESET_PIN,MOTOR4_RESET_PIN,MOTOR5_RESET_PIN,MOTOR6_RESET_PIN};    //Pin
  if(RESET_PORT[Motorx])
  {  
    if(NewState == DISABLE) //禁止工作： 驱动芯片RESET脚为低电平-复位状态
    {
      if(MOTOR_RESET_INVERTER == 1) //有反相器
      { GPIO_SetBits(RESET_PORT[Motorx], RESET_PINx[Motorx]); } //输出1，经反相器后为0
      else  //无反相器
      { GPIO_ResetBits(RESET_PORT[Motorx], RESET_PINx[Motorx]); } //输出0
    }
    else    //使能工作： 驱动芯片RESET脚为高电平
    {
       if(MOTOR_RESET_INVERTER == 1) //有反相器
       {GPIO_ResetBits(RESET_PORT[Motorx], RESET_PINx[Motorx]); } //输出0，经反相器后为1
       else
       {GPIO_SetBits(RESET_PORT[Motorx], RESET_PINx[Motorx]);}  //输出1
    }
  }
#endif
  if(ENABLE_PORT[Motorx])
  {  
    //控制ENABLE引脚(包括RESET与之并联)
    if(NewState == DISABLE) //禁止工作： 驱动芯片ENABLE脚为低电平
    {
      if(MOTOR_ENABLE_INVERTER == 1)
      { GPIO_SetBits(ENABLE_PORT[Motorx], ENABLE_PINx[Motorx]); } //输出1，经反相器后为0
      else
      { GPIO_ResetBits(ENABLE_PORT[Motorx], ENABLE_PINx[Motorx]); }  //输出0
     }
    else    //使能工作： 驱动芯片ENABLE脚为高电平
    {
      if(MOTOR_ENABLE_INVERTER == 1)
      { GPIO_ResetBits(ENABLE_PORT[Motorx], ENABLE_PINx[Motorx]); } //输出0，经反相器后为1
      else
      { GPIO_SetBits(ENABLE_PORT[Motorx], ENABLE_PINx[Motorx]);   }//输出1
    }
  }
}
/*******************************************************************************
* Function Name  : StepMotor_TIM_Cmd
* Description    : 泵使用的TIM控制(使能或失能)
* 输入参数       : Motorx - 电机编号: 枚举中的值(0~5: 对应 STEPMOTOR1~STEPMOTOR6)
*                  NewState - 控制命令：ENABLE或DISABLE
* Output         : None
* Return         : None
*******************************************************************************/
void StepMotor_TIM_Cmd(StepMotor_TypeDef Motorx,FunctionalState NewState)   
{
 TIM_TypeDef *StepMotor_Tim[6] = {MOTOR1_TIM,MOTOR2_TIM,MOTOR3_TIM,MOTOR4_TIM,MOTOR5_TIM,MOTOR6_TIM};   //各泵使用的TIMx
// u8  StepMotor_Tim_IRQ[6] = {MOTOR1_TIM_IRQ,MOTOR2_TIM_IRQ,MOTOR3_TIM_IRQ,MOTOR4_TIM_IRQ,MOTOR5_TIM_IRQ,MOTOR6_TIM_IRQ};   //1=中断有效
 
 if(StepMotor_Tim[Motorx])
  {  
     TIM_Cmd(StepMotor_Tim[Motorx],NewState);
     TIM_CtrlPWMOutputs(StepMotor_Tim[Motorx],NewState);
     
#ifdef MOTOR_PWM_IRQ_EN   //配置PWM的TIM中断有效，主要用于流量脉冲累计

    TIM_ClearITPendingBit(StepMotor_Tim[Motorx], TIM_IT_Update );
    TIM_ITConfig(StepMotor_Tim[Motorx], TIM_IT_Update, NewState);

#endif
  }
}

/*******************************************************************************
* Function Name  : StepMotor_Set_DIR
* Description    : 输出步进电机驱动器的CW/CCW方向控制信号
* 输入参数       : Motorx - 电机编号: 枚举中的值(0~5: 对应 STEPMOTOR1~STEPMOTOR6)
*                  newdir - 方向:0=正转(到底); 1=反转(到顶) 
* Output         : None
* Return         : None
*******************************************************************************/
void StepMotor_Set_DIR(StepMotor_TypeDef Motorx, u8 newdir)
{
#ifdef MOTOR_DIR_EN 
  GPIO_TypeDef *StepMotor_PORT[6] = {MOTOR1_DIR_PORT,MOTOR2_DIR_PORT,MOTOR3_DIR_PORT,MOTOR4_DIR_PORT,MOTOR5_DIR_PORT,MOTOR6_DIR_PORT};   //端口
  uint16_t   Motor_DIR_PINx[6] =    {MOTOR1_DIR_PIN,MOTOR2_DIR_PIN,MOTOR3_DIR_PIN,MOTOR4_DIR_PIN,MOTOR5_DIR_PIN,MOTOR6_DIR_PIN};    //Pin
  
  //  配置CW/CCW引脚：推挽输出  
  if(StepMotor_PORT[Motorx])
  {  
      if(newdir == 0)  //正转 - 0
    {
      if(MOTOR_DIR_INVERTER == 1)
      {    GPIO_SetBits(StepMotor_PORT[Motorx], Motor_DIR_PINx[Motorx]);  } //输出1,经反相后为0
      else
      {    GPIO_ResetBits(StepMotor_PORT[Motorx], Motor_DIR_PINx[Motorx]);  }//输出0
    }
    else  //反转  -1
    {
      if(MOTOR_DIR_INVERTER == 1)
      {    GPIO_ResetBits(StepMotor_PORT[Motorx], Motor_DIR_PINx[Motorx]);  } //输出0,经反相后为1
      else
      {    GPIO_SetBits(StepMotor_PORT[Motorx], Motor_DIR_PINx[Motorx]);  }//输出1
    }
    if(StepMotor_Data[Motorx].OldDir != newdir) //如果方向有切换，则速度降为0,然后重新调速. // 则延时
    { 
      StepMotor_Data[Motorx].CurrSpeed = 0;
    }
    StepMotor_Data[Motorx].OldDir = newdir;
  }
#endif  
}
/*******************************************************************************
* Function Name  : StepMotor_ALERT_Read
* Description    : 读取ALERT信号，把其状态值写入结构的alert变量中
* 输入参数       : Motorx - 电机编号: 枚举中的值(0~5: 对应 STEPMOTOR1~STEPMOTOR6)
* Output         : None
* Return         : None
*******************************************************************************/
void StepMotor_ALERT_Read(StepMotor_TypeDef Motorx)
{
 #ifdef MOTOR_ALERT_EN
  GPIO_TypeDef *StepMotor_PORT[6] = {MOTOR1_ALERT_PORT,MOTOR2_ALERT_PORT,MOTOR3_ALERT_PORT,MOTOR4_ALERT_PORT,MOTOR5_ALERT_PORT,MOTOR6_ALERT_PORT};   //端口
  uint16_t   Motor_ALERT_PINx[6] =    {MOTOR1_ALERT_PIN,MOTOR2_ALERT_PIN,MOTOR3_ALERT_PIN,MOTOR4_ALERT_PIN,MOTOR5_ALERT_PIN,MOTOR6_ALERT_PIN};    //Pin
  static u16 cnt[6]={0}; //去抖计数器
  u8 pinval;
  if(StepMotor_PORT[Motorx])
  {  
    if(StepMotor_Data[Motorx].Status == ENABLE) //电机工作时，才检测ALERT信号
     {  
      pinval = GPIO_ReadInputDataBit(StepMotor_PORT[Motorx], Motor_ALERT_PINx[Motorx]);
      if(MOTOR_ALERT_INVERTER == 1) //GPIO端为高电平报警
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
      else  //GPIO端为低电平报警
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
* Description    : 读取NF信号，把其状态值写入结构的nfa变量中
* 输入参数       : Motorx - 电机编号: 枚举中的值(0~5: 对应 STEPMOTOR1~STEPMOTOR6)
* Output         : None
* Return         : None
缺相检测方法： 10S内一直保持报警电平，则报警
*******************************************************************************/
void StepMotor_NF_Read(StepMotor_TypeDef Motorx)
{
 #ifdef MOTOR_NF_EN
  GPIO_TypeDef *StepMotor_PORT[6] = {MOTOR1_NF_PORT,MOTOR2_NF_PORT,MOTOR3_NF_PORT,MOTOR4_NF_PORT,MOTOR5_NF_PORT,MOTOR6_NF_PORT};   //端口
  uint16_t   Motor_NF_PINx[6] =    {MOTOR1_NF_PIN,MOTOR2_NF_PIN,MOTOR3_NF_PIN,MOTOR4_NF_PIN,MOTOR5_NF_PIN,MOTOR6_NF_PIN};    //Pin  
  u8 pinval;
  if(StepMotor_PORT[Motorx])
  {  
     if(StepMotor_Data[Motorx].Status == ENABLE) //电机工作时，才检测NF信号
     {  
      pinval = GPIO_ReadInputDataBit(StepMotor_PORT[Motorx], Motor_NF_PINx[Motorx]);
      if(pinval == MOTOR_NF_INVERTER) //GPIO端为报警电平
      {
          if(NF_Cnt[Motorx] > STEPMOTOR_NF_CHECK_10S) //要求>10S
          {
            StepMotor_Data[Motorx].NF = 1;
          }
      }
      else  //为正常电平，则清除报警及计时器
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
* Description    : 读取到顶信号，把其状态值写入结构的ToTop变量中
* 输入参数       : Motorx - 电机编号: 枚举中的值(0~5: 对应 STEPMOTOR1~STEPMOTOR6)
* Output         : None
* Return         : None
*******************************************************************************/
void StepMotor_TOP_Read(StepMotor_TypeDef Motorx)
{
 #ifdef MOTOR_TOP_CHECK_EN
  GPIO_TypeDef *StepMotor_PORT[6] = {MOTOR1_TOP_PORT,MOTOR2_TOP_PORT,MOTOR3_TOP_PORT,MOTOR4_TOP_PORT,MOTOR5_TOP_PORT,MOTOR6_TOP_PORT};   //端口
  uint16_t   Motor_TOP_PINx[6] =    {MOTOR1_TOP_PIN,MOTOR2_TOP_PIN,MOTOR3_TOP_PIN,MOTOR4_TOP_PIN,MOTOR5_TOP_PIN,MOTOR6_TOP_PIN};    //Pin
  static u16 cnt[6]={0}; //去抖计数器
  u8 pinval;

  if(StepMotor_PORT[Motorx])
  {  
     if(StepMotor_Data[Motorx].Status == ENABLE) //电机工作时，才检测top信号
     {  
      pinval = GPIO_ReadInputDataBit(StepMotor_PORT[Motorx], Motor_TOP_PINx[Motorx]);
      if(MOTOR_TOP_INVERTER == 1) //GPIO端为高电平到位
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
      else  //GPIO端为低电平到位
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
      if((StepMotor_Data[Motorx].ToTop==1)&&(StepMotor_Data[Motorx].SetDir==1)) //到顶且反转，则关闭电机
      {StepMotor_Data[Motorx].Enable = DISABLE; }
     }
  }
#endif
}
/*******************************************************************************
* Function Name  : StepMotor_BOTTOM_Read
* Description    : 读取到底信号，把其状态值写入结构的ToBottom变量中
* 输入参数       : Motorx - 电机编号: 枚举中的值(0~5: 对应 STEPMOTOR1~STEPMOTOR6)
* Output         : None
* Return         : None
*******************************************************************************/
void StepMotor_BOTTOM_Read(StepMotor_TypeDef Motorx)
{
 #ifdef MOTOR_BOTTOM_CHECK_EN
  GPIO_TypeDef *StepMotor_PORT[6] = {MOTOR1_BOTTOM_PORT,MOTOR2_BOTTOM_PORT,MOTOR3_BOTTOM_PORT,MOTOR4_BOTTOM_PORT,MOTOR5_BOTTOM_PORT,MOTOR6_BOTTOM_PORT};   //端口
  uint16_t   Motor_BOTTOM_PINx[6] = {MOTOR1_BOTTOM_PIN,MOTOR2_BOTTOM_PIN,MOTOR3_BOTTOM_PIN,MOTOR4_BOTTOM_PIN,MOTOR5_BOTTOM_PIN,MOTOR6_BOTTOM_PIN};    //Pin
  static u16 cnt[6]={0}; //去抖计数器
  u8 pinval;
  if(StepMotor_PORT[Motorx])
  {  
     if(StepMotor_Data[Motorx].Status == ENABLE) //电机工作时，才检测Bottom信号
     {  
      pinval = GPIO_ReadInputDataBit(StepMotor_PORT[Motorx], Motor_BOTTOM_PINx[Motorx]);
      if(MOTOR_BOTTOM_INVERTER == 1) //GPIO端为高电平到位
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
      else  //GPIO端为低电平到位
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
      if((StepMotor_Data[Motorx].ToBottom==1)&&(StepMotor_Data[Motorx].SetDir==0)&&(Motorx!=4)) //到底且正转，则关闭电机，肝素泵除外
        {StepMotor_Data[Motorx].Enable = DISABLE; } 
     }
  }
#endif
}
/*******************************************************************************
* Function Name  : StepMotor_BOTTOM_Pin_Configuration
* Description    : 读取强制制动(泵盖等)检测引脚信号，把其状态值写入结构的Braking变量中
* 输入参数       : Motorx - 电机编号: 枚举中的值(0~5: 对应 STEPMOTOR1~STEPMOTOR6)
* Output         : None
* Return         : None
*******************************************************************************/
void StepMotor_BRAKING_Read(StepMotor_TypeDef Motorx)
{
#ifdef MOTOR_BRAKING_EN 
  GPIO_TypeDef *StepMotor_PORT[6]  = {MOTOR1_BRAKING_PORT,MOTOR2_BRAKING_PORT,MOTOR3_BRAKING_PORT,MOTOR4_BRAKING_PORT,MOTOR5_BRAKING_PORT,MOTOR6_BRAKING_PORT};   //端口
  uint16_t   Motor_BRAKING_PINx[6] = {MOTOR1_BRAKING_PIN,MOTOR2_BRAKING_PIN,MOTOR3_BRAKING_PIN,MOTOR4_BRAKING_PIN,MOTOR5_BRAKING_PIN,MOTOR6_BRAKING_PIN};    //Pin
 
  static u16 cnt[6]={0}; //去抖计数器
  u8 pinval;
  if(StepMotor_PORT[Motorx])  //强制制动(泵盖等)检测引脚 有效
  {  
      pinval = GPIO_ReadInputDataBit(StepMotor_PORT[Motorx], Motor_BRAKING_PINx[Motorx]);
      if(MOTOR_BRAKING_INVERTER == 1) //泵盖打开时,GPIO端为高电平
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
      else  //泵盖打开时,GPIO端为低电平
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
    if(StepMotor_Data[Motorx].Braking == 1)  //强制制动有效，则保存运行状态，并停止运行
    {
      StepMotor_Data[Motorx].RunSta_Brak = StepMotor_Data[Motorx].Enable;
      StepMotor_Data[Motorx].Enable = 0;
    }
    else    //强制制动无效，则自动恢复运行状态
    {
      StepMotor_Data[Motorx].Enable = StepMotor_Data[Motorx].RunSta_Brak;
    }
    
  }
  else    //强制制动(泵盖等)检测引脚 无效，则Braking为0
  {
    StepMotor_Data[Motorx].Braking = 0;  
  }
#else    //强制制动(泵盖等)检测引脚 无效，则Braking为0
    StepMotor_Data[Motorx].Braking = 0;  
#endif
}
/******************************************************************************
函数名:    StepMotor_Speed_ADJ
函数原形:  void StepMotor_Speed_ADJ(void)
功能描述:  根据转速(0.1rpm)设置DCY1,DCY2的电平值          
输入参数:  Motorx - 电机编号: 枚举中的值(0~5: 对应 STEPMOTOR1~STEPMOTOR6)
            Turn_speed - 转速(单位:1RPM)
输出参数:  无 
返回值:    --
******************************************************************************/
void StepMotor_Speed_ADJ(StepMotor_TypeDef Motorx,float Turn_speed)   
{
 uint32_t Per,div;
 TIM_TypeDef *StepMotor_Tim[6] = {MOTOR1_TIM,MOTOR2_TIM,MOTOR3_TIM,MOTOR4_TIM,MOTOR5_TIM,MOTOR6_TIM};   //各泵使用的TIMx
  u8   TIM_CHx[6] =              {MOTOR1_CH,MOTOR2_CH,MOTOR3_CH,MOTOR4_CH,MOTOR5_CH,MOTOR6_CH};    //各泵使用的TIM的CHx
 
 TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
 TIM_OCInitTypeDef  TIM_OCInitStructure;

 if(StepMotor_Tim[Motorx] && TIM_CHx[Motorx])  //TIM及CH号有效
 { 
     StepMotor_Set_DCY(Motorx,(u16)Turn_speed);
     
     /*************转速与频率输出设置换算关系****************
      *已知条件有：细分度=64;步距角=1.8度;SetSpeed为转速(fload型),单位为rpm;PWM的TIM的工作时钟为72Mhz则有：
      *   每秒的转速度为：SetSpeed/60(r/s) 
      *   电机转一圈所需要的脉冲数为: (360度/1.8度)*64=200*64=12800(脉冲数/r)   
      *则一定转速下的脉冲频率为: Frequency=(SetSpeed/60)*12800 (脉冲数/s) 
      *  得:TIM_Period=72000000/Frequency = 72000000*60/(12800*SetSpeed)=337500/SetSpeed
     ******************************************************/
     Per=(uint32_t)(STEPMOTOR_COEFFICIENT/Turn_speed+0.5); 
     if(Per<=65535) //转速小于5.15(r/min) 
      {  
        TIM_TimeBaseStructure.TIM_Period =Per-1;   //自动重装载值-PWM周期          
        TIM_TimeBaseStructure.TIM_Prescaler =0;    //不分频
        TIM_TimeBaseStructure.TIM_ClockDivision =TIM_CKD_DIV1;      //时钟倍频因子              
        TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // 向上计数模式
      }
      else        //转速大于5.15(r/min) 
      {  
       // Per=(uint32_t)((Per/1000.0)+0.5); 
        div = (Per >> 16);  //分频值 
        Per = Per/(div+1);
        TIM_TimeBaseStructure.TIM_Period =Per-1;    //自动重装载值-PWM周期         
        TIM_TimeBaseStructure.TIM_Prescaler = div;   //分频 (分频系数-1)
        TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;      //时钟倍频因子，输出时钟供给死区发生器等
        TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //向上计数模式
      }
      TIM_TimeBaseInit(StepMotor_Tim[Motorx], &TIM_TimeBaseStructure); 
        //配置 CHx的PWM模式  
      TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;             //设置PWM输出模式
      TIM_OCInitStructure.TIM_Pulse = Per/2;    //设置占空比
      TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;    //使能该通道输出
      TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;  //高级定时器-使能互补端输出
      TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;         //设置输出极性
      TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;      //高级定时器-设置互补端输出极性
      TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;       //高级定时器-死区后输出状态
      TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;    //高级定时器-死区后互补端输出状态
      
      switch(TIM_CHx[Motorx]) //选通PWM相应的CHx通道
      {
        case 1:
            TIM_OC1Init(StepMotor_Tim[Motorx], &TIM_OCInitStructure); //通道1使能
        break;
        case 2:
            TIM_OC2Init(StepMotor_Tim[Motorx], &TIM_OCInitStructure); //通道2使能
        break;
        case 3:
            TIM_OC3Init(StepMotor_Tim[Motorx], &TIM_OCInitStructure); //通道3使能
        break;
        case 4:
            TIM_OC4Init(StepMotor_Tim[Motorx], &TIM_OCInitStructure); //通道4使能
        break;
      }
 }
}
/*******************************************************************************
* Function Name  : StepMotor_Work_Control
* Description    : 泵工作控制主程序:速度自动升降，状态检测等
* 输入参数       : 
* Output         : None
* Return         : None
* 使用           : 在主循环程序中调用
*******************************************************************************/
void StepMotor_Work_Control(void)
{
  u8 mi,adjsta,clrsta;
  u32 adjcnt;
  
  adjcnt = StepMotor_ADJ_Cnt;
  clrsta = 0;   //是否清0调速间隔时间计数器的标志位,6个泵共用
  for(mi = 0; mi < MOTOR_TOTAL; mi++)   //轮询各个泵
  {
    adjsta = 0;
    if(StepMotor_Data[mi].SetSpeed == 0) 
      StepMotor_Data[mi].Enable = DISABLE;
    if(StepMotor_Data[mi].Enable == DISABLE) //关机命令
    {
      if(StepMotor_Data[mi].Status != DISABLE)  //此前为开机状态，则执行停机命令
      {
        StepMotor_Data[mi].Status = DISABLE;
        StepMotor_Data[mi].CurrSpeed = 0;
        StepMotor_Enable_Cmd((StepMotor_TypeDef)mi,DISABLE);
        StepMotor_TIM_Cmd((StepMotor_TypeDef)mi,DISABLE);
      }
    }
    else    //开机命令
    {
      if(StepMotor_Data[mi].Status != ENABLE) //此前为停机状态，则启动定时器等
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
      //工作期间: 设置方向,读取报警状态等
      StepMotor_Set_DIR((StepMotor_TypeDef)mi,StepMotor_Data[mi].SetDir);
      StepMotor_ALERT_Read((StepMotor_TypeDef)mi);
      StepMotor_NF_Read((StepMotor_TypeDef)mi);
      StepMotor_BOTTOM_Read((StepMotor_TypeDef)mi);
      StepMotor_TOP_Read((StepMotor_TypeDef)mi);
      StepMotor_BRAKING_Read((StepMotor_TypeDef)mi);
      
      if(StepMotor_Data[mi].SetSpeed > STEPMOTOR_MAX_SETSPEED) StepMotor_Data[mi].SetSpeed=STEPMOTOR_MAX_SETSPEED;
      //当前速度与设定速度不相等，且达到一定的时间间隔后,才递增或递减至设定转速
      if((StepMotor_Data[mi].CurrSpeed != StepMotor_Data[mi].SetSpeed)&&(adjcnt > STEPMOTOR_ADJSPEED_SPACE))   
      { 
        if(StepMotor_Data[mi].CurrSpeed < StepMotor_Data[mi].SetSpeed) //升速
        {
          adjsta = 1;
           StepMotor_Data[mi].CurrSpeed += STEPMOTOR_ADJSPEED_STEP;
           if(StepMotor_Data[mi].CurrSpeed > StepMotor_Data[mi].SetSpeed)  //递增到设定值
           {
             StepMotor_Data[mi].CurrSpeed = StepMotor_Data[mi].SetSpeed;
           }
        }
        else //降速
        {
             adjsta = 1;   
           if(StepMotor_Data[mi].CurrSpeed >= STEPMOTOR_ADJSPEED_STEP) 
           {
             StepMotor_Data[mi].CurrSpeed -= STEPMOTOR_ADJSPEED_STEP;   //递减
           }
           else 
           {
              StepMotor_Data[mi].CurrSpeed = 0;     //递减到0
           }
           if(StepMotor_Data[mi].CurrSpeed < StepMotor_Data[mi].SetSpeed) //递减到设定值
           {
             StepMotor_Data[mi].CurrSpeed = StepMotor_Data[mi].SetSpeed; 
           }
        } 
        if(adjsta == 1)  //本组泵参数扫描中，有泵速度调节
         {
           StepMotor_Speed_ADJ((StepMotor_TypeDef)mi, StepMotor_Data[mi].CurrSpeed);
           clrsta = 1;
         }
      }
     } 
   }
   if(clrsta == 1)StepMotor_ADJ_Cnt = 0;   //6个泵中，有任意一个泵执行了速度调节，则清0计数器
}

/*******************************************************************************
* Function Name  : StepMotor_PWM_TIM_IRQ
* Description    : PWM定时器中断处理程序
* 输入参数       : Motorx - 电机编号: 枚举中的值(0~5: 对应 STEPMOTOR1~STEPMOTOR6)
* Output         : None
* Return         : None
*******************************************************************************/
void StepMotor_PWM_TIM_IRQ(StepMotor_TypeDef Motorx)   
{
  static u16 pwmpulse_real[MOTOR_TOTAL] = {0};
  u8 ENCODER[6] = {MOTOR1_ENCODER,MOTOR2_ENCODER,MOTOR3_ENCODER,MOTOR4_ENCODER,MOTOR5_ENCODER,MOTOR6_ENCODER};  
  u16 pulse_encoder;
#ifdef MOTOR_PWM_IRQ_EN   //配置PWM的TIM中断有效，主要用于流量脉冲累计
  TIM_TypeDef *StepMotor_Tim[6] = {MOTOR1_TIM,MOTOR2_TIM,MOTOR3_TIM,MOTOR4_TIM,MOTOR5_TIM,MOTOR6_TIM};   //各泵使用的TIMx

 if (TIM_GetITStatus(StepMotor_Tim[Motorx], TIM_IT_Update) != RESET)
 { 
    TIM_ClearITPendingBit(StepMotor_Tim[Motorx], TIM_IT_Update );
    StepMotor_Data[Motorx].Flow++;      //流量脉累计
#ifdef MOTOR_PWM_LOCKROTOR_CHECK_EN        //判断是否堵转
    StepMotor_Data[Motorx].FlowLockRotor++;
    pulse_encoder = STEPMOTOR_PWMPULSE / ENCODER[Motorx];  //每一个旋转编码器脉冲对应的PWM发出脉冲数 = 12800/齿数
    if(StepMotor_Data[Motorx].Flow==StepMotor_Data[Motorx].Pluse)    //JK
    {
     StepMotor_Data[Motorx].Enable =  DISABLE;
    }
    if(StepMotor_Data[Motorx].FlowLockRotor > (pulse_encoder + (pulse_encoder >> 1))) //超出每齿脉冲数的50%后判断一次是否堵转
    { //步时脉冲个数超出 标准数量的1.5倍后，判断是否有堵转
      if(StepMotor_Data[Motorx].LockRotorSpeed == 0)
      {
        StepMotor_Data[Motorx].LockRotor = 1; //堵转
      }
      else
      {
        StepMotor_Data[Motorx].LockRotor = 0; //未堵转
      }
        StepMotor_Data[Motorx].FlowLockRotor = 0;
        StepMotor_Data[Motorx].LockRotorSpeed = 0;
    }
#endif  
#ifdef MOTOR_REALSPEED_CHECK_EN        //真实转速测试
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
* Description    : 速度反饋信號的EXTI中斷處理程序
* 输入参数       : Motorx - 电机编号: 枚举中的值(0~5: 对应 STEPMOTOR1~STEPMOTOR6)
* Output         : None
* Return         : None
*******************************************************************************/
void StepMotor_EXTI_IRQ(StepMotor_TypeDef Motorx)   
{
#ifdef MOTOR_REALSPEED_CHECK_EN    
 uint32_t    EXTI_Linex[6]  =  {MOTOR1_REAL_EXTI_Line,MOTOR2_REAL_EXTI_Line,MOTOR3_REAL_EXTI_Line,MOTOR4_REAL_EXTI_Line,MOTOR5_REAL_EXTI_Line,MOTOR6_REAL_EXTI_Line};    //Pin

if(EXTI_GetITStatus(EXTI_Linex[Motorx])!=RESET)//判断某个线上的中断是否发生 
  { 
   EXTI_ClearITPendingBit(EXTI_Linex[Motorx]);  //清除 LINE 上的中断标志位    
   StepMotor_Data[Motorx].LockRotorSpeed++;     //累计实际检测到的转速脉冲个数   
   StepMotor_Data[Motorx].RealPulse++;          //累计实际检测到的转速脉冲个数   
  }   
#endif 
}



/******************* (C) COPYRIGHT 2008 SWS *****END OF FILE*******************/

