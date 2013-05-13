/**************************** (C) COPYRIGHT 2008 SWS ***************************
* File Name          : sws_stepmotor_v2.h
* Author             : 周友兵
* Version            : V2.0
* Date               : 2013.4.7
* Description        : 步进电机接口函数头文件 - 包括配置，用户接口等
使用规则：
    1. 本接口软件最多支持6个步进电机的驱动，依次为MOTOR1,MOTOR2,MOTOR3,MOTOR4,MOTOR5,MOTOR6

*******************************************************************************/

#ifndef __SWS_STEPMOTOR_V2_H
#define __SWS_STEPMOTOR_V2_H

#ifdef __cplusplus
 extern "C" {
#endif

/**************** 选择步进电机相关检测和控制功能 (PWM,ENABLE默认有效) *******************/
#define MOTOR_DCY_EN                  //DCY功能有效
//#define MOTOR_RESET_EN                //驱动芯片复位脚有效
#define MOTOR_DIR_EN                  //驱动芯片方向控制脚有效
#define MOTOR_ALERT_EN                //驱动芯片过流过热检测有效
#define MOTOR_NF_EN                   //缺相检测有效,要求电路上把A,B两相的缺机检测信号合二为一
//#define MOTOR_TOP_CHECK_EN            //到顶检测有效
#define MOTOR_BOTTOM_CHECK_EN         //到底检测有效
//#define MOTOR_BRAKING_EN              //强制刹车(泵盖等检测)有效
#define MOTOR_REALSPEED_CHECK_EN      //马达真实转速检测有效(检测方式为霍尔或编码盘)
#define MOTOR_PWM_IRQ_EN              //配置PWM的TIM中断有效，主要用于流量脉冲累计
#define MOTOR_PWM_LOCKROTOR_CHECK_EN  //在PWM中断检测堵转有效(条件：MOTOR_REALSPEED_CHECK_EN和MOTOR_PWM_IRQ_EN必须有效)


/*************定义步进脉冲的TIM系数****************
  *已知条件有：细分度=64;步距角=1.8度;SetSpeed为转速(fload型),单位为rpm;PWM的TIM的工作时钟为72Mhz则有：
  *   每秒的转速度为：SetSpeed/60(r/s) 
  *   电机转一圈所需要的脉冲数为: (360度/1.8度)*64=200*64=12800(脉冲数/r)   
  *则一定转速下的脉冲频率为: Frequency=(SetSpeed/60)*12800 (脉冲数/s) 
  *  得:TIM_Period=72000000/Frequency = 72000000*60/(12800*SetSpeed)=337500/SetSpeed
 ******************************************************/
#define STEPMOTOR_COEFFICIENT     337500.0  //步进脉冲的TIM系数
#define STEPMOTOR_PWMPULSE        12800     //步进电机每转的PWM脉冲数 =(360度/1.8度)*64
                                            //   计数公式为: 12800/齿数

/************* 定义自动加减速的相关参数    *******************************/
#define STEPMOTOR_ADJSPEED_SPACE    15  //调速间隔时间(单位ms)
#define STEPMOTOR_ADJSPEED_STEP     2  //2rmp - 每次调速增/减的步长（转数）

#define STEPMOTOR_IN_DEBOUNCE       30    //步进电机的所有相关输入引脚去抖动次数
#define STEPMOTOR_NF_CHECK_10S      1000  //缺相检测周期10s: 0.01s*1000
#define STEPMOTOR_MAX_SETSPEED      200.0 //转速限制，A,B泵最大250rpm

/************* 定义相关工作参数的结构体    *******************************/
typedef struct
{
    u8  Enable;     //电机启/停控制命令： DISABLE=关闭电机；ENABLE=使能电机工作
    u8  Status;     //电机的当前工作状态：DISABLE=停机状态；ENABLE=工作状态
    u8  SetDir;     //设定转动方向:1=反转;0=正转
    u8  OldDir;     //旧转动方向
    float SetSpeed;   //设定转速(rpm)
    float CurrSpeed;  //当前转速(rpm)
    u8  ToBottom;    //1=到底;0=未到底
    u8  ToTop;       //1=到顶;0=未到顶
    u8  Alert;       //过热、过流：1=报警,0=正常
    u8  NF;          //缺相：1=报警,0=正常
    u8  Braking;     //强制制动(泵盖等)引脚的状态：1=制动有效;0=不制动,正常运行
    u8  RunSta_Brak; //暂存强制制动(泵盖等)前泵的工作状态,待取消强制制动后,自动恢复运行
    u8  LockRotor;   //堵转：1=堵转报警,0=正常
    u32 Flow;        //流量累计(PWM脉冲(中断)累计)
    u32 Pluse;       //脉冲个数，jk
    u32 FlowLockRotor;   //堵转检测用的PWM脉冲计数
    u32 LockRotorSpeed;  //堵转检测用的实际转速检测计数，即编码盘或HALL信号
    float RealSpeed;       //真实转速(rpm) - 通过转速检测信号实测得到的转速
    u32   RealPulse;       //真实转速脉冲累计，用于计算真实转速，每分钟清0一次
    u8  RatioSwitch;      //泵管系数测试开关
    u8  Down_Mode;        //下行运转模式(00正常,01管路安装,10泵管系数测试，11自检模式)
    u8  Up_Mode;
    u16  Ratio;            //泵管系数ml/R
    u16  SetFlow;          //设定流量(0~100ml/h*10)放大10倍
}StepMotor_WorkInfo_Def;

/**************** 系统中使用的步进电机泵的枚举定义 - 根据实际个数修改，最多为6个 *******************/
 //注意：其初始值必须与STEPMOTOR1的值相同，这样才能依次与STEPMOTOR1~STEPMOTOR6对应。
#define MOTOR_TOTAL   5   //系统中实际使用的电机总个数 [必须一致]
typedef enum   
{
  UFP = 0,   //  - 为MOTOR1
  BPP,       //  - 为MOTOR2
  SBP,       //    - 为MOTOR3
  SFP,       //   - 为MOTOR4
  HP,        //   - 为MOTOR5
}StepMotor_TypeDef;

/**************** 旋转编码器的齿数,即每转的脉冲数  --可修改, 未使用的定义为1 **********/
  
#define  MOTOR1_ENCODER     1    // 
#define  MOTOR2_ENCODER     1    //
#define  MOTOR3_ENCODER     1    // 
#define  MOTOR4_ENCODER     1    // 
#define  MOTOR5_ENCODER     16    // 
#define  MOTOR6_ENCODER     1     //


/**************** 泵PWM使用的时钟及通道号宏定义  --可修改, 未使用的定义为0 **********/
  //时钟为定时器代号: TIM1,TIM2,TIM3,...,TIM8等. 未使用的泵的值与最后一个泵相同
#define  MOTOR1_TIM         TIM5    // TIM5_CH2
#define  MOTOR2_TIM         TIM8    // TIM8_CH1N
#define  MOTOR3_TIM         TIM2    // TIM2-CH3 (重定义)
#define  MOTOR4_TIM         TIM4    // TIM4-CH3 (重定义)
#define  MOTOR5_TIM         TIM3    // TIM3-CH4 (重定义)
#define  MOTOR6_TIM         0       //
  //通道号为数字: 1,2,3,4。未使用的泵的值与最后一个泵相同
#define  MOTOR1_CH          2   //
#define  MOTOR2_CH          1   //
#define  MOTOR3_CH          3   //
#define  MOTOR4_CH          3   //
#define  MOTOR5_CH          4   //
#define  MOTOR6_CH          0   //
//  //TIM中断总开关使能后，可以单独选择某个泵的是否产生中断： 1=允许产生中断;0=禁止产生中断
//#ifdef MOTOR_PWM_IRQ_EN   
//#define  MOTOR1_TIM_IRQ         1    //
//#define  MOTOR2_TIM_IRQ         1    //
//#define  MOTOR3_TIM_IRQ         1    //
//#define  MOTOR4_TIM_IRQ         1    //
//#define  MOTOR5_TIM_IRQ         1    //
//#define  MOTOR6_TIM_IRQ         0    //
//#endif
  // PWM引脚及端口设置. 未使用的泵的值与最后一个泵相同
#define  MOTOR1_PWM_PORT    GPIOA   //PA1
#define  MOTOR2_PWM_PORT    GPIOA   //PA7
#define  MOTOR3_PWM_PORT    GPIOB   //PB10
#define  MOTOR4_PWM_PORT    GPIOD   //PD14
#define  MOTOR5_PWM_PORT    GPIOC   //PC9
#define  MOTOR6_PWM_PORT    0       //-
#define  MOTOR1_PWM_PIN     GPIO_Pin_1
#define  MOTOR2_PWM_PIN     GPIO_Pin_7
#define  MOTOR3_PWM_PIN     GPIO_Pin_10
#define  MOTOR4_PWM_PIN     GPIO_Pin_14
#define  MOTOR5_PWM_PIN     GPIO_Pin_9
#define  MOTOR6_PWM_PIN     0

/**************** ENABLE引脚 输出GPIO  --可修改, 未使用的定义为0 **********/
//THB6064AH使能控制引脚,必须使用
#define  MOTOR_ENABLE_INVERTER    1    //1=电路有反相器，电平需反相;0=无反相
#define  MOTOR1_ENABLE_PORT    GPIOB   //PB0
#define  MOTOR2_ENABLE_PORT    GPIOE   //PE9
#define  MOTOR3_ENABLE_PORT    GPIOB   //PB11
#define  MOTOR4_ENABLE_PORT    GPIOB   //PB12
#define  MOTOR5_ENABLE_PORT    GPIOD   //PD11
#define  MOTOR6_ENABLE_PORT    0       //-
#define  MOTOR1_ENABLE_PIN     GPIO_Pin_0
#define  MOTOR2_ENABLE_PIN     GPIO_Pin_9
#define  MOTOR3_ENABLE_PIN     GPIO_Pin_11
#define  MOTOR4_ENABLE_PIN     GPIO_Pin_12
#define  MOTOR5_ENABLE_PIN     GPIO_Pin_11
#define  MOTOR6_ENABLE_PIN     0

/**************** RESET引脚 输出GPIO  --可修改, 未使用的定义为0**********/
#ifdef MOTOR_RESET_EN //驱动芯片复位脚有效
#define  MOTOR_RESET_INVERTER    1    //1=电路有反相器，电平需反相;0=无反相
#define  MOTOR1_RESET_PORT    GPIOB   //PB0
#define  MOTOR2_RESET_PORT    GPIOE   //PE9
#define  MOTOR3_RESET_PORT    GPIOB   //PB10
#define  MOTOR4_RESET_PORT    GPIOB   //PB12
#define  MOTOR5_RESET_PORT    GPIOD   //PD11
#define  MOTOR6_RESET_PORT    0       //
#define  MOTOR1_RESET_PIN     GPIO_Pin_0
#define  MOTOR2_RESET_PIN     GPIO_Pin_9
#define  MOTOR3_RESET_PIN     GPIO_Pin_10
#define  MOTOR4_RESET_PIN     GPIO_Pin_12
#define  MOTOR5_RESET_PIN     GPIO_Pin_11
#define  MOTOR6_RESET_PIN     0
#endif

/***************** DCY衰减设定输出 -可根据电机工作电流大小适当修改  --可修改, 未使用的定义为0***************/
#ifdef MOTOR_DCY_EN
#define DCY_VAL1  75     //42电机小于85RPM  20%快衰减
#define DCY_VAL2  200    //42电机小于200RPM 40%快衰减
#define DCY_VAL3  400    //42电机小于400RPM 60%快衰减
                          //  超过DCY_VAL3为 80%快衰减
 //DCY1
#define  MOTOR_DCY_INVERTER    1    //1=电路有反相器，电平需反相;0=无反相
#define  MOTOR1_DCY1_PORT    GPIOB   //PB1
#define  MOTOR2_DCY1_PORT    GPIOE   //PE10
#define  MOTOR3_DCY1_PORT    GPIOE   //PE14
#define  MOTOR4_DCY1_PORT    GPIOB   //PB13
#define  MOTOR5_DCY1_PORT    GPIOD   //PD8
#define  MOTOR6_DCY1_PORT    0   //-
#define  MOTOR1_DCY1_PIN     GPIO_Pin_1
#define  MOTOR2_DCY1_PIN     GPIO_Pin_10
#define  MOTOR3_DCY1_PIN     GPIO_Pin_14
#define  MOTOR4_DCY1_PIN     GPIO_Pin_13
#define  MOTOR5_DCY1_PIN     GPIO_Pin_8
#define  MOTOR6_DCY1_PIN     0
  //DCY2
#define  MOTOR1_DCY2_PORT    GPIOB   //PB2
#define  MOTOR2_DCY2_PORT    GPIOE   //PE11
#define  MOTOR3_DCY2_PORT    GPIOE   //PE13
#define  MOTOR4_DCY2_PORT    GPIOB   //PB14
#define  MOTOR5_DCY2_PORT    GPIOD   //PD9
#define  MOTOR6_DCY2_PORT    0       //-
#define  MOTOR1_DCY2_PIN     GPIO_Pin_2
#define  MOTOR2_DCY2_PIN     GPIO_Pin_11
#define  MOTOR3_DCY2_PIN     GPIO_Pin_13
#define  MOTOR4_DCY2_PIN     GPIO_Pin_14
#define  MOTOR5_DCY2_PIN     GPIO_Pin_9
#define  MOTOR6_DCY2_PIN     0
#endif
/**************** CW/CCW方向控制引脚 输出GPIO  --可修改, 未使用的定义为0  **********/
#ifdef MOTOR_DIR_EN //驱动芯片方向控制脚有效
#define  MOTOR_DIR_INVERTER    1    //1=电路有反相器，电平需反相;0=无反相
#define  MOTOR1_DIR_PORT    0   //-
#define  MOTOR2_DIR_PORT    0   //-
#define  MOTOR3_DIR_PORT    0   //-
#define  MOTOR4_DIR_PORT    0   //-
#define  MOTOR5_DIR_PORT    GPIOC   //PC7
#define  MOTOR6_DIR_PORT    0   //-
#define  MOTOR1_DIR_PIN     0
#define  MOTOR2_DIR_PIN     0
#define  MOTOR3_DIR_PIN     0
#define  MOTOR4_DIR_PIN     0
#define  MOTOR5_DIR_PIN     GPIO_Pin_7
#define  MOTOR6_DIR_PIN     0
#endif

/**************** ALERT引脚 输入GPIO  --可修改, 未使用的定义为0  **********/
//THB6064AH过流过热报警输出
#ifdef MOTOR_ALERT_EN
#define  MOTOR_ALERT_INVERTER    1    //1=电路有反相器，电平需反相;0=无反相
#define  MOTOR1_ALERT_PORT    GPIOE   //PE7
#define  MOTOR2_ALERT_PORT    GPIOE   //PE12
#define  MOTOR3_ALERT_PORT    GPIOE   //PE15
#define  MOTOR4_ALERT_PORT    GPIOB   //PB15
#define  MOTOR5_ALERT_PORT    GPIOD   //PD10
#define  MOTOR6_ALERT_PORT    0   //-
#define  MOTOR1_ALERT_PIN     GPIO_Pin_7
#define  MOTOR2_ALERT_PIN     GPIO_Pin_12
#define  MOTOR3_ALERT_PIN     GPIO_Pin_15
#define  MOTOR4_ALERT_PIN     GPIO_Pin_15
#define  MOTOR5_ALERT_PIN     GPIO_Pin_10
#define  MOTOR6_ALERT_PIN     0
#endif
/**************** NF(缺相)引脚 输入GPIO  --可修改, 未使用的定义为0  **********/
//缺相检测:1.当A,B两相的信号由外部组合为一个信号NF时，使用NFA的定义
#ifdef MOTOR_NF_EN   //A相缺相检测有效
#define  MOTOR_NF_INVERTER    0    //1=GPIO端为高电平报警;0=GPIO端为低电平报警
#define  MOTOR1_NF_PORT    GPIOE   //PE8
#define  MOTOR2_NF_PORT    GPIOD   //PD12
#define  MOTOR3_NF_PORT    GPIOC   //PC8
#define  MOTOR4_NF_PORT    GPIOC   //PC6
#define  MOTOR5_NF_PORT    GPIOB   //PB6
#define  MOTOR6_NF_PORT    0   //
#define  MOTOR1_NF_PIN     GPIO_Pin_8
#define  MOTOR2_NF_PIN     GPIO_Pin_12
#define  MOTOR3_NF_PIN     GPIO_Pin_8
#define  MOTOR4_NF_PIN     GPIO_Pin_6
#define  MOTOR5_NF_PIN     GPIO_Pin_6
#define  MOTOR6_NF_PIN     0
#endif

/**************** 到顶到底检测引脚定义 输入GPIO  --可修改, 未使用的定义为0  **********/
#ifdef MOTOR_TOP_CHECK_EN
  //到顶
#define  MOTOR_TOP_INVERTER    1    //1=GPIO端为高电平到位;0=GPIO端为低电平到位
#define  MOTOR1_TOP_PORT    0   //
#define  MOTOR2_TOP_PORT    0   //
#define  MOTOR3_TOP_PORT    0   //
#define  MOTOR4_TOP_PORT    0   //
#define  MOTOR5_TOP_PORT    0   //
#define  MOTOR6_TOP_PORT    0   //
#define  MOTOR1_TOP_PIN     0
#define  MOTOR2_TOP_PIN     0
#define  MOTOR3_TOP_PIN     0
#define  MOTOR4_TOP_PIN     0
#define  MOTOR5_TOP_PIN     0
#define  MOTOR6_TOP_PIN     0
#endif 

#ifdef MOTOR_BOTTOM_CHECK_EN
  //到底
#define  MOTOR_BOTTOM_INVERTER    1    //1=GPIO端为高电平到位;0=GPIO端为低电平到位
#define  MOTOR1_BOTTOM_PORT    0   //
#define  MOTOR2_BOTTOM_PORT    0   //
#define  MOTOR3_BOTTOM_PORT    0   //
#define  MOTOR4_BOTTOM_PORT    0   //
#define  MOTOR5_BOTTOM_PORT    GPIOD   //PD2
#define  MOTOR6_BOTTOM_PORT    0   //
#define  MOTOR1_BOTTOM_PIN     0
#define  MOTOR2_BOTTOM_PIN     0
#define  MOTOR3_BOTTOM_PIN     0
#define  MOTOR4_BOTTOM_PIN     0
#define  MOTOR5_BOTTOM_PIN     GPIO_Pin_2
#define  MOTOR6_BOTTOM_PIN     0
#endif 
/**************** 强制制动(泵盖等)检测引脚定义 输入GPIO  --可修改, 未使用的定义为0  **********/
#ifdef MOTOR_BRAKING_EN   //强制刹车功能有效
#define  MOTOR_BRAKING_INVERTER    1    //1=GPIO端为高电平强制刹车;0=GPIO端为低电平强制刹车
#define  MOTOR1_BRAKING_PORT    GPIOA   //PA5
#define  MOTOR2_BRAKING_PORT    GPIOA   //PA6
#define  MOTOR3_BRAKING_PORT    GPIOC   //PC4
#define  MOTOR4_BRAKING_PORT    GPIOC   //PC5
#define  MOTOR5_BRAKING_PORT    0   //-
#define  MOTOR6_BRAKING_PORT    0   //-
#define  MOTOR1_BRAKING_PIN     GPIO_Pin_5
#define  MOTOR2_BRAKING_PIN     GPIO_Pin_6
#define  MOTOR3_BRAKING_PIN     GPIO_Pin_4
#define  MOTOR4_BRAKING_PIN     GPIO_Pin_5
#define  MOTOR5_BRAKING_PIN     0
#define  MOTOR6_BRAKING_PIN     0
#endif 

/**************** 真实转速检测引脚 输入GPIO中断  --可修改, 未使用的定义为0  **********/
//原为低有效(报警)，经反相器后为高有效
#ifdef MOTOR_REALSPEED_CHECK_EN
//EXTI触发边沿定义:  EXTI_Trigger_Rising -上升沿;
                  // EXTI_Trigger_Falling -下降沿;
                  // EXTI_Trigger_Rising_Falling  - 上升沿和下降沿
#define  MOTOR_REAL_EXTI_TRIGGER    EXTI_Trigger_Falling  
//EXTI-GPIO定义
#define  MOTOR1_REAL_PORT    GPIOA   //PA9
#define  MOTOR2_REAL_PORT    GPIOA   //PA10
#define  MOTOR3_REAL_PORT    GPIOA   //PA8
#define  MOTOR4_REAL_PORT    GPIOA   //PA11
#define  MOTOR5_REAL_PORT    GPIOA   //PA12
#define  MOTOR6_REAL_PORT    0   //-
#define  MOTOR1_REAL_PIN     GPIO_Pin_9
#define  MOTOR2_REAL_PIN     GPIO_Pin_10
#define  MOTOR3_REAL_PIN     GPIO_Pin_8
#define  MOTOR4_REAL_PIN     GPIO_Pin_11
#define  MOTOR5_REAL_PIN     GPIO_Pin_12
#define  MOTOR6_REAL_PIN     0
//EXTI-外部中断定义
//  中断源端口与引脚端口对应,如GPIOA的是GPIO_PortSourceGPIOA;
//  中断源管脚与GPIO_Pin_x对应，如GPIO_Pin_9的是GPIO_PinSource9
//  中断线编号与GPIO_Pin_x对应，如GPIO_Pin_9的是EXTI_Line9   *****为0,则表示EXTI不使用 *****
#define  MOTOR1_REAL_SourcePORT   GPIO_PortSourceGPIOA    //EXTI中断源端口
#define  MOTOR1_REAL_SourcePIN    GPIO_PinSource9          //EXTI中断源管脚
#define  MOTOR1_REAL_EXTI_Line    EXTI_Line9               //EXTI中断线编号
#define  MOTOR2_REAL_SourcePORT   GPIO_PortSourceGPIOA    //EXTI中断源端口
#define  MOTOR2_REAL_SourcePIN    GPIO_PinSource10          //EXTI中断源管脚
#define  MOTOR2_REAL_EXTI_Line    EXTI_Line10               //EXTI中断线编号
#define  MOTOR3_REAL_SourcePORT   GPIO_PortSourceGPIOA    //EXTI中断源端口
#define  MOTOR3_REAL_SourcePIN    GPIO_PinSource8          //EXTI中断源管脚
#define  MOTOR3_REAL_EXTI_Line    EXTI_Line8               //EXTI中断线编号
#define  MOTOR4_REAL_SourcePORT   GPIO_PortSourceGPIOA    //EXTI中断源端口
#define  MOTOR4_REAL_SourcePIN    GPIO_PinSource11          //EXTI中断源管脚
#define  MOTOR4_REAL_EXTI_Line    EXTI_Line11               //EXTI中断线编号
#define  MOTOR5_REAL_SourcePORT   GPIO_PortSourceGPIOA    //EXTI中断源端口
#define  MOTOR5_REAL_SourcePIN    GPIO_PinSource12          //EXTI中断源管脚
#define  MOTOR5_REAL_EXTI_Line    EXTI_Line12               //EXTI中断线编号
#define  MOTOR6_REAL_SourcePORT   GPIO_PortSourceGPIOA    //EXTI中断源端口
#define  MOTOR6_REAL_SourcePIN    GPIO_PinSource0          //EXTI中断源管脚
#define  MOTOR6_REAL_EXTI_Line    0               //EXTI中断线编号
#endif

/* Private function prototypes -----------------------------------------------*/
//sws_stepmotor_v2.0.c内部使用的函数，外部不需要调用
void StepMotor_RCC_Configuration(void);
void StepMotor_PWM_Pin_Configuration(StepMotor_TypeDef Motorx);
void StepMotor_DIR_Pin_Configuration(StepMotor_TypeDef Motorx);
void StepMotor_ENABLE_Pin_Configuration(StepMotor_TypeDef Motorx);
void StepMotor_RESET_Pin_Configuration(StepMotor_TypeDef Motorx);
void StepMotor_DCY1_Pin_Configuration(StepMotor_TypeDef Motorx);
void StepMotor_DCY2_Pin_Configuration(StepMotor_TypeDef Motorx);
void StepMotor_ALERT_Pin_Configuration(StepMotor_TypeDef Motorx);
void StepMotor_NF_Pin_Configuration(StepMotor_TypeDef Motorx);
void StepMotor_TOP_Pin_Configuration(StepMotor_TypeDef Motorx);
void StepMotor_BOTTOM_Pin_Configuration(StepMotor_TypeDef Motorx);
void StepMotor_BRAKING_Pin_Configuration(StepMotor_TypeDef Motorx);
void StepMotor_REAL_Pin_EXTI_Configuration(StepMotor_TypeDef Motorx);

void StepMotor_Speed_ADJ(StepMotor_TypeDef Motorx,float Turn_speed);
void StepMotor_Set_DCY(StepMotor_TypeDef Motorx,u16 Turn_speed);
void StepMotor_Enable_Cmd(StepMotor_TypeDef Motorx,FunctionalState NewState);
void StepMotor_TIM_Cmd(StepMotor_TypeDef Motorx,FunctionalState NewState);
void StepMotor_ALERT_Read(StepMotor_TypeDef Motorx);
void StepMotor_NF_Read(StepMotor_TypeDef Motorx);
void StepMotor_BRAKING_Read(StepMotor_TypeDef Motorx);

//供用户其它子程序调用的函数及变量等
void StepMotor_Init(void);
void StepMotor_Work_Control(void);
void StepMotor_PWM_TIM_IRQ(StepMotor_TypeDef Motorx);
void StepMotor_EXTI_IRQ(StepMotor_TypeDef Motorx);

extern u32 StepMotor_ADJ_Cnt;
extern StepMotor_WorkInfo_Def StepMotor_Data[];
extern u16 NF_Cnt[MOTOR_TOTAL];

#ifdef __cplusplus
}
#endif

#endif
/******************* (C) COPYRIGHT 2008 SWS *****END OF FILE*******************/


