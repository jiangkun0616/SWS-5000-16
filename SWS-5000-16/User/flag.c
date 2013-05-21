#include "stm32f10x.h"
#include "flag.h"
#include "swsbus.h"
#include "sws_stepmotor_v2.h"
#include "hp.h"

u8  SWITCH = 0;
u8  SWITCH_SFP = 0;   //�������(0:��,1:��) ���ܱ�
u8  SWITCH_SBP = 0;  //��Һ��
u8  SWITCH_UFP = 0;  //���˱�
u8  SWITCH_BPP = 0;  //Ѫ����
u8  SWITCH_HP = 0;    //���ر�

void Down_flag()
{
  if(1==NEW_CMD)       //������Ϣ��ռδʹ��
  {
    NEW_CMD = 0;
    SWITCH = BUS_Buffer.WR0[1];
    if(2 == SWITCH)
    {
      StepMotor_Data[SFP].Down_Mode = SELF_TEST;  //�Լ�
      StepMotor_Data[SBP].Down_Mode = SELF_TEST;
      StepMotor_Data[BPP].Down_Mode = SELF_TEST;
      StepMotor_Data[UFP].Down_Mode = SELF_TEST;
    }
    if(3 == SWITCH)
    {
      StepMotor_Data[SFP].Down_Mode = SELF_TEST; //�Լ�
      StepMotor_Data[SBP].Down_Mode = SELF_TEST;
      StepMotor_Data[BPP].Down_Mode = SELF_TEST;
      StepMotor_Data[UFP].Down_Mode = SELF_TEST;
      StepMotor_Data[HP].Down_Mode = SELF_TEST;      
    }
    
  }
    
  if(1==NEW_CMD_SFP)     //���ܱ�
  {
    NEW_CMD_SFP = 0;
    SWITCH_SFP = ((0x08 & BUS_Buffer.WR1[1]) >> 3); //�ÿ���
    if(SWITCH_SFP)
    {
     StepMotor_Data[SFP].Enable = ENABLE;     
     StepMotor_Data[SFP].LockRotor = 0;    //δ��ת  
    }
    else   //�ر�
    {
     StepMotor_Data[SFP].Enable = DISABLE; 
    }
    
    StepMotor_Data[SFP].Down_Mode = (0x03 & BUS_Buffer.WR1[1]); //��תģʽ
    if(1==StepMotor_Data[SFP].Down_Mode)   //��·��װģʽ
    {
      StepMotor_Data[SFP].TestMode = ENABLE;
      StepMotor_Data[SFP].SetSpeed = INSTALL_Speed;
      StepMotor_Data[SFP].TestNum = INSTALL_Count;
      StepMotor_Data[SFP].Flow = 0;
    }
    if(((0x20 & BUS_Buffer.WR1[1]) >> 5))  //0�����ۼƣ�1��������
    {
      StepMotor_Data[SFP].Flow = 0;
    }
    StepMotor_Data[SFP].RatioSwitch = ((0x10 & BUS_Buffer.WR1[1]) >> 4); //�ù�ϵ�����Կ���
    
    StepMotor_Data[SFP].Ratio = BUS_Buffer.WR1[4]*0x100+BUS_Buffer.WR1[3];  //�ù�ϵ�� ��ml/R��
    StepMotor_Data[SFP].SetFlow = BUS_Buffer.WR1[6]*0x100+BUS_Buffer.WR1[5]; //��������ml/min��   
  }
  
  if(1==NEW_CMD_SBP)     //��Һ��
  {
    NEW_CMD_SBP = 0;
    SWITCH_SBP = ((0x08 & BUS_Buffer.WR2[1]) >> 3); //�ÿ���
    if(SWITCH_SBP)
    {
     StepMotor_Data[SBP].Enable = ENABLE;     
     StepMotor_Data[SBP].LockRotor = 0;    //δ��ת  
    }
    else   //�ر�
    {
     StepMotor_Data[SBP].Enable = DISABLE; 
    }
    StepMotor_Data[SBP].Down_Mode = (0x03 & BUS_Buffer.WR2[1]); //��תģʽ 
    if(1==StepMotor_Data[SBP].Down_Mode)   //��·��װģʽ
    {
      StepMotor_Data[SBP].TestMode = ENABLE;
      StepMotor_Data[SBP].SetSpeed = INSTALL_Speed;
      StepMotor_Data[SBP].TestNum = INSTALL_Count;
      StepMotor_Data[SBP].Flow = 0;
    }
    if(((0x20 & BUS_Buffer.WR2[1]) >> 5))  //0�ۼƣ�1�ۼƹ���
    {
      StepMotor_Data[SBP].Flow = 0;
    }
    StepMotor_Data[SBP].RatioSwitch = ((0x10 & BUS_Buffer.WR2[1]) >> 4); //�ù�ϵ�����Կ���
    
    StepMotor_Data[SBP].Ratio = BUS_Buffer.WR2[4]*0x100+BUS_Buffer.WR2[3];  //�ù�ϵ��
    StepMotor_Data[SBP].SetFlow = BUS_Buffer.WR2[6]*0x100+BUS_Buffer.WR2[5]; //��������ml/min��   
  }
  
  if(1==NEW_CMD_UFP)     //���˱�
  {
    NEW_CMD_UFP = 0;
    SWITCH_UFP = ((0x08 & BUS_Buffer.WR3[1]) >> 3); 
    if(SWITCH_UFP)//�ÿ�
    {
     StepMotor_Data[UFP].Enable = ENABLE;     
     StepMotor_Data[UFP].LockRotor = 0;    //δ��ת  
    }
    else   //�ر�
    {
     StepMotor_Data[UFP].Enable = DISABLE; 
    }
    StepMotor_Data[UFP].Down_Mode = (0x03 & BUS_Buffer.WR3[1]); //��תģʽ 
    if(1==StepMotor_Data[UFP].Down_Mode)   //��·��װģʽ
    {
      StepMotor_Data[UFP].TestMode = ENABLE;
      StepMotor_Data[UFP].SetSpeed = INSTALL_Speed;
      StepMotor_Data[UFP].TestNum = INSTALL_Count;
      StepMotor_Data[UFP].Flow = 0;
      
    }
    
    if(((0x20 & BUS_Buffer.WR3[1]) >> 5))  //0�ۼƣ�1�ۼƹ���
    {
      StepMotor_Data[UFP].Flow = 0;
    }
    StepMotor_Data[UFP].RatioSwitch = ((0x10 & BUS_Buffer.WR3[1]) >> 4); //�ù�ϵ�����Կ���
    
    StepMotor_Data[UFP].Ratio = BUS_Buffer.WR3[4]*0x100+BUS_Buffer.WR3[3];  //�ù�ϵ��
    StepMotor_Data[UFP].SetFlow = BUS_Buffer.WR3[6]*0x100+BUS_Buffer.WR3[5]; //��������ml/min��   
  }
  
  if(1==NEW_CMD_BPP)     //Ѫ����
  {
    NEW_CMD_BPP = 0;
    SWITCH_BPP = ((0x08 & BUS_Buffer.WR4[1]) >> 3); //�ÿ���
    if(SWITCH_BPP)
    {
     StepMotor_Data[BPP].Enable = ENABLE;     
     StepMotor_Data[BPP].LockRotor = 0;    //δ��ת  
    }
    else   //�ر�
    {
     StepMotor_Data[BPP].Enable = DISABLE; 
    }
    
    StepMotor_Data[BPP].Down_Mode = (0x03 & BUS_Buffer.WR4[1]); //��תģʽ 
    if(1==StepMotor_Data[BPP].Down_Mode)   //��·��װģʽ
    {
      StepMotor_Data[BPP].TestMode = ENABLE;
      StepMotor_Data[BPP].SetSpeed = INSTALL_Speed;
      StepMotor_Data[BPP].TestNum = INSTALL_Count;
      StepMotor_Data[BPP].Flow = 0;
    }
    if(((0x20 & BUS_Buffer.WR4[1]) >> 5))  //0�ۼƣ�1�ۼƹ���
    {
      StepMotor_Data[BPP].Flow=0;
    }
    StepMotor_Data[BPP].RatioSwitch = ((0x10 & BUS_Buffer.WR4[1]) >> 4); //�ù�ϵ�����Կ���
    
    StepMotor_Data[BPP].Ratio = BUS_Buffer.WR4[4]*0x100+BUS_Buffer.WR4[3];  //�ù�ϵ��
    StepMotor_Data[BPP].SetFlow = BUS_Buffer.WR4[6]*0x100+BUS_Buffer.WR4[5]; //��������ml/min��   
  }
  
  if(1==NEW_CMD_HP)      //���ر�
  {
    NEW_CMD_HP = 0;
    SWITCH_HP = ((0x08 & BUS_Buffer.WR5[1]) >> 3); //�ÿ���
    if(SWITCH_HP)
    {
     StepMotor_Data[HP].Enable = ENABLE;     
     StepMotor_Data[HP].LockRotor = 0;    //δ��ת         
    }
    else   //�ر�
    {
     StepMotor_Data[HP].Enable = DISABLE; 
    }
    StepMotor_Data[HP].SetDir = ((0x04 & BUS_Buffer.WR5[1]) >> 2); // 0˳ʱ�� ��ת��1��ת
    StepMotor_Data[HP].Down_Mode = (0x03 & BUS_Buffer.WR5[1]); //��תģʽ  
    if(((0x20 & BUS_Buffer.WR5[1]) >> 5))  //0�ۼƣ�1�ۼƹ���
    {
      StepMotor_Data[HP].Flow = 0;
    }
    StepMotor_Data[HP].SetFlow = BUS_Buffer.WR5[5]*0x100+BUS_Buffer.WR5[4];//Ŀ����ע������ml/h��
  }
  
  if(1==NEW_CMD2_HP)      //���ر�У��
  {
    NEW_CMD2_HP = 0;
    HP_Data.HP_Check=((0x80&BUS_Buffer.WR6[1])>>7);    //���ر�У������
    if(HP_Data.HP_Check)
    {
     StepMotor_Data[HP].Enable = ENABLE; 
     StepMotor_Data[HP].Down_Mode = HP_CORRECT;  //У��ģʽ
    }
    HP_Data.HP_OrderType = BUS_Buffer.WR6[2];        //ע�����ͺ�
    if(HP_Data.HP_OrderType!=HP_Data.HP_Type)     //ע�����ͺŶԱ�
    {
      HP_Data.HP_Compare = 0x01;  
    }
    
     

  }
    
}

