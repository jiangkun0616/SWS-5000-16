/************************ (C) COPYRIGHT 2008 SWS *******************************
* File Name          : swsbus.c
* Author             : ͯ��
* Version            : V2.0
* Date               : 2012.11.30
* Description        : SWSBUSЭ�����

V1.1:�޸�RD0-RD7��������������ݴ�������
     �޸����CRC��ʱ��������CRC��������
     �޸�485ʹ����ʱ��������
V1.2:ɾ��4��д����Ͷ�����
     ��֡��֮֡����ʱ��Ϊ0.5ms
     CRCУ�����󲻷�����Ϣ
V2.0:����4��д����Ͷ�����
     �޸Ĵ������ú�����ӦSTM32��V3.5.0���ļ�
     �޸�ͷ�ļ�������Ӳ�����ö˿ھ���ͷ�ļ�������
********************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "swsbus.h"
#include "main.h"
#include "string.h"
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
u8  TxBuffer[TxBufferSize];    //�������ݻ���
u8  RxBuffer[RxBufferSize];    //�������ݻ���
u8  TxBufferLength;            //�������ݳ���
u8  RxBufferLength;            //�������ݳ���
struct COM_Array BUS_Buffer;
/*******************************************************************************
** �������� ��RS485_Init
** �������� ��RS485�˿ڼ���������
** ��ڲ��� ����
** ���ڲ��� ����
*******************************************************************************/
void RS485_Init(void)
{
  USART_InitTypeDef USART_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
//  TIM_OCInitTypeDef  TIM_OCInitStructure;
  
/************************  RS485�˿ڼ���������   ******************************/  
  /* ����485ͨ��TX�˿� */
  GPIO_InitStructure.GPIO_Pin = USART_TX;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIO_USART, &GPIO_InitStructure);
  /* ����485ͨ��RX�˿� */
  GPIO_InitStructure.GPIO_Pin = USART_RX;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIO_USART, &GPIO_InitStructure);
  /* ����485ͨ�ŷ��ͽ���ʹ�ܶ˿�*/
  GPIO_InitStructure.GPIO_Pin = USART_RD;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIO_USART, &GPIO_InitStructure);
  
  /* ����485ͨ�Ŵ���*/
  USART_InitStructure.USART_BaudRate = BaudRate;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(RS485_USART, &USART_InitStructure);
  USART_ITConfig(RS485_USART, USART_IT_RXNE, ENABLE);
  USART_Cmd(RS485_USART, ENABLE);
  RS485_RE;      //485����ʹ��
  
/************************  RS485ͨ�Ŷ�ʱ������   ******************************/ 
  TIM_TimeBaseStructure.TIM_Period = 4;
  TIM_TimeBaseStructure.TIM_Prescaler = 7199;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(RS485_TIM, &TIM_TimeBaseStructure);
  
  /*Prescaler configuration :*/
//  TIM_InternalClockConfig(RS485_TIM);
//  TIM_PrescalerConfig(RS485_TIM, 0x0E0F, TIM_PSCReloadMode_Immediate);//clock = 10 KHz

  /* Output Compare Active Mode configuration: Channel1*/
//  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Inactive;
//  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
//  TIM_OCInitStructure.TIM_Pulse = 5;
//  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
//  TIM_OC1Init(RS485_TIM, &TIM_OCInitStructure);
//  TIM_OC1PreloadConfig(RS485_TIM, TIM_OCPreload_Disable);
//  TIM_Cmd(RS485_TIM, DISABLE);
}

/*******************************************************************************
** �������� ��BUS_DataResponse
** �������� ������ͨ���н��ܵ�һ֡���ݺ��ж���ʲô���������Ӧ����Ӧ
** ��ڲ��� ����
** ���ڲ��� ����
*******************************************************************************/
void BUS_DataResponse(void)
{
  int i,CRC_CODE=0;
  u16 CRC_Result;                //CRCУ����
  CRC_Result = CRC16(RxBuffer,RxBufferLength);
  if(CRC_Result == 0)   //CRCУ����ȷ���Ϊ0
  {
    LED_485_ON;        //������ȷһ֡����
     
    switch(RxBuffer[1]) //���ݹ�����ִ����Ӧ����
    {
    case BUS_INI:       //��ʼ��
      break;

    case BUS_ASK:                      //��������
      TxBuffer[0] = LocalAddress;      //������ַ
      TxBuffer[1] = BUS_ASK;           //������
     CRC_CODE = CRC16(TxBuffer,2);    //����CRC��
      TxBuffer[2] = (CRC_CODE & 0x00FF);        //CRCУ����ĵ��ֽ�
      TxBuffer[3] = ((CRC_CODE >> 8) & 0x00FF); //CRCУ����ĸ��ֽ�
      RS485_SendData(MIN_BufferSize);           //����λ��������Ӧ����
      break;

    case BUS_WR0:                      //��Ի�д����0
      BUS_Buffer.WR0[0] = 1;           //��ʾ���µ����ݸ���
      for(i=2;i<RxBufferLength-2;i++)  //ȥ����ַ�����ܺ�CRC��
      {
        BUS_Buffer.WR0[i-1] = RxBuffer[i];
      }
      TxBuffer[0] = LocalAddress;      //������ַ
      TxBuffer[1] = BUS_WR0;           //������
      CRC_CODE = CRC16(TxBuffer,2);    //����CRC��
      TxBuffer[2] = (CRC_CODE & 0x00FF);        //CRCУ����ĵ��ֽ�
      TxBuffer[3] = ((CRC_CODE >> 8) & 0x00FF); //CRCУ����ĸ��ֽ�
      RS485_SendData(MIN_BufferSize);           //����λ��������Ӧ����
      break;

   case BUS_WR1:
      BUS_Buffer.WR1[0] = 1;
      for(i=2;i<RxBufferLength-2;i++)
      {
        BUS_Buffer.WR1[i-1] = RxBuffer[i];
      }
      TxBuffer[0] = LocalAddress;
      TxBuffer[1] = BUS_WR1;
      CRC_CODE = CRC16(TxBuffer,2);
      TxBuffer[2] = (CRC_CODE & 0x00FF);
      TxBuffer[3] = ((CRC_CODE >> 8) & 0x00FF);
      RS485_SendData(MIN_BufferSize);
      break;

    case BUS_WR2:
      BUS_Buffer.WR2[0] = 1;
      for(i=2;i<RxBufferLength-2;i++)
      {
        BUS_Buffer.WR2[i-1] = RxBuffer[i];
      }
      TxBuffer[0] = LocalAddress;
      TxBuffer[1] = BUS_WR2;
      CRC_CODE = CRC16(TxBuffer,2);
      TxBuffer[2] = (CRC_CODE & 0x00FF);
      TxBuffer[3] = ((CRC_CODE >> 8) & 0x00FF);
      RS485_SendData(MIN_BufferSize);
      break;

    case BUS_WR3:
      BUS_Buffer.WR3[0] = 1;
      for(i=2;i<RxBufferLength-2;i++)
      {
        BUS_Buffer.WR3[i-1] = RxBuffer[i];
      }
      TxBuffer[0] = LocalAddress;
      TxBuffer[1] = BUS_WR3;
      CRC_CODE = CRC16(TxBuffer,2);
      TxBuffer[2] = (CRC_CODE & 0x00FF);
      TxBuffer[3] = ((CRC_CODE >> 8) & 0x00FF);
      RS485_SendData(MIN_BufferSize);
      break;
      
    case BUS_WR4:
      BUS_Buffer.WR4[0] = 1;
      for(i=2;i<RxBufferLength-2;i++)
      {
        BUS_Buffer.WR4[i-1] = RxBuffer[i];
      }
      TxBuffer[0] = LocalAddress;
      TxBuffer[1] = BUS_WR4;
      CRC_CODE = CRC16(TxBuffer,2);
      TxBuffer[2] = (CRC_CODE & 0x00FF);
      TxBuffer[3] = ((CRC_CODE >> 8) & 0x00FF);
      RS485_SendData(MIN_BufferSize);
      break;

    case BUS_WR5:
      BUS_Buffer.WR5[0] = 1;
      for(i=2;i<RxBufferLength-2;i++)
      {
        BUS_Buffer.WR5[i-1] = RxBuffer[i];
      }
      TxBuffer[0] = LocalAddress;
      TxBuffer[1] = BUS_WR5;
      CRC_CODE = CRC16(TxBuffer,2);
      TxBuffer[2] = (CRC_CODE & 0x00FF);
      TxBuffer[3] = ((CRC_CODE >> 8) & 0x00FF);
      RS485_SendData(MIN_BufferSize);
      break;
      
    case BUS_WR6:
      BUS_Buffer.WR6[0] = 1;
      for(i=2;i<RxBufferLength-2;i++)
      {
        BUS_Buffer.WR6[i-1] = RxBuffer[i];
      }
      TxBuffer[0] = LocalAddress;
      TxBuffer[1] = BUS_WR6;
      CRC_CODE = CRC16(TxBuffer,2);
      TxBuffer[2] = (CRC_CODE & 0x00FF);
      TxBuffer[3] = ((CRC_CODE >> 8) & 0x00FF);
      RS485_SendData(MIN_BufferSize);
      break;

    case BUS_WR7:
      BUS_Buffer.WR7[0] = 1;
      for(i=2;i<RxBufferLength-2;i++)
      {
        BUS_Buffer.WR7[i-1] = RxBuffer[i];
      }
      TxBuffer[0] = LocalAddress;
      TxBuffer[1] = BUS_WR7;
      CRC_CODE = CRC16(TxBuffer,2);
      TxBuffer[2] = (CRC_CODE & 0x00FF);
      TxBuffer[3] = ((CRC_CODE >> 8) & 0x00FF);
      RS485_SendData(MIN_BufferSize);
      break;   
      
    case BUS_WR8:
      BUS_Buffer.WR8[0] = 1;
      for(i=2;i<RxBufferLength-2;i++)
      {
        BUS_Buffer.WR8[i-1] = RxBuffer[i];
      }
      TxBuffer[0] = LocalAddress;
      TxBuffer[1] = BUS_WR8;
      CRC_CODE = CRC16(TxBuffer,2);
      TxBuffer[2] = (CRC_CODE & 0x00FF);
      TxBuffer[3] = ((CRC_CODE >> 8) & 0x00FF);
      RS485_SendData(MIN_BufferSize);
      break;      
      
    case BUS_WR9:
      BUS_Buffer.WR9[0] = 1;
      for(i=2;i<RxBufferLength-2;i++)
      {
        BUS_Buffer.WR9[i-1] = RxBuffer[i];
      }
      TxBuffer[0] = LocalAddress;
      TxBuffer[1] = BUS_WR9;
      CRC_CODE = CRC16(TxBuffer,2);
      TxBuffer[2] = (CRC_CODE & 0x00FF);
      TxBuffer[3] = ((CRC_CODE >> 8) & 0x00FF);
      RS485_SendData(MIN_BufferSize);
      break;      

    case BUS_RD0:                        //�ӴԻ�������
      TxBuffer[0] = LocalAddress;        //������ַ
      TxBuffer[1] = BUS_RD0;             //������
      for(i=2;i<RD0_BufferSize+2;i++)
      {
        TxBuffer[i] = BUS_Buffer.RD0[i-2];    //���Ի����ݴ��͸����ڷ��ͻ�����
      }
      CRC_CODE = CRC16(TxBuffer,RD0_BufferSize + 2);//����CRC��
      TxBuffer[i] = (CRC_CODE & 0x00FF);         //CRCУ����ĵ��ֽ�
      TxBuffer[i+1] = ((CRC_CODE >> 8) & 0x00FF);//CRCУ����ĸ��ֽ�
      RS485_SendData(RD0_BufferSize + 4);       //����λ��������Ӧ����
      break;

    case BUS_RD1:
      TxBuffer[0] = LocalAddress;
      TxBuffer[1] = BUS_RD1;
      for(i=2;i<RD1_BufferSize+2;i++)
      {
        TxBuffer[i] = BUS_Buffer.RD1[i-2];
      }
      CRC_CODE = CRC16(TxBuffer,RD1_BufferSize + 2);
      TxBuffer[i] = (CRC_CODE & 0x00FF);
      TxBuffer[i+1] = ((CRC_CODE >> 8) & 0x00FF);
      RS485_SendData(RD1_BufferSize + 4);
      break;

    case BUS_RD2:
      TxBuffer[0] = LocalAddress;
      TxBuffer[1] = BUS_RD2;
      for(i=2;i<RD2_BufferSize+2;i++)
      {
        TxBuffer[i] = BUS_Buffer.RD2[i-2];
      }
      CRC_CODE = CRC16(TxBuffer,RD2_BufferSize + 2);
      TxBuffer[i] = (CRC_CODE & 0x00FF);
      TxBuffer[i+1] = ((CRC_CODE >> 8) & 0x00FF);
      RS485_SendData(RD2_BufferSize + 4);
      break;

    case BUS_RD3:
      TxBuffer[0] = LocalAddress;
      TxBuffer[1] = BUS_RD3;
      for(i=2;i<RD3_BufferSize+2;i++)
      {
        TxBuffer[i] = BUS_Buffer.RD3[i-2];
      }
      CRC_CODE = CRC16(TxBuffer,RD3_BufferSize + 2);
      TxBuffer[i] = (CRC_CODE & 0x00FF);
      TxBuffer[i+1] = ((CRC_CODE >> 8) & 0x00FF);
      RS485_SendData(RD3_BufferSize + 4);
      break;
      
    case BUS_RD4:
      TxBuffer[0] = LocalAddress;
      TxBuffer[1] = BUS_RD4;
      for(i=2;i<RD4_BufferSize+2;i++)
      {
        TxBuffer[i] = BUS_Buffer.RD4[i-2];
      }
      CRC_CODE = CRC16(TxBuffer,RD4_BufferSize + 2);
      TxBuffer[i] = (CRC_CODE & 0x00FF);
      TxBuffer[i+1] = ((CRC_CODE >> 8) & 0x00FF);
      RS485_SendData(RD4_BufferSize + 4);
      break;   
      
    case BUS_RD5:
      TxBuffer[0] = LocalAddress;
      TxBuffer[1] = BUS_RD5;
      for(i=2;i<RD5_BufferSize+2;i++)
      {
        TxBuffer[i] = BUS_Buffer.RD5[i-2];
      }
      CRC_CODE = CRC16(TxBuffer,RD5_BufferSize + 2);
      TxBuffer[i] = (CRC_CODE & 0x00FF);
      TxBuffer[i+1] = ((CRC_CODE >> 8) & 0x00FF);
      RS485_SendData(RD5_BufferSize + 4);
      break; 

    case BUS_RD6:
      TxBuffer[0] = LocalAddress;
      TxBuffer[1] = BUS_RD6;
      for(i=2;i<RD6_BufferSize+2;i++)
      {
        TxBuffer[i] = BUS_Buffer.RD6[i-2];
      }
      CRC_CODE = CRC16(TxBuffer,RD6_BufferSize + 2);
      TxBuffer[i] = (CRC_CODE & 0x00FF);
      TxBuffer[i+1] = ((CRC_CODE >> 8) & 0x00FF);
      RS485_SendData(RD6_BufferSize + 4);
      break;

    case BUS_RD7:
      TxBuffer[0] = LocalAddress;
      TxBuffer[1] = BUS_RD7;
      for(i=2;i<RD7_BufferSize+2;i++)
      {
        TxBuffer[i] = BUS_Buffer.RD7[i-2];
      }
      CRC_CODE = CRC16(TxBuffer,RD7_BufferSize + 2);
      TxBuffer[i] = (CRC_CODE & 0x00FF);
      TxBuffer[i+1] = ((CRC_CODE >> 8) & 0x00FF);
      RS485_SendData(RD7_BufferSize + 4);
      break;   
      
    case BUS_RD8:
      TxBuffer[0] = LocalAddress;
      TxBuffer[1] = BUS_RD8;
      for(i=2;i<RD8_BufferSize+2;i++)
      {
        TxBuffer[i] = BUS_Buffer.RD8[i-2];
      }
      CRC_CODE = CRC16(TxBuffer,RD8_BufferSize + 2);
      TxBuffer[i] = (CRC_CODE & 0x00FF);
      TxBuffer[i+1] = ((CRC_CODE >> 8) & 0x00FF);
      RS485_SendData(RD8_BufferSize + 4);
      break;
      
    case BUS_RD9:
      TxBuffer[0] = LocalAddress;
      TxBuffer[1] = BUS_RD9;
      for(i=2;i<RD9_BufferSize+2;i++)
      {
        TxBuffer[i] = BUS_Buffer.RD9[i-2];
      }
      CRC_CODE = CRC16(TxBuffer,RD9_BufferSize + 2);
      TxBuffer[i] = (CRC_CODE & 0x00FF);
      TxBuffer[i+1] = ((CRC_CODE >> 8) & 0x00FF);
      RS485_SendData(RD9_BufferSize + 4);
      break;
    }
  }

  /* ������ջ�������ĳ���*/
  RxBufferLength= 0;
  memset(RxBuffer,0,sizeof(RxBuffer));
}

/*******************************************************************************
** �������� ��CRC16()
** �������� ��CRCУ������㼰��⣨�������ֵ����������ɵ�CRCУ��ֵʱ��
** ��ڲ��� �����յ���һ֡���ݵ������׵�ַ���䳤��
** ���ڲ��� ��CRCУ���루�������ֵ����������ɵ�CRCУ��ֵʱ�����㣩
********************************************************************************/
static u8 aucCRCHi[] = {
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40
};

static u8 aucCRCLo[] = {
    0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7,
    0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E,
    0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09, 0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9,
    0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC,
    0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
    0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32,
    0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D,
    0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A, 0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38,
    0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF,
    0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
    0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1,
    0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4,
    0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB,
    0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA,
    0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
    0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0,
    0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97,
    0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C, 0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E,
    0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89,
    0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
    0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83,
    0x41, 0x81, 0x80, 0x40
};

u16 CRC16(u8 *Frame,u8 Length)     // ����ҪУ������������䳤��
{
  u8 CRCHi = 0xFF;
  u8 CRCLo = 0xFF;
  int iIndex;
  while( Length-- )
  {
    iIndex = CRCLo ^ *( Frame++ );
    CRCLo = ( u8 )( CRCHi ^ aucCRCHi[iIndex] );
    CRCHi = aucCRCLo[iIndex];
  }
  return ( u16 )( CRCHi << 8 | CRCLo );// CRCУ�鷵��ֵ
}

