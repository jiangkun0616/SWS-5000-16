/************************ (C) COPYRIGHT SWS *******************************
* File Name          : swsbus.h
* Author             : ͯ��
* Version            : V2.0
* Date               : 2012.11.30
* Description        : SWSBUSЭ�����ͷ�ļ�
********************************************************************************/
#ifndef __SWSBUS_H
#define __SWSBUS_H


//�û������ã��ɸ�����Ҫ����
#define   RS485_TIM       TIM6                 //RS485ʹ��TIM
#define   RS485_USART     USART2               //RS485ʹ�ô���
#define   GPIO_USART      GPIOA                //RS485ʹ��GPIO(Ҫ��RD,TX,RXʹ��ͬһ��GPIO)
#define   USART_RD        GPIO_Pin_4           //���ͽ���ʹ�ܶ˿�
#define   USART_TX        GPIO_Pin_2          //���ڷ����źŶ˿�
#define   USART_RX        GPIO_Pin_3          //���ڽ����źŶ˿�
#define   RS485_RE        GPIO_ResetBits(GPIO_USART,USART_RD)//485����ʹ��
#define   RS485_SE        GPIO_SetBits(GPIO_USART,USART_RD)  //485����ʹ��

#define   BaudRate        115200                             //���ò�����
#define   LocalAddress      0x23        // ������ַ: 0x23-Һ·��
#define   BroadcastAddress  0xAA        // �㲥��ַ

#define   TxBufferSize      30          //���ͻ��������鳤��
#define   RxBufferSize      30          //���ջ��������鳤��
#define   WR0_BufferSize   0x0d        // Һ·���������й�����Ϣ�ֳ�
#define   WR1_BufferSize   0x07        // ���ܱ�������Ϣ�ֳ�
#define   WR2_BufferSize   0x07        // ��Һ��������Ϣ�ֳ�
#define   WR3_BufferSize   0x07        // ���˱�������Ϣ�ֳ�
#define   WR4_BufferSize   0x07        // Ѫ����������Ϣ�ֳ�
#define   WR5_BufferSize   0x06        // ���ر�������Ϣ�ֳ�
#define   WR6_BufferSize   0x05        // ���ر�У��������Ϣ�ֳ�
#define   WR7_BufferSize   0x01        // д����7�Ľӿ����鳤���ֳ�
#define   WR8_BufferSize   0x01        // д����8�Ľӿ����鳤���ֳ�
#define   WR9_BufferSize   0x01        // д����9�Ľӿ����鳤���ֳ�

#define   RD0_BufferSize   0x06        // Һ·���������й�����Ϣ�ֳ�
#define   RD1_BufferSize   0x0c        // ���ܱ�������Ϣ�ֳ�
#define   RD2_BufferSize   0x0c        // ��Һ��������Ϣ�ֳ�
#define   RD3_BufferSize   0x0c        // ���˱�������Ϣ�ֳ�
#define   RD4_BufferSize   0x0c        // Ѫ����������Ϣ�ֳ�
#define   RD5_BufferSize   0x09        // ���ر�������Ϣ�ֳ�
#define   RD6_BufferSize   0x0b        // ���ر�У��������Ϣ�ֳ�
#define   RD7_BufferSize   0x01        // ������7�Ľӿ����鳤���ֳ�
#define   RD8_BufferSize   0x01        // ������8�Ľӿ����鳤���ֳ�
#define   RD9_BufferSize   0x06        // ����Ӳ���汾������Ϣ�ֳ�

#define   MIN_BufferSize    0x04        // ���ж���д����Ľӿ��������С����


//�����붨�壬�û����ɸ���
#define   BUS_INI     0x00     // ͨ�ų�ʼ��״̬
#define   BUS_ASK     0x21     // ��ӻ�����������Ϣ
#define   CRC_ERR     0x55     // CRCУ�����
//#define   BUS_INI     0x20     // ͨ�ų�ʼ��״̬
//#define   BUS_ASK     0x21     // ��ӻ�����������Ϣ
//#define   CRC_ERR     0x22     // CRCУ�����

#define   BUS_WR0     0x30     // ��ӻ�д0
#define   BUS_WR1     0x31     // ��ӻ�д1
#define   BUS_WR2     0x32     // ��ӻ�д2
#define   BUS_WR3     0x33     // ��ӻ�д3
#define   BUS_WR4     0x34     // ��ӻ�д4
#define   BUS_WR5     0x35     // ��ӻ�д5
#define   BUS_WR6     0x36     // ��ӻ�д6
#define   BUS_WR7     0x37     // ��ӻ�д7
#define   BUS_WR8     0x38     // ��ӻ�д8
#define   BUS_WR9     0x39     // ��ӻ�д9 
   
#define   BUS_RD0     0x40     // �Դӻ���0
#define   BUS_RD1     0x41     // �Դӻ���1
#define   BUS_RD2     0x42     // �Դӻ���2
#define   BUS_RD3     0x43     // �Դӻ���3
#define   BUS_RD4     0x44     // �Դӻ���4
#define   BUS_RD5     0x45     // �Դӻ���5
#define   BUS_RD6     0x46     // �Դӻ���6
#define   BUS_RD7     0x47     // �Դӻ���7
#define   BUS_RD8     0x48     // �Դӻ���8
#define   BUS_RD9     0x49     // �Դӻ���9

extern u8     TxBuffer[TxBufferSize]; //�������ݻ���
extern u8     RxBuffer[RxBufferSize]; //�������ݻ���
extern u8     TxBufferLength;         //�������ݳ���
extern u8     RxBufferLength;         //�������ݳ���
extern u16    CRC_Result;             //CRCУ����
extern struct COM_Array BUS_Buffer;

struct COM_Array
{
  u8 WR0[WR0_BufferSize];    // д����0�Ľӿ�����
  u8 WR1[WR1_BufferSize];    // д����1�Ľӿ�����
  u8 WR2[WR2_BufferSize];    // д����2�Ľӿ�����
  u8 WR3[WR3_BufferSize];    // д����3�Ľӿ�����
  u8 WR4[WR4_BufferSize];    // д����4�Ľӿ�����
  u8 WR5[WR5_BufferSize];    // д����5�Ľӿ�����
  u8 WR6[WR6_BufferSize];    // д����6�Ľӿ�����
  u8 WR7[WR7_BufferSize];    // д����7�Ľӿ�����
  u8 WR8[WR8_BufferSize];    // д����8�Ľӿ�����
  u8 WR9[WR9_BufferSize];    // д����9�Ľӿ�����

  u8 RD0[RD0_BufferSize];    // ������0�Ľӿ�����
  u8 RD1[RD1_BufferSize];    // ������1�Ľӿ�����
  u8 RD2[RD2_BufferSize];    // ������2�Ľӿ�����
  u8 RD3[RD3_BufferSize];    // ������3�Ľӿ�����
  u8 RD4[RD4_BufferSize];    // ������4�Ľӿ�����
  u8 RD5[RD5_BufferSize];    // ������5�Ľӿ�����
  u8 RD6[RD6_BufferSize];    // ������6�Ľӿ�����
  u8 RD7[RD7_BufferSize];    // ������7�Ľӿ�����
  u8 RD8[RD8_BufferSize];    // ������8�Ľӿ�����
  u8 RD9[RD9_BufferSize];    // ������9�Ľӿ�����
};

void RS485_Init(void);
void BUS_DataResponse(void);             //�������ݽ���
u16  CRC16(u8 *Frame,u8 Length);         //CRCУ��
void RS485_SendData(u8 SendDataLength);  //��������
void TIM_IRQ(void);
void USART_IRQ(void);
void Delay_Bus(u32 dly);                 //��ʱ



#endif
/******************* (C) COPYRIGHT 2008 SWS *****END OF FILE*******************/
