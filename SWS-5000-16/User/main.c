/**************************************************************************
---------------重庆山外山科技有限公司-----------
文件名:Filter
模块名:改进型SWS4000配液板
创建:刘旭东
日期:2009.2.10
原始版本 Ver4.0
当前版本:Ver4.1
修订记录:
Ver4.1   LXD     2009.4.23 SPI通信内容增加透析液流量
Ver4.21  LYM/LXD 2010.3.20 增加醋酸盐透析功能
                 1.主机发出B电导值与B泵流量均为0时判断为醋酸盐模式。
                 2.醋酸盐透析模式B泵停止工作
                    
****************************************************************************/

#include "stm32f10x.h"
#include "main.h"
#include "swsbus.h"
#include "sws_stepmotor_v2.h"

/*-----------------全局变量------------------*/
u16   LED_COM_Cnt = 0;


/**************************************************************************
函数原型：void delay(u16 uS)
功能：	  延时
入口参数：无
出口参数：无
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
函数原型：void Motor_OFF(u8 Num)
函数功能：电机关闭
入口参数：电机编号
出口参数：无
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
函数原型：void Speed_A_ADJ(u16 RPM)
功能：	  A泵转速调整
入口参数：转速(10倍）
出口参数：无
***************************************************************************/
void Speed_A_ADJ(u16 RPM)
{ 
 StepMotor_Data[UFP].SetSpeed = (float)RPM/10.0;
 
}

/***************************************************************************
函数原型：void Speed_B_ADJ(u16 RPM)
功能：	  B泵转速调整
入口参数：转速X10
出口参数：无
***************************************************************************/
void Speed_B_ADJ(u16 RPM)
{ 
 StepMotor_Data[HP].SetSpeed = (float)RPM/10.0;

}



////////////////////////////主函数//////////////////////////////
int main(void)
{ 

  System_Init();	         //系统初始化
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
  if(LED_COM_Cnt > 2)  //系统运行指示灯
  { 
    LED_RUN_OFF; 
  }
  else
  {
   LED_RUN_ON; 
  }
  
//  //AP,BP工作指示灯闪烁: AP,BP工作时，AP灯，BP灯分别快速闪烁;当AP,BP不工作时，AP灯间隔1.2S闪一次，表示待机 
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
  
//   //在正常转速时(非加减速)，每15S更新一次HALL转速，并且检测是否堵转
//    if(StepMotor_Data[UFP].SetSpeed==StepMotor_Data[UFP].CurrSpeed) //非加减速时，15s更新一次hall转速
//    {
//      if(CNT_15S >= 15)
//      { 
//        Run_Par_A.Fack_RPM = StepMotor_Data[UFP].LockRotorSpeed;  //每转4个脉冲: 60s/15/4=1
//         if((StepMotor_Data[UFP].LockRotorSpeed > ((u16)StepMotor_Data[UFP].SetSpeed+5)) ||((StepMotor_Data[UFP].LockRotorSpeed +5 )< (u16)StepMotor_Data[UFP].SetSpeed))
//          StepMotor_Data[UFP].LockRotor = 1;  //堵转
//        else
//          StepMotor_Data[UFP].LockRotor = 0;
//
//        StepMotor_Data[UFP].LockRotorSpeed = 0;
//      }
//    }
//    else  //加减速过程中，每秒更新一次
//    {
//      StepMotor_Data[UFP].LockRotorSpeed = 0;
//      CNT_15S = 0;
//    }
//    if(StepMotor_Data[HP].SetSpeed==StepMotor_Data[HP].CurrSpeed) //非加减速时，15s更新一次转速
//    {
//      if(CNT_15S >= 15)
//      { 
//        Run_Par_B.Fack_RPM = StepMotor_Data[HP].LockRotorSpeed;  //每转4个脉冲: 60s/15/4=1
//        if((StepMotor_Data[HP].LockRotorSpeed > ((u16)StepMotor_Data[HP].SetSpeed+5)) ||((StepMotor_Data[HP].LockRotorSpeed +5 )< (u16)StepMotor_Data[HP].SetSpeed))
//          StepMotor_Data[HP].LockRotor = 1;  //堵转
//        else
//          StepMotor_Data[HP].LockRotor = 0;
//        
//        CNT_15S = 0;
//        StepMotor_Data[HP].LockRotorSpeed = 0;
//      }
//    }
//    else  //加减速过程中，每秒更新一次
//    {
//      StepMotor_Data[HP].LockRotorSpeed = 0;
//      CNT_15S = 0;
//    }   
}
