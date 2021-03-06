#ifndef __HP_H
#define __HP_H


#define HPSELF_Speed 100        //肝素泵自检时转速

#define HP_NORMAL_RUN      0    //肝素正常运行模式
#define HP_FAST_FORWARD    1    //肝素快进模式
#define HP_SELF_TEST       3    //肝素自检模式
#define HP_CORRECT         4    //肝素泵校正模式


#define HP_CORRECT_DEFAULT     0    //肝素泵校正   
#define HP_CORRECT_RUNING      1    //肝素泵正在校正
#define HP_CORRECT_PASS        2    //肝素泵校正通过
#define HP_CORRECT_ERROR       3    //肝素泵校正异常 

extern float CoeffIcient_HP;

#define Correct_Length10 282            //10ml注射器校正圈数(1毫米长度=0.2圈)
#define Correct_Length20 349            //20ml注射器校正圈数(1毫米长度=0.2圈)
#define Correct_Length30 378            //30ml注射器校正圈数(1毫米长度=0.2圈)
#define Correct_Length50 373            //50ml注射器校正圈数(1毫米长度=0.2圈)

#define Correct_Flow10  0.03348     //步进电机一圈流量 ml/r
#define Correct_Flow20  0.0555
#define Correct_Flow30  0.0831
#define Correct_Flow50  0.133

#define   Correct_Length10_B 50      //10mL情况 检测到位后需要推注10mm   
#define   Correct_Length20_B 45      //20mL情况 检测到位后需要推注9mm
#define   Correct_Length30_B 47      //30mL情况 检测到位后需要推注9.5mm
#define   Correct_Length50_B 8       //50mL情况 检测到位后需要推注1.5mm

typedef struct struct_Hp_param  
{  
  u16 flg;           //2    该数据块是否有效的标志：0x5a5a-有效
  u16 ret_1;         //4    未使用,目的是位置填充,以按字对齐
  u16 correct[4];   //12    10ml、20、30、50ml注射器校正参数    
  u16 correct_B[4]; //20    10ml、20、30、50ml注射器到底校正参数   
  u16 ret_2[21];    //62    未使用,用于补齐64字节
  u16 crc16;        //64    校检和
} HP_PARAM;
extern HP_PARAM hp_param;

typedef struct HP  
{  
  u8 Hall_State;       //霍尔状态
  u8 HP_Type;         //霍尔检测注射器型号
  u8 HP_OrderType;    //上位机下发的注射器型号
  u8 HP_Compare;       //注射器型号对比结果
  u16 HP_Turn;         //到底检测到位后还需要推注的圈数 
  float HP_Flow;
  u8 HP_Length;       //检测到位后需要推注长度
  u8 HP_Check;       //肝素泵校正开关
  u8 Up_Mode;        //肝素泵校正上行模式
} HP_Work;
extern HP_Work HP_Data;

#define  Top_HP     GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_1)   //注射器是否安装到位检测 PD1
#define  Bottom_HP  GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_2)   //肝素到底检测  PD2
#define  SyringeMode_S1  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_12) //注射器型号检测 PC12
#define  SyringeMode_S2  GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_0) //注射器型号检测 PD0
#define  SyringeMode_S3  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_11) //注射器型号检测 PC11

extern void HP_CheckInfo();
extern void Syringe_Type();
#endif 