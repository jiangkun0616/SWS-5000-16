#include "stm32f10x.h"
#include "hp.h"
#include "swsbus.h"
HP_PARAM hp_param;
HP_Work HP_Data;

u16 Correct_Length10=282;            //10ml注射器校正圈数(1毫米长度=0.2圈)
u16 Correct_Length20=349;            //20ml注射器校正圈数(1毫米长度=0.2圈)
u16 Correct_Length30=378;            //30ml注射器校正圈数(1毫米长度=0.2圈)
u16 Correct_Length50=373;            //50ml注射器校正圈数(1毫米长度=0.2圈)

u16   Correct_Length10_B=50;      //10mL情况 检测到位后需要推注10mm
u16   Correct_Length20_B=45;      //20mL情况 检测到位后需要推注9mm
u16   Correct_Length30_B=47;      //30mL情况 检测到位后需要推注9.5mm
u16   Correct_Length50_B=8;       //50mL情况 检测到位后需要推注1.5mm

float CoeffIcient_HP = 80.5369;   //肝素泵系数   ml/r

void HP_Check()
{
  
}

void Syringe_Type()
{

  HP_Data.Hall_State = SyringeMode_S1|(SyringeMode_S2<<1)|(SyringeMode_S3<<2);
  switch(HP_Data.Hall_State)
 {
   case 0x07:         //注射器型号10ml
       HP_Data.HP_Type=10;
       HP_Data.HP_Length=Correct_Length10_B;  //10mL情况 检测到位后需要推注10mm
       HP_Data.HP_Turn=Correct_Length10;     //10ml注射器校正圈数(1毫米长度=0.2圈)
       break;
   case 0x06:         //注射器型号20ml
       HP_Data.HP_Type=20;
       HP_Data.HP_Length=Correct_Length20_B;   //20mL情况 检测到位后需要推注9mm
       HP_Data.HP_Turn=Correct_Length20; 
       break;
   case 0x04:        //注射器型号30ml
       HP_Data.HP_Type=30;
       HP_Data.HP_Length=Correct_Length30_B;   //30mL情况 检测到位后需要推注9.5mm
       HP_Data.HP_Turn=Correct_Length30; 
       break;
   case 0x02:        //注射器型号50ml
       HP_Data.HP_Type=50;
       HP_Data.HP_Length=Correct_Length50_B;   //50mL情况 检测到位后需要推注1.5mm
       HP_Data.HP_Turn=Correct_Length50; 
       break;
   default:
       HP_Data.HP_Type=0;
       HP_Data.HP_Turn=0;
       break;
  }
  if((Correct_Length10!=(BUS_Buffer.WR0[4]*0x100+BUS_Buffer.WR0[3]))||
    (Correct_Length20!=(BUS_Buffer.WR0[6]*0x100+BUS_Buffer.WR0[5]))||
    (Correct_Length30!=(BUS_Buffer.WR0[8]*0x100+BUS_Buffer.WR0[7]))||
    (Correct_Length50!=(BUS_Buffer.WR0[10]*0x100+BUS_Buffer.WR0[9])))
  {
    HP_Data.Up_Mode = HP_CORRECT_ERROR;   //肝素泵校正异常
  }
  CoeffIcient_HP =  HP_Data.HP_Type*600/HP_Data.HP_Turn; //肝素泵系数计算
  
    

}