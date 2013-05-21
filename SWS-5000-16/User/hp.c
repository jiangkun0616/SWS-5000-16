#include "stm32f10x.h"
#include "hp.h"
#include "swsbus.h"
HP_PARAM hp_param;
HP_Work HP_Data = {0};

void HP_CheckInfo()
{
  
}

void Syringe_Type()
{

  HP_Data.Hall_State = (SyringeMode_S1|(SyringeMode_S2<<1)|(SyringeMode_S3<<2));
  switch(HP_Data.Hall_State)
 {
   case 0x07:         //ע�����ͺ�10ml
       HP_Data.HP_Type=10;
       HP_Data.HP_Length=Correct_Length10_B;  //10mL��� ��⵽λ����Ҫ��ע10mm
       HP_Data.HP_Turn=Correct_Length10;     //10mlע����У��Ȧ��(1���׳���=0.2Ȧ)
       HP_Data.HP_Flow=Correct_Flow10;
       break;
   case 0x06:         //ע�����ͺ�20ml
       HP_Data.HP_Type=20;
       HP_Data.HP_Length=Correct_Length20_B;   //20mL��� ��⵽λ����Ҫ��ע9mm
       HP_Data.HP_Turn=Correct_Length20; 
       HP_Data.HP_Flow=Correct_Flow20;
       break;
   case 0x04:        //ע�����ͺ�30ml
       HP_Data.HP_Type=30;
       HP_Data.HP_Length=Correct_Length30_B;   //30mL��� ��⵽λ����Ҫ��ע9.5mm
       HP_Data.HP_Turn=Correct_Length30; 
       HP_Data.HP_Flow=Correct_Flow30;
       break;
   case 0x02:        //ע�����ͺ�50ml
       HP_Data.HP_Type=50;
       HP_Data.HP_Length=Correct_Length50_B;   //50mL��� ��⵽λ����Ҫ��ע1.5mm
       HP_Data.HP_Turn=Correct_Length50; 
       HP_Data.HP_Flow=Correct_Flow50;
       break;
   default:
       break;
  }
  if((Correct_Length10!=(BUS_Buffer.WR0[4]*0x100+BUS_Buffer.WR0[3]))||
    (Correct_Length20!=(BUS_Buffer.WR0[6]*0x100+BUS_Buffer.WR0[5]))||
    (Correct_Length30!=(BUS_Buffer.WR0[8]*0x100+BUS_Buffer.WR0[7]))||
    (Correct_Length50!=(BUS_Buffer.WR0[10]*0x100+BUS_Buffer.WR0[9])))
  {
    HP_Data.Up_Mode = HP_CORRECT_ERROR;   //���ر�У���쳣
  }

  
    

}