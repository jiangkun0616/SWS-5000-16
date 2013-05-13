#ifndef __HP_H
#define __HP_H


#define HPSELF_Speed 100        //���ر��Լ�ʱת��

#define HP_NORMAL_RUN      0    //������������ģʽ
#define HP_FAST_FORWARD    1    //���ؿ��ģʽ
#define HP_SELF_TEST       3    //�����Լ�ģʽ
#define HP_CORRECT         4    //���ر�У��ģʽ


#define HP_CORRECT_DEFAULT     0    //���ر�У��   
#define HP_CORRECT_RUNING      1    //���ر�����У��
#define HP_CORRECT_PASS        2    //���ر�У��ͨ��
#define HP_CORRECT_ERROR       3    //���ر�У���쳣 

extern float CoeffIcient_HP;

typedef struct struct_Hp_param  
{  
  u16 flg;           //2    �����ݿ��Ƿ���Ч�ı�־��0x5a5a-��Ч
  u16 ret_1;         //4    δʹ��,Ŀ����λ�����,�԰��ֶ���
  u16 correct[4];   //12    10ml��20��30��50mlע����У������    
  u16 correct_B[4]; //20    10ml��20��30��50mlע��������У������   
  u16 ret_2[21];    //62    δʹ��,���ڲ���64�ֽ�
  u16 crc16;        //64    У���
} HP_PARAM;
extern HP_PARAM hp_param;

typedef struct HP  
{  
  u8 Hall_State;       //����״̬
  u8 HP_Type;         //�������ע�����ͺ�
  u8 HP_OrderType;    //��λ���·���ע�����ͺ�
  u8 HP_Compare;       //ע�����ͺŶԱ�
  u8 HP_Turn;         //���׼�⵽λ����Ҫ��ע��Ȧ�� 
  u8 HP_Length;       //��⵽λ����Ҫ��ע����
  u8 HP_Check;       //���ر�У��
  u8 Up_Mode;        //���ر�У������ģʽ
} HP_Work;
extern HP_Work HP_Data;

#define  Top_HP     GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_1)   //ע�����Ƿ�װ��λ��� PD1
#define  Bottom_HP  GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_2)   //���ص��׼��  PD2
#define  SyringeMode_S1  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_12) //ע�����ͺż�� PC12
#define  SyringeMode_S2  GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_0) //ע�����ͺż�� PD0
#define  SyringeMode_S3  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_11) //ע�����ͺż�� PC11

extern void HP_Check();
extern void Syringe_Type();
#endif 