/*******************************************************************************
** �������� ��RS485_SendData
** �������� ��485��������
** ��ڲ��� ��SendDataLength
** ���ڲ��� ����
*******************************************************************************/
void RS485_SendData(u8 SendDataLength)
{
  u8 i;
  RS485_SE;   // RS485����ʹ��
  //Delay_Bus(1);
//   USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
  for(i=0;i<SendDataLength;i++)
  {
    USART_SendData(RS485_USART, TxBuffer[i]);//������Ӧ
    while(USART_GetFlagStatus(RS485_USART, USART_FLAG_TC) == RESET);
  }
//  USART_ClearITPendingBit(USART1, USART_IT_RXNE);
//  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
  RS485_RE;    // RS485����ʹ��
 // Delay_Bus(1);
}
/********************************************************************************
** �������� ��TIM_IRQ
** �������� ��TIM�жϷ������
** ��ڲ��� ����
** ���ڲ��� ����
********************************************************************************/
void TIM_IRQ(void)
{
  TIM_ClearITPendingBit(RS485_TIM, TIM_IT_Update);//��ʱ�жϱ�־����
  TIM_ITConfig(RS485_TIM, TIM_IT_Update, DISABLE);//�жϹر�
//  USART_Cmd(RS485_USART, DISABLE);   //�ش����ж�
  if((RxBuffer[0] == LocalAddress) || (RxBuffer[0] == BroadcastAddress))
  {
    BUS_DataResponse();
    
  }
  
  RxBufferLength= 0;
  memset(RxBuffer,0,sizeof(RxBuffer));
//  USART_Cmd(RS485_USART, ENABLE);   //�������ж�
  
}
/********************************************************************************
** �������� ��USART_IRQ
** �������� ��USART�жϷ������
** ��ڲ��� ����
** ���ڲ��� ����
********************************************************************************/
void USART_IRQ(void)
{  
  u8 usartsr;
 if(USART_GetITStatus(RS485_USART, USART_IT_RXNE) != RESET)
  {  
    USART_ClearITPendingBit(RS485_USART, USART_IT_RXNE);
    RxBuffer[RxBufferLength++] = USART_ReceiveData(RS485_USART);
    if(RxBufferLength >= RxBufferSize) 
      RxBufferLength = RxBufferSize-1;
    
    /*ÿ������һ���ֽ������ж�һ�μ��ʱ��*/
    TIM_ClearITPendingBit(RS485_TIM, TIM_IT_Update); //��ʱ�жϱ�־����
    TIM_ITConfig(RS485_TIM, TIM_IT_Update, ENABLE);  //�ж�ʹ��
    TIM_SetCounter(RS485_TIM,0);                  //��ʱ������
    TIM_Cmd(RS485_TIM, ENABLE);                   //��ʱ��ʹ��
  }
  else if(USART_GetFlagStatus(RS485_USART, USART_FLAG_ORE) != RESET)
  {   //���ORE(����ж�)��־
   //USART_ClearFlag(RS485_USART,USART_FLAG_ORE); //��SR��ʵ���������־
   usartsr = RS485_USART->SR;
   usartsr += USART_ReceiveData(RS485_USART);
  }
}

/********************************************************************************
** �������� ��Delay_Bus
** �������� ��ʹ�ܶ˿�״̬�仯����ʱ
** ��ڲ��� ��dly
** ���ڲ��� ����
********************************************************************************/
void Delay_Bus(u32 dly)
{
  u8 i;
  while(dly--)
  {
    i = 8;
    while(--i);
  }
}

/******************* (C) COPYRIGHT SWS *****END OF FILE*******************/
