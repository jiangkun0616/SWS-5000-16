/**************************** (C) COPYRIGHT 2008 SWS ***************************
* File Name          : sws_stepmotor_v2.h
* Author             : ���ѱ�
* Version            : V2.0
* Date               : 2013.4.7
* Description        : ��������ӿں���ͷ�ļ� - �������ã��û��ӿڵ�
ʹ�ù���
    1. ���ӿ�������֧��6���������������������ΪMOTOR1,MOTOR2,MOTOR3,MOTOR4,MOTOR5,MOTOR6

*******************************************************************************/

#ifndef __SWS_STEPMOTOR_V2_H
#define __SWS_STEPMOTOR_V2_H

#ifdef __cplusplus
 extern "C" {
#endif

/**************** ѡ�񲽽������ؼ��Ϳ��ƹ��� (PWM,ENABLEĬ����Ч) *******************/
#define MOTOR_DCY_EN                  //DCY������Ч
//#define MOTOR_RESET_EN                //����оƬ��λ����Ч
#define MOTOR_DIR_EN                  //����оƬ������ƽ���Ч
#define MOTOR_ALERT_EN                //����оƬ�������ȼ����Ч
#define MOTOR_NF_EN                   //ȱ������Ч,Ҫ���·�ϰ�A,B�����ȱ������źź϶�Ϊһ
//#define MOTOR_TOP_CHECK_EN            //���������Ч
#define MOTOR_BOTTOM_CHECK_EN         //���׼����Ч
//#define MOTOR_BRAKING_EN              //ǿ��ɲ��(�øǵȼ��)��Ч
#define MOTOR_REALSPEED_CHECK_EN      //�����ʵת�ټ����Ч(��ⷽʽΪ�����������)
#define MOTOR_PWM_IRQ_EN              //����PWM��TIM�ж���Ч����Ҫ�������������ۼ�
#define MOTOR_PWM_LOCKROTOR_CHECK_EN  //��PWM�жϼ���ת��Ч(������MOTOR_REALSPEED_CHECK_EN��MOTOR_PWM_IRQ_EN������Ч)


/*************���岽�������TIMϵ��****************
  *��֪�����У�ϸ�ֶ�=64;�����=1.8��;SetSpeedΪת��(fload��),��λΪrpm;PWM��TIM�Ĺ���ʱ��Ϊ72Mhz���У�
  *   ÿ���ת�ٶ�Ϊ��SetSpeed/60(r/s) 
  *   ���תһȦ����Ҫ��������Ϊ: (360��/1.8��)*64=200*64=12800(������/r)   
  *��һ��ת���µ�����Ƶ��Ϊ: Frequency=(SetSpeed/60)*12800 (������/s) 
  *  ��:TIM_Period=72000000/Frequency = 72000000*60/(12800*SetSpeed)=337500/SetSpeed
 ******************************************************/
#define STEPMOTOR_COEFFICIENT     337500.0  //���������TIMϵ��
#define STEPMOTOR_PWMPULSE        12800     //�������ÿת��PWM������ =(360��/1.8��)*64
                                            //   ������ʽΪ: 12800/����

/************* �����Զ��Ӽ��ٵ���ز���    *******************************/
#define STEPMOTOR_ADJSPEED_SPACE    15  //���ټ��ʱ��(��λms)
#define STEPMOTOR_ADJSPEED_STEP     2  //2rmp - ÿ�ε�����/���Ĳ�����ת����

#define STEPMOTOR_IN_DEBOUNCE       30    //������������������������ȥ��������
#define STEPMOTOR_NF_CHECK_10S      1000  //ȱ��������10s: 0.01s*1000
#define STEPMOTOR_MAX_SETSPEED      200.0 //ת�����ƣ�A,B�����250rpm

/************* ������ع��������Ľṹ��    *******************************/
typedef struct
{
    u8  Enable;     //�����/ͣ������� DISABLE=�رյ����ENABLE=ʹ�ܵ������
    u8  Status;     //����ĵ�ǰ����״̬��DISABLE=ͣ��״̬��ENABLE=����״̬
    u8  SetDir;     //�趨ת������:1=��ת;0=��ת
    u8  OldDir;     //��ת������
    float SetSpeed;   //�趨ת��(rpm)
    float CurrSpeed;  //��ǰת��(rpm)
    u8  ToBottom;    //1=����;0=δ����
    u8  ToTop;       //1=����;0=δ����
    u8  Alert;       //���ȡ�������1=����,0=����
    u8  NF;          //ȱ�ࣺ1=����,0=����
    u8  Braking;     //ǿ���ƶ�(�øǵ�)���ŵ�״̬��1=�ƶ���Ч;0=���ƶ�,��������
    u8  RunSta_Brak; //�ݴ�ǿ���ƶ�(�øǵ�)ǰ�õĹ���״̬,��ȡ��ǿ���ƶ���,�Զ��ָ�����
    u8  LockRotor;   //��ת��1=��ת����,0=����
    u32 Flow;        //�����ۼ�(PWM����(�ж�)�ۼ�)
    u32 Pluse;       //���������jk
    u32 FlowLockRotor;   //��ת����õ�PWM�������
    u32 LockRotorSpeed;  //��ת����õ�ʵ��ת�ټ��������������̻�HALL�ź�
    float RealSpeed;       //��ʵת��(rpm) - ͨ��ת�ټ���ź�ʵ��õ���ת��
    u32   RealPulse;       //��ʵת�������ۼƣ����ڼ�����ʵת�٣�ÿ������0һ��
    u8  RatioSwitch;      //�ù�ϵ�����Կ���
    u8  Down_Mode;        //������תģʽ(00����,01��·��װ,10�ù�ϵ�����ԣ�11�Լ�ģʽ)
    u8  Up_Mode;
    u16  Ratio;            //�ù�ϵ��ml/R
    u16  SetFlow;          //�趨����(0~100ml/h*10)�Ŵ�10��
}StepMotor_WorkInfo_Def;

/**************** ϵͳ��ʹ�õĲ�������õ�ö�ٶ��� - ����ʵ�ʸ����޸ģ����Ϊ6�� *******************/
 //ע�⣺���ʼֵ������STEPMOTOR1��ֵ��ͬ����������������STEPMOTOR1~STEPMOTOR6��Ӧ��
#define MOTOR_TOTAL   5   //ϵͳ��ʵ��ʹ�õĵ���ܸ��� [����һ��]
typedef enum   
{
  UFP = 0,   //  - ΪMOTOR1
  BPP,       //  - ΪMOTOR2
  SBP,       //    - ΪMOTOR3
  SFP,       //   - ΪMOTOR4
  HP,        //   - ΪMOTOR5
}StepMotor_TypeDef;

/**************** ��ת�������ĳ���,��ÿת��������  --���޸�, δʹ�õĶ���Ϊ1 **********/
  
#define  MOTOR1_ENCODER     1    // 
#define  MOTOR2_ENCODER     1    //
#define  MOTOR3_ENCODER     1    // 
#define  MOTOR4_ENCODER     1    // 
#define  MOTOR5_ENCODER     16    // 
#define  MOTOR6_ENCODER     1     //


/**************** ��PWMʹ�õ�ʱ�Ӽ�ͨ���ź궨��  --���޸�, δʹ�õĶ���Ϊ0 **********/
  //ʱ��Ϊ��ʱ������: TIM1,TIM2,TIM3,...,TIM8��. δʹ�õıõ�ֵ�����һ������ͬ
#define  MOTOR1_TIM         TIM5    // TIM5_CH2
#define  MOTOR2_TIM         TIM8    // TIM8_CH1N
#define  MOTOR3_TIM         TIM2    // TIM2-CH3 (�ض���)
#define  MOTOR4_TIM         TIM4    // TIM4-CH3 (�ض���)
#define  MOTOR5_TIM         TIM3    // TIM3-CH4 (�ض���)
#define  MOTOR6_TIM         0       //
  //ͨ����Ϊ����: 1,2,3,4��δʹ�õıõ�ֵ�����һ������ͬ
#define  MOTOR1_CH          2   //
#define  MOTOR2_CH          1   //
#define  MOTOR3_CH          3   //
#define  MOTOR4_CH          3   //
#define  MOTOR5_CH          4   //
#define  MOTOR6_CH          0   //
//  //TIM�ж��ܿ���ʹ�ܺ󣬿��Ե���ѡ��ĳ���õ��Ƿ�����жϣ� 1=��������ж�;0=��ֹ�����ж�
//#ifdef MOTOR_PWM_IRQ_EN   
//#define  MOTOR1_TIM_IRQ         1    //
//#define  MOTOR2_TIM_IRQ         1    //
//#define  MOTOR3_TIM_IRQ         1    //
//#define  MOTOR4_TIM_IRQ         1    //
//#define  MOTOR5_TIM_IRQ         1    //
//#define  MOTOR6_TIM_IRQ         0    //
//#endif
  // PWM���ż��˿�����. δʹ�õıõ�ֵ�����һ������ͬ
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

/**************** ENABLE���� ���GPIO  --���޸�, δʹ�õĶ���Ϊ0 **********/
//THB6064AHʹ�ܿ�������,����ʹ��
#define  MOTOR_ENABLE_INVERTER    1    //1=��·�з���������ƽ�跴��;0=�޷���
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

/**************** RESET���� ���GPIO  --���޸�, δʹ�õĶ���Ϊ0**********/
#ifdef MOTOR_RESET_EN //����оƬ��λ����Ч
#define  MOTOR_RESET_INVERTER    1    //1=��·�з���������ƽ�跴��;0=�޷���
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

/***************** DCY˥���趨��� -�ɸ��ݵ������������С�ʵ��޸�  --���޸�, δʹ�õĶ���Ϊ0***************/
#ifdef MOTOR_DCY_EN
#define DCY_VAL1  75     //42���С��85RPM  20%��˥��
#define DCY_VAL2  200    //42���С��200RPM 40%��˥��
#define DCY_VAL3  400    //42���С��400RPM 60%��˥��
                          //  ����DCY_VAL3Ϊ 80%��˥��
 //DCY1
#define  MOTOR_DCY_INVERTER    1    //1=��·�з���������ƽ�跴��;0=�޷���
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
/**************** CW/CCW����������� ���GPIO  --���޸�, δʹ�õĶ���Ϊ0  **********/
#ifdef MOTOR_DIR_EN //����оƬ������ƽ���Ч
#define  MOTOR_DIR_INVERTER    1    //1=��·�з���������ƽ�跴��;0=�޷���
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

/**************** ALERT���� ����GPIO  --���޸�, δʹ�õĶ���Ϊ0  **********/
//THB6064AH�������ȱ������
#ifdef MOTOR_ALERT_EN
#define  MOTOR_ALERT_INVERTER    1    //1=��·�з���������ƽ�跴��;0=�޷���
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
/**************** NF(ȱ��)���� ����GPIO  --���޸�, δʹ�õĶ���Ϊ0  **********/
//ȱ����:1.��A,B������ź����ⲿ���Ϊһ���ź�NFʱ��ʹ��NFA�Ķ���
#ifdef MOTOR_NF_EN   //A��ȱ������Ч
#define  MOTOR_NF_INVERTER    0    //1=GPIO��Ϊ�ߵ�ƽ����;0=GPIO��Ϊ�͵�ƽ����
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

/**************** �������׼�����Ŷ��� ����GPIO  --���޸�, δʹ�õĶ���Ϊ0  **********/
#ifdef MOTOR_TOP_CHECK_EN
  //����
#define  MOTOR_TOP_INVERTER    1    //1=GPIO��Ϊ�ߵ�ƽ��λ;0=GPIO��Ϊ�͵�ƽ��λ
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
  //����
#define  MOTOR_BOTTOM_INVERTER    1    //1=GPIO��Ϊ�ߵ�ƽ��λ;0=GPIO��Ϊ�͵�ƽ��λ
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
/**************** ǿ���ƶ�(�øǵ�)������Ŷ��� ����GPIO  --���޸�, δʹ�õĶ���Ϊ0  **********/
#ifdef MOTOR_BRAKING_EN   //ǿ��ɲ��������Ч
#define  MOTOR_BRAKING_INVERTER    1    //1=GPIO��Ϊ�ߵ�ƽǿ��ɲ��;0=GPIO��Ϊ�͵�ƽǿ��ɲ��
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

/**************** ��ʵת�ټ������ ����GPIO�ж�  --���޸�, δʹ�õĶ���Ϊ0  **********/
//ԭΪ����Ч(����)������������Ϊ����Ч
#ifdef MOTOR_REALSPEED_CHECK_EN
//EXTI�������ض���:  EXTI_Trigger_Rising -������;
                  // EXTI_Trigger_Falling -�½���;
                  // EXTI_Trigger_Rising_Falling  - �����غ��½���
#define  MOTOR_REAL_EXTI_TRIGGER    EXTI_Trigger_Falling  
//EXTI-GPIO����
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
//EXTI-�ⲿ�ж϶���
//  �ж�Դ�˿������Ŷ˿ڶ�Ӧ,��GPIOA����GPIO_PortSourceGPIOA;
//  �ж�Դ�ܽ���GPIO_Pin_x��Ӧ����GPIO_Pin_9����GPIO_PinSource9
//  �ж��߱����GPIO_Pin_x��Ӧ����GPIO_Pin_9����EXTI_Line9   *****Ϊ0,���ʾEXTI��ʹ�� *****
#define  MOTOR1_REAL_SourcePORT   GPIO_PortSourceGPIOA    //EXTI�ж�Դ�˿�
#define  MOTOR1_REAL_SourcePIN    GPIO_PinSource9          //EXTI�ж�Դ�ܽ�
#define  MOTOR1_REAL_EXTI_Line    EXTI_Line9               //EXTI�ж��߱��
#define  MOTOR2_REAL_SourcePORT   GPIO_PortSourceGPIOA    //EXTI�ж�Դ�˿�
#define  MOTOR2_REAL_SourcePIN    GPIO_PinSource10          //EXTI�ж�Դ�ܽ�
#define  MOTOR2_REAL_EXTI_Line    EXTI_Line10               //EXTI�ж��߱��
#define  MOTOR3_REAL_SourcePORT   GPIO_PortSourceGPIOA    //EXTI�ж�Դ�˿�
#define  MOTOR3_REAL_SourcePIN    GPIO_PinSource8          //EXTI�ж�Դ�ܽ�
#define  MOTOR3_REAL_EXTI_Line    EXTI_Line8               //EXTI�ж��߱��
#define  MOTOR4_REAL_SourcePORT   GPIO_PortSourceGPIOA    //EXTI�ж�Դ�˿�
#define  MOTOR4_REAL_SourcePIN    GPIO_PinSource11          //EXTI�ж�Դ�ܽ�
#define  MOTOR4_REAL_EXTI_Line    EXTI_Line11               //EXTI�ж��߱��
#define  MOTOR5_REAL_SourcePORT   GPIO_PortSourceGPIOA    //EXTI�ж�Դ�˿�
#define  MOTOR5_REAL_SourcePIN    GPIO_PinSource12          //EXTI�ж�Դ�ܽ�
#define  MOTOR5_REAL_EXTI_Line    EXTI_Line12               //EXTI�ж��߱��
#define  MOTOR6_REAL_SourcePORT   GPIO_PortSourceGPIOA    //EXTI�ж�Դ�˿�
#define  MOTOR6_REAL_SourcePIN    GPIO_PinSource0          //EXTI�ж�Դ�ܽ�
#define  MOTOR6_REAL_EXTI_Line    0               //EXTI�ж��߱��
#endif

/* Private function prototypes -----------------------------------------------*/
//sws_stepmotor_v2.0.c�ڲ�ʹ�õĺ������ⲿ����Ҫ����
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

//���û������ӳ�����õĺ�����������
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


