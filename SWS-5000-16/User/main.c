/**************************************************************************
---------------����ɽ��ɽ�Ƽ����޹�˾-----------
�ļ���:Filter
ģ����:�Ľ���SWS4000��Һ��
����:����
����:2009.2.10
ԭʼ�汾 Ver4.0
��ǰ�汾:Ver4.1
�޶���¼:
Ver4.1   LXD     2009.4.23 SPIͨ����������͸��Һ����
Ver4.21  LYM/LXD 2010.3.20 ���Ӵ�����͸������
                 1.��������B�絼ֵ��B��������Ϊ0ʱ�ж�Ϊ������ģʽ��
                 2.������͸��ģʽB��ֹͣ����
                    
****************************************************************************/

#include "stm32f10x.h"
#include "main.h"
#include "swsbus.h"
#include "sws_stepmotor_v2.h"
#include "flag.h"
#include "hp.h"

/*-----------------ȫ�ֱ���------------------*/
u16   LED_COM_Cnt = 0;


/**************************************************************************
����ԭ�ͣ�void delay(u16 uS)
���ܣ�	  ��ʱ
��ڲ�������
���ڲ�������
***************************************************************************/
void Delay(u16 uS)
{
  u8 i;
  while(uS--)
  {
    i=7;
    while(i--);
  }
}

int main(void)
{ 

  System_Init();	         //ϵͳ��ʼ��
  RS485_Init();
  StepMotor_Init();
  HP_Check();                   //��ȡ���ر�У����Ϣ
  
  while(1)
  {
    Syringe_Type();                  //ע�����ͺż��
    Down_flag();                     //������־λ
    check_lockrotor();               //ϵͳ���������ָʾ��
    Motor_Run_Mode();  
    StepMotor_Work_Control();
    Up_flag();                       //������Ϣ 
    
  }
 
}  
 
u16 CNT_15S = 0;
u8  lamp_pump_cnt = 0;
void check_lockrotor(void)
{
  if(LED_COM_Cnt < 100)  //ϵͳ����ָʾ��
  { 
    LED_RUN_OFF; 
    LED_485_OFF;
    
  }
  else if(LED_COM_Cnt < 200)
  {
   LED_RUN_ON; 
   
  }
  else if(200 <LED_COM_Cnt)
  {
    LED_COM_Cnt = 0;
  }
  
  
//   //������ת��ʱ(�ǼӼ���)��ÿ15S����һ��HALLת�٣����Ҽ���Ƿ��ת
//    if(StepMotor_Data[UFP].SetSpeed==StepMotor_Data[UFP].CurrSpeed) //�ǼӼ���ʱ��15s����һ��hallת��
//    {
//      if(CNT_15S >= 15)
//      { 
//        Run_Par_A.Fack_RPM = StepMotor_Data[UFP].LockRotorSpeed;  //ÿת4������: 60s/15/4=1
//         if((StepMotor_Data[UFP].LockRotorSpeed > ((u16)StepMotor_Data[UFP].SetSpeed+5)) ||((StepMotor_Data[UFP].LockRotorSpeed +5 )< (u16)StepMotor_Data[UFP].SetSpeed))
//          StepMotor_Data[UFP].LockRotor = 1;  //��ת
//        else
//          StepMotor_Data[UFP].LockRotor = 0;
//
//        StepMotor_Data[UFP].LockRotorSpeed = 0;
//      }
//    }
//    else  //�Ӽ��ٹ����У�ÿ�����һ��
//    {
//      StepMotor_Data[UFP].LockRotorSpeed = 0;
//      CNT_15S = 0;
//    }
//    if(StepMotor_Data[HP].SetSpeed==StepMotor_Data[HP].CurrSpeed) //�ǼӼ���ʱ��15s����һ��ת��
//    {
//      if(CNT_15S >= 15)
//      { 
//        Run_Par_B.Fack_RPM = StepMotor_Data[HP].LockRotorSpeed;  //ÿת4������: 60s/15/4=1
//        if((StepMotor_Data[HP].LockRotorSpeed > ((u16)StepMotor_Data[HP].SetSpeed+5)) ||((StepMotor_Data[HP].LockRotorSpeed +5 )< (u16)StepMotor_Data[HP].SetSpeed))
//          StepMotor_Data[HP].LockRotor = 1;  //��ת
//        else
//          StepMotor_Data[HP].LockRotor = 0;
//        
//        CNT_15S = 0;
//        StepMotor_Data[HP].LockRotorSpeed = 0;
//      }
//    }
//    else  //�Ӽ��ٹ����У�ÿ�����һ��
//    {
//      StepMotor_Data[HP].LockRotorSpeed = 0;
//      CNT_15S = 0;
//    }   
}