void Up_flag()
{
 BUS_Buffer.RD0[3] = (StepMotor_Data[BPP].Up_Mode|
                     (StepMotor_Data[BPP].Alert<<2)|       //���ȡ�����
                     (StepMotor_Data[BPP].NF<<3)|          //ȱ��
                     (StepMotor_Data[BPP].LockRotor<<4)|   //��ת
                     (StepMotor_Data[BPP].RunSta_Brak<<6));   //�øǿ��� 
                                                 
 
 BUS_Buffer.RD0[3] = (StepMotor_Data[UFP].Up_Mode|
                     (StepMotor_Data[UFP].Alert<<2)|       //���ȡ�����
                     (StepMotor_Data[UFP].NF<<3)|          //ȱ��
                     (StepMotor_Data[UFP].LockRotor<<4)|   //��ת
                     (StepMotor_Data[UFP].RunSta_Brak<<6));   //�øǿ��� 
                                                 
 BUS_Buffer.RD0[2] = (StepMotor_Data[SBP].Up_Mode|
                     (StepMotor_Data[SBP].Alert<<2)|       //���ȡ�����
                     (StepMotor_Data[SBP].NF<<3)|          //ȱ��
                     (StepMotor_Data[SBP].LockRotor<<4)|   //��ת
                     (StepMotor_Data[SBP].RunSta_Brak<<6));   //�øǿ��� 
                                                
 BUS_Buffer.RD0[1] = (StepMotor_Data[SFP].Up_Mode|
                     (StepMotor_Data[SFP].Alert<<2)|       //���ȡ�����
                     (StepMotor_Data[SFP].NF<<3)|          //ȱ��
                     (StepMotor_Data[SFP].LockRotor<<4)|   //��ת
                     (StepMotor_Data[SFP].RunSta_Brak<<6));   //�øǿ��� 
                                                
  if((SELF_TEST_PASS==StepMotor_Data[SFP].Up_Mode)&&
     (SELF_TEST_PASS==StepMotor_Data[SBP].Up_Mode)&&
     (SELF_TEST_PASS==StepMotor_Data[BPP].Up_Mode)&&
     (SELF_TEST_PASS==StepMotor_Data[UFP].Up_Mode))
//     (SELF_TEST_PASS==StepMotor_Data[HP].Up_Mode))
  {
      BUS_Buffer.RD0[0]=SELF_TEST_PASS;   //5���ö��Լ�ͨ�� 
  }
  
  if((SELF_TEST_RUNING==StepMotor_Data[SFP].Up_Mode)||
     (SELF_TEST_RUNING==StepMotor_Data[SBP].Up_Mode)||
     (SELF_TEST_RUNING==StepMotor_Data[BPP].Up_Mode)||
     (SELF_TEST_RUNING==StepMotor_Data[UFP].Up_Mode)||
     (SELF_TEST_RUNING==StepMotor_Data[HP].Up_Mode))
  {
      BUS_Buffer.RD0[0]=SELF_TEST_RUNING;   //5��������һ���Լ�δ��� 
  }
  if((SELF_TEST_ERROR==StepMotor_Data[SFP].Up_Mode)||
     (SELF_TEST_ERROR==StepMotor_Data[SBP].Up_Mode)||
     (SELF_TEST_ERROR==StepMotor_Data[BPP].Up_Mode)||
     (SELF_TEST_ERROR==StepMotor_Data[UFP].Up_Mode)||
     (SELF_TEST_ERROR==StepMotor_Data[HP].Up_Mode))
  {
      BUS_Buffer.RD0[0]=SELF_TEST_ERROR;   //5��������һ���Լ��쳣 
  }
  else
  {
    BUS_Buffer.RD0[0] =  SELF_TEST_RUNING;   //�������Լ�  
  }
  
//���ܱ�
  BUS_Buffer.RD1[11]=BUS_Buffer.WR1[6];    
  BUS_Buffer.RD1[10]=BUS_Buffer.WR1[5];     //  ml/m
  BUS_Buffer.RD1[9]=BUS_Buffer.WR1[4];      //  ml/r
  BUS_Buffer.RD1[8]=BUS_Buffer.WR1[3];        
  BUS_Buffer.RD1[7]=StepMotor_Data[SFP].Flow/0x1000000;       //����ע�������ֽ�
  BUS_Buffer.RD1[6]=StepMotor_Data[SFP].Flow/0x10000%0x100;   //����ע�������ֽ�
  BUS_Buffer.RD1[5]=StepMotor_Data[SFP].Flow/0x100%0x100;     //����ע�������ֽ�
  BUS_Buffer.RD1[4]=StepMotor_Data[SFP].Flow%0x100;   
  BUS_Buffer.RD1[3]=(u8)(StepMotor_Data[SFP].SetSpeed/0x100);                   //Ŀ��ת�ٸ��ֽ�
  BUS_Buffer.RD1[2]=(u8)((u16)StepMotor_Data[SFP].SetSpeed%0x100);                   //Ŀ��ת�ٵ��ֽ� 
  BUS_Buffer.RD1[0]=(StepMotor_Data[SFP].Up_Mode<<4)|(StepMotor_Data[SFP].Status<<3)|(StepMotor_Data[SFP].LockRotor<<1)|(StepMotor_Data[SFP].RunSta_Brak<<1);
  
//��Һ��
  BUS_Buffer.RD2[11]=BUS_Buffer.WR2[6];    
  BUS_Buffer.RD2[10]=BUS_Buffer.WR2[5];     //  ml/m
  BUS_Buffer.RD2[9]=BUS_Buffer.WR2[4];      //  ml/r
  BUS_Buffer.RD2[8]=BUS_Buffer.WR2[3];        
  BUS_Buffer.RD2[7]=StepMotor_Data[SBP].Flow/0x1000000;       //����ע�������ֽ�
  BUS_Buffer.RD2[6]=StepMotor_Data[SBP].Flow/0x10000%0x100;   //����ע�������ֽ�
  BUS_Buffer.RD2[5]=StepMotor_Data[SBP].Flow/0x100%0x100;     //����ע�������ֽ�
  BUS_Buffer.RD2[4]=StepMotor_Data[SBP].Flow%0x100;   
  BUS_Buffer.RD2[3]=(u8)(StepMotor_Data[SBP].SetSpeed/0x100);                   //Ŀ��ת�ٸ��ֽ�
  BUS_Buffer.RD2[2]=(u8)((u16)StepMotor_Data[SBP].SetSpeed%0x100);                   //Ŀ��ת�ٵ��ֽ� 
  BUS_Buffer.RD2[0]=(StepMotor_Data[SBP].Up_Mode<<4)|(StepMotor_Data[SBP].Status<<3)|(StepMotor_Data[SBP].LockRotor<<1)|(StepMotor_Data[SBP].RunSta_Brak<<1);    

//���˱�
  BUS_Buffer.RD3[11]=BUS_Buffer.WR3[6];    
  BUS_Buffer.RD3[10]=BUS_Buffer.WR3[5];     //  ml/m
  BUS_Buffer.RD3[9]=BUS_Buffer.WR3[4];      //  ml/r
  BUS_Buffer.RD3[8]=BUS_Buffer.WR3[3];        
  BUS_Buffer.RD3[7]=(StepMotor_Data[UFP].Flow/STEPMOTOR_PWMPULSE)/0x1000000;       //����ע�������ֽ�
  BUS_Buffer.RD3[6]=(StepMotor_Data[UFP].Flow/STEPMOTOR_PWMPULSE)/0x10000%0x100;   //����ע�������ֽ�
  BUS_Buffer.RD3[5]=(StepMotor_Data[UFP].Flow/STEPMOTOR_PWMPULSE)/0x100%0x100;     //����ע�������ֽ�
  BUS_Buffer.RD3[4]=(StepMotor_Data[UFP].Flow/STEPMOTOR_PWMPULSE)%0x100;   
  BUS_Buffer.RD3[3]=(u8)(StepMotor_Data[UFP].SetSpeed/0x100);                   //Ŀ��ת�ٸ��ֽ�
  BUS_Buffer.RD3[2]=(u8)((u16)StepMotor_Data[UFP].SetSpeed%0x100);                   //Ŀ��ת�ٵ��ֽ� 
  BUS_Buffer.RD3[0]=(StepMotor_Data[UFP].Up_Mode<<4)|(StepMotor_Data[UFP].Status<<3)|(StepMotor_Data[UFP].LockRotor<<1)|(StepMotor_Data[UFP].RunSta_Brak<<1);      

//Ѫ����
  BUS_Buffer.RD4[11]=BUS_Buffer.WR4[6];    
  BUS_Buffer.RD4[10]=BUS_Buffer.WR4[5];     //  ml/m
  BUS_Buffer.RD4[9]=BUS_Buffer.WR4[4];      //  ml/r
  BUS_Buffer.RD4[8]=BUS_Buffer.WR4[3];        
  BUS_Buffer.RD4[7]=StepMotor_Data[BPP].Flow/0x1000000;       //����ע�������ֽ�
  BUS_Buffer.RD4[6]=StepMotor_Data[BPP].Flow/0x10000%0x100;   //����ע�������ֽ�
  BUS_Buffer.RD4[5]=StepMotor_Data[BPP].Flow/0x100%0x100;     //����ע�������ֽ�
  BUS_Buffer.RD4[4]=StepMotor_Data[BPP].Flow%0x100;   
  BUS_Buffer.RD4[3]=(u8)(StepMotor_Data[BPP].SetSpeed/0x100);                   //Ŀ��ת�ٸ��ֽ�
  BUS_Buffer.RD4[2]=(u8)((u16)StepMotor_Data[BPP].SetSpeed%0x100);                   //Ŀ��ת�ٵ��ֽ� 
  BUS_Buffer.RD4[0]=(StepMotor_Data[BPP].Up_Mode<<4)|(StepMotor_Data[BPP].Status<<3)|(StepMotor_Data[BPP].Status<<2)|(StepMotor_Data[BPP].LockRotor<<1)|(StepMotor_Data[BPP].RunSta_Brak<<1); 
  
//���ر�
  BUS_Buffer.RD5[8]=BUS_Buffer.WR5[5];      //  ��ע����ml/h
  BUS_Buffer.RD5[7]=BUS_Buffer.WR5[4];         
  BUS_Buffer.RD5[6]=StepMotor_Data[HP].Flow/0x1000000;       //����ע�������ֽ�
  BUS_Buffer.RD5[5]=StepMotor_Data[HP].Flow/0x10000%0x100;   //����ע�������ֽ�
  BUS_Buffer.RD5[4]=StepMotor_Data[HP].Flow/0x100%0x100;     //����ע�������ֽ�
  BUS_Buffer.RD5[3]=StepMotor_Data[HP].Flow%0x100;   
  BUS_Buffer.RD5[2]=HP_Data.HP_Type;
  BUS_Buffer.RD5[1]=(Bottom_HP<<1)|Top_HP;
  BUS_Buffer.RD5[0]=(StepMotor_Data[HP].Up_Mode<<4)|(StepMotor_Data[HP].Status<<3)|(StepMotor_Data[HP].SetDir<<2)|(StepMotor_Data[HP].LockRotor<<1)|(StepMotor_Data[HP].ToBottom<<7); 

  //���ر�У����Ϣ
   
//  BUS_Buffer.RD6[6]
//  BUS_Buffer.RD6[5]
//  BUS_Buffer.RD6[4]
//  BUS_Buffer.RD6[3]=
  BUS_Buffer.RD6[2]=HP_Data.HP_Type;    //ע�����ͺ�
  BUS_Buffer.RD6[0]=HP_Data.Up_Mode;   //���ر�У��ģʽ
  
    //���¹�����Ϣ
   
  BUS_Buffer.RD9[0] = CODE/0x100;         //������ʶ����ֽ�
  BUS_Buffer.RD9[1] = CODE%0x100;         //������ʶ����ֽ�
  BUS_Buffer.RD9[2] = H_VER/0x100;        //Ӳ���汾���ֽ�
  BUS_Buffer.RD9[3] = H_VER%0x100;        //Ӳ���汾���ֽ�
  BUS_Buffer.RD9[4] = S_VER/0x100;        //����汾���ֽ�
  BUS_Buffer.RD9[5] = S_VER%0x100;        //����汾���ֽ�
}

