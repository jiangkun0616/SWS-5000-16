/***************************************************************************
-----------�ļ���Ϣ-----------
����:SYS.h
����ģ��:SWSII�Ͳ������������
����:����
��������:2008.8.30
�޶���¼:Vser1.0
***************************************************************************/
#ifndef __MAIN_CCOND_H
#define __MAIN_CCOND_H

#define LED_HP_ON     GPIO_ResetBits(GPIOE,GPIO_Pin_6)  //PE6-HP����״ָ̬��
#define LED_HP_OFF    GPIO_SetBits(GPIOE,GPIO_Pin_6) 
#define LED_SFP_ON    GPIO_ResetBits(GPIOE,GPIO_Pin_5)  //PE5-SFP����״ָ̬��
#define LED_SFP_OFF   GPIO_SetBits(GPIOE,GPIO_Pin_5) 
#define LED_SBP_ON    GPIO_ResetBits(GPIOE,GPIO_Pin_4)  //PE4-SBP����״ָ̬��
#define LED_SBP_OFF   GPIO_SetBits(GPIOE,GPIO_Pin_4) 
#define LED_SPP_ON    GPIO_ResetBits(GPIOE,GPIO_Pin_3)  //PE3-SPP����״ָ̬��
#define LED_SPP_OFF   GPIO_SetBits(GPIOE,GPIO_Pin_3) 
#define LED_UFP_ON    GPIO_ResetBits(GPIOE,GPIO_Pin_2)  //PE2-UFP����״ָ̬��
#define LED_UFP_OFF   GPIO_SetBits(GPIOE,GPIO_Pin_2) 
#define LED_485_ON    GPIO_ResetBits(GPIOE,GPIO_Pin_1)  //PE1-485����״ָ̬��
#define LED_485_OFF   GPIO_SetBits(GPIOE,GPIO_Pin_1) 
#define LED_RUN_ON    GPIO_ResetBits(GPIOD,GPIO_Pin_15)  //PD15-RUN����״ָ̬��
#define LED_RUN_OFF   GPIO_SetBits(GPIOD,GPIO_Pin_15) 


extern u8  Work_Mode;		   //����ģʽ 
extern u8  K_ADJ; 
extern u8 RS485_STDL;
extern u8 RS485_STDH;  
extern u16 Comm_CNT;     //485ͨѶ��������
extern u8  Comm_State;   //485ͨѶ״̬
extern u16   LED_COM_Cnt;

struct RUN_PAR_A
{  
  u16 Flux;       //AҺ����10��
  u16 K_Flux;    //A�ù�ϵ��1000��
  u8 Set_Conduct;//AĿ��絼ֵ10��
  u16 Fack_Conduct; //A�絼ֵ100��
  u16 Temp;      //A�¶�10��
  u16  Set_RPM;  //A�òο�ת��X10
  u8  Fack_RPM;  //����ת��
  u16 Work_RPM;
  u16 Freq;      //AƵ��Hz
};



/*struct RS485_PAR
{
u16 Conduct_A;
u16 Conduct_B;
u16 Temp_A;
u16 Temp_B;
u16 Freq_A;
u16 Freq_B;
	  };*/ 
extern u8 TC_ADJ;
extern u8 Sec_Flag;
extern struct RUN_PAR_A Run_Par_A;
extern struct RUN_PAR_A Run_Par_B;
extern u8 SpiHead_OK;
extern u8 SPI_Buf[18];
extern u8 SPI_CNT;
extern u8 SysState;
extern u8 Back_Mode;
extern u8 Int_Conduct_A,Int_Conduct_B;
extern u8 SysErr;
extern u8 Data_OK;
extern u8 Confect_Start;
extern u8 Flag_3S;
extern u8 Flag_Sec;
extern u8 Flux_Flag;
extern void TIM2_Configuration_A(void);
extern void TIM2_Configuration_B(void);
extern void Delay(u16 uS);
extern void System_Init(void);
//void StepMotor_Set_DCY_A(u16 speed);
//void StepMotor_Set_DCY_B(u16 speed);

extern u16 CNT_15S;
extern u8  lamp_pump_cnt;
void check_lockrotor(void);

#endif