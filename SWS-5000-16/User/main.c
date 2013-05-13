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
#include "flag.h"
#include "hp.h"

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

int main(void)
{ 

  System_Init();	         //系统初始化
  RS485_Init();
  StepMotor_Init();
  HP_Check();                   //读取肝素泵校正信息
  
  while(1)
  {
    Syringe_Type();                  //注射器型号检测
    Down_flag();                     //解析标志位
    check_lockrotor();               //系统，电机运行指示灯
    Motor_Run_Mode();  
    StepMotor_Work_Control();
    Up_flag();                       //上行消息 
    
  }
 
}  
 
u16 CNT_15S = 0;
u8  lamp_pump_cnt = 0;
void check_lockrotor(void)
{
  if(LED_COM_Cnt < 100)  //系统运行指示灯
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