void Motor_Run_Mode()
{
  switch(StepMotor_Data[SFP].Down_Mode)    // ���ܱ�
  {
    case NORMAL_RUN:     //������ת
      {      
        StepMotor_Data[SFP].SetSpeed = StepMotor_Data[SFP].SetFlow/StepMotor_Data[SFP].Ratio;  //�������Աù�ϵ��
      }
      break;
    case SLOW_RUN:      //��·��װ���� ���̶�Ŀ��ת��
      { 

      }
      break;
    case C_TEST:       //�ù�ϵ������
      {
      }
      break;
    case SELF_TEST:   //�Լ�
      {
        
        
        if((StepMotor_Data[SFP].CurrSpeed < SELF_Speed)&&(SELF_TEST_PASS!=StepMotor_Data[SFP].Up_Mode))    //û�дﵽĿ���ٶȣ������Լ�
        {
          StepMotor_Data[SFP].Enable = ENABLE;
          StepMotor_Data[SFP].SetSpeed = SELF_Speed;
          StepMotor_Data[SFP].Up_Mode = SELF_TEST_RUNING;
        }
        if(StepMotor_Data[SFP].CurrSpeed==SELF_Speed) //�Լ����
        {
          StepMotor_Data[SFP].Up_Mode = SELF_TEST_PASS;
          StepMotor_Data[SFP].Enable = DISABLE;
          StepMotor_Data[SFP].SetSpeed = 0;
          
        }
        if((StepMotor_Data[SFP].LockRotor==1)||(StepMotor_Data[SFP].NF==1)) //�Լ���� ȱ�࣬��ת
        {
          StepMotor_Data[SFP].Up_Mode = SELF_TEST_ERROR;
        }    
      }
      break;
    default:         //��ֹͣ
      StepMotor_Data[SFP].Status = DISABLE;
      break;
     
  }
  
  switch(StepMotor_Data[SBP].Down_Mode)    // ��Һ��
  {
    case NORMAL_RUN:     //������ת
      {      
      StepMotor_Data[SBP].SetSpeed = StepMotor_Data[SBP].SetFlow/StepMotor_Data[SBP].Ratio;   // rpm/min
      }
      break;
    case SLOW_RUN:      //��·��װ���� ���̶�Ŀ��ת��
      {

      }
      break;
    case C_TEST:       //�ù�ϵ������
      break;
    case SELF_TEST:   //�Լ�
      {
        
        
        if((StepMotor_Data[SBP].CurrSpeed < SELF_Speed)&&(SELF_TEST_PASS!=StepMotor_Data[SBP].Up_Mode))    //�����Լ�
        {
          StepMotor_Data[SBP].Enable = ENABLE;
          StepMotor_Data[SBP].SetSpeed = SELF_Speed;
          StepMotor_Data[SBP].Up_Mode = SELF_TEST_RUNING;
        }
        if(StepMotor_Data[SBP].CurrSpeed==SELF_Speed) //�Լ����
        {
          StepMotor_Data[SBP].Up_Mode = SELF_TEST_PASS;
          StepMotor_Data[SBP].Enable = DISABLE;
          StepMotor_Data[SBP].SetSpeed = 0;
        }
        if((StepMotor_Data[SBP].LockRotor==1)||(StepMotor_Data[SBP].NF==1)) //�Լ���� ȱ�࣬��ת
        {
          StepMotor_Data[SBP].Up_Mode = SELF_TEST_ERROR;
        }    
      }
      break;
    default:         //��ֹͣ
      StepMotor_Data[SBP].Status = DISABLE;
      break;
     
  }
  
  
  switch(StepMotor_Data[UFP].Down_Mode)    // ���˱�
  {
    case NORMAL_RUN:     //������ת
      {      
      StepMotor_Data[UFP].SetSpeed = StepMotor_Data[UFP].SetFlow/StepMotor_Data[UFP].Ratio;
      }
      break;
    case SLOW_RUN:      //��·��װ���� ���̶�Ŀ��ת��
      {

      }
      break;
    case C_TEST:       //�ù�ϵ������
      break;
    case SELF_TEST:   //�Լ�
      {
        
        
        if((StepMotor_Data[UFP].CurrSpeed < SELF_Speed)&&(SELF_TEST_PASS!=StepMotor_Data[UFP].Up_Mode))    //�����Լ�
        {
          StepMotor_Data[UFP].Enable = ENABLE;
          StepMotor_Data[UFP].SetSpeed = SELF_Speed;
          StepMotor_Data[UFP].Up_Mode = SELF_TEST_RUNING;
        }
        if(StepMotor_Data[UFP].CurrSpeed==SELF_Speed) //�Լ����
        {
          StepMotor_Data[UFP].Up_Mode = SELF_TEST_PASS;
          StepMotor_Data[UFP].Enable = DISABLE;
          StepMotor_Data[UFP].SetSpeed = 0;
        }
        if((StepMotor_Data[UFP].LockRotor==1)||(StepMotor_Data[UFP].NF==1)) //�Լ���� ȱ�࣬��ת
        {
          StepMotor_Data[UFP].Up_Mode = SELF_TEST_ERROR;
        }    
        
      }
      break;
    default:         //��ֹͣ
      StepMotor_Data[UFP].Status = DISABLE;
      break;
     
  }
  
  switch(StepMotor_Data[BPP].Down_Mode)    // Ѫ����
  {
    case NORMAL_RUN:     //������ת
      {      
      StepMotor_Data[BPP].SetSpeed = StepMotor_Data[BPP].SetFlow/StepMotor_Data[BPP].Ratio;
      }
      break;
    case SLOW_RUN:      //��·��װ���� ���̶�Ŀ��ת��
      {
      }
      break;
    case C_TEST:       //�ù�ϵ������
      break;
    case SELF_TEST:   //�Լ�
      {
        
        
        if((StepMotor_Data[BPP].CurrSpeed < SELF_Speed)&&(SELF_TEST_PASS!=StepMotor_Data[BPP].Up_Mode))    //�����Լ�
        {
          StepMotor_Data[BPP].Enable = ENABLE;
          StepMotor_Data[BPP].SetSpeed = SELF_Speed;
          StepMotor_Data[BPP].Up_Mode = SELF_TEST_RUNING;
        }
        if(StepMotor_Data[BPP].CurrSpeed==SELF_Speed) //�Լ����
        {
          StepMotor_Data[BPP].Up_Mode = SELF_TEST_PASS;
          StepMotor_Data[BPP].Enable = DISABLE;
          StepMotor_Data[BPP].SetSpeed = 0;
        }
        if((StepMotor_Data[BPP].LockRotor==1)||(StepMotor_Data[BPP].NF==1)) //�Լ���� ȱ�࣬��ת
        {
          StepMotor_Data[BPP].Up_Mode = SELF_TEST_ERROR;
        }     
      }
      break;
    default:         //��ֹͣ
      StepMotor_Data[BPP].Status = DISABLE;
      break;
     
  }
  
  switch(StepMotor_Data[HP].Down_Mode)    // ���ر�
  {
    case HP_NORMAL_RUN:     //������ת
      {      
        StepMotor_Data[HP].Enable = ENABLE; 
        StepMotor_Data[HP].SetSpeed = StepMotor_Data[HP].SetFlow/HP_Data.HP_Flow/60;   //���� ml/h ���� ml/rpm
        if(Bottom_HP)   //���׺�ת��Ȧ��
        {
          StepMotor_Data[HP].TestNum = HP_Data.HP_Turn;
          StepMotor_Data[HP].Flow = 0;
        }
      }
      break;
    case HP_FAST_FORWARD:      //���رÿ��ģʽ
      {
      }
      break;
    case HP_SELF_TEST:   //���ر��Լ�ģʽ
      {
       StepMotor_Data[HP].Enable = ENABLE;
       StepMotor_Data[HP].SetSpeed = HPSELF_Speed;
       if(Bottom_HP)       
       {
         StepMotor_Data[HP].Up_Mode = SELF_TEST_RUNING;   //���ر���ת�Լ�
         StepMotor_Data[HP].SetDir = 1;  //���׺���
         StepMotor_Data[HP].TestNum = 0; //���׺�ת��Ȧ��
         if(1==StepMotor_Data[HP].LockRotor) //��ת���ﶥ�˺��ת
         {
          StepMotor_Data[HP].Enable = DISABLE;
          StepMotor_Data[HP].Up_Mode = SELF_TEST_PASS;   //���ر��Լ����
         }
       }
       if(1==StepMotor_Data[HP].NF)
       {
          StepMotor_Data[HP].Enable = DISABLE;
          StepMotor_Data[HP].Up_Mode = SELF_TEST_ERROR;   //���ر��Լ��쳣         
       }
      }
      break;
  case HP_CORRECT:      //���ر�У��ģʽ
      {
        if(HP_Data.HP_OrderType!=HP_Data.HP_Type)
        {
         HP_Data.Up_Mode = HP_CORRECT_ERROR;   //���ر�У���쳣 
        }
        else
        {
         HP_Data.Up_Mode = HP_CORRECT_RUNING;  //���ر�����У��
        }
      }
      break;
    default:         
      break;
     
  }
  
}


  