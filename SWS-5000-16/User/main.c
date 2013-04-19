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



/***************************************************************************
����ԭ�ͣ�void Motor_OFF(u8 Num)
�������ܣ�����ر�
��ڲ�����������
���ڲ�������
***************************************************************************/
void Motor_OFF(u8 Channel)
{
   if(Channel=='A')
  { 
    //LED_AP_OFF;
     StepMotor_Data[UFP].Enable = 0;
     //StepMotor_Data[UFP].SetSpeed = 0;

   // Motor_A_OFF;
   // TIM_Cmd(TIM3,DISABLE);
    //Delay(10);
    
  }
  else if(Channel=='B')
  { 
    //LED_BP_OFF;
    StepMotor_Data[HP].Enable = 0;
   // StepMotor_Data[UFP].SetSpeed = 0;

   // Motor_B_OFF;
   // Delay(10);
   // TIM_Cmd(TIM4,DISABLE);
   // Delay(10);
  }
}

/***************************************************************************
����ԭ�ͣ�void Speed_A_ADJ(u16 RPM)
���ܣ�	  A��ת�ٵ���
��ڲ�����ת��(10����
���ڲ�������
***************************************************************************/
void Speed_A_ADJ(u16 RPM)
{ 
 StepMotor_Data[UFP].SetSpeed = (float)RPM/10.0;
 
}

/***************************************************************************
����ԭ�ͣ�void Speed_B_ADJ(u16 RPM)
���ܣ�	  B��ת�ٵ���
��ڲ�����ת��X10
���ڲ�������
***************************************************************************/
void Speed_B_ADJ(u16 RPM)
{ 
 StepMotor_Data[HP].SetSpeed = (float)RPM/10.0;

}



////////////////////////////������//////////////////////////////
int main(void)
{ 

  System_Init();	         //ϵͳ��ʼ��
  RS485_Init();
  StepMotor_Init();
  while(1)
  {
    check_lockrotor();
    StepMotor_Work_Control();
  }
 
}  
 
u16 CNT_15S = 0;
u8  lamp_pump_cnt = 0;
void check_lockrotor(void)
{
  if(LED_COM_Cnt > 2)  //ϵͳ����ָʾ��
  { 
    LED_RUN_OFF; 
  }
  else
  {
   LED_RUN_ON; 
  }
  
//  //AP,BP����ָʾ����˸: AP,BP����ʱ��AP�ƣ�BP�Ʒֱ������˸;��AP,BP������ʱ��AP�Ƽ��1.2S��һ�Σ���ʾ���� 
//  if(lamp_pump_cnt > 16)
//  {
//
//    if(StepMotor_Data[UFP].Enable != DISABLE)
//    {
//      LED_AP_ON;     lamp_pump_cnt = 0;
//    } 
//    else
//    {
//      if(lamp_pump_cnt > 128) 
//      {LED_AP_ON;     lamp_pump_cnt = 0;}
//    }
//    if(StepMotor_Data[HP].Enable != DISABLE)
//    {
//      LED_BP_ON;     lamp_pump_cnt = 0;
//    } 
//  }
//  else
//  {
//    if(lamp_pump_cnt > 3)
//    {
//      LED_AP_OFF;
//      LED_BP_OFF;
//    }
//  }
  
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
