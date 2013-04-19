/***************************************************************************
-----------�ļ���Ϣ-----------
����:SYS.h
����ģ��:SWSII�Ͳ������������
����:����
��������:2008.8.30
�޶���¼:Vser1.0
***************************************************************************/
#ifndef __SWSBUS_CCOND_H
#define __SWSBUS_CCOND_H

//�û������ã��ɸ�����Ҫ����
#define   RS485_TIM_Ccond       TIM1                 //RS485ʹ��TIM
#define   RS485_USART_Ccond     USART1               //RS485ʹ�ô���
#define   GPIO_USART_Ccond      GPIOA                //RS485ʹ��GPIO(Ҫ��RD,TX,RXʹ��ͬһ��GPIO)
#define   USART_RD_Ccond        GPIO_Pin_8           //���ͽ���ʹ�ܶ˿�
#define   USART_TX_Ccond        GPIO_Pin_9          //���ڷ����źŶ˿�
#define   USART_RX_Ccond        GPIO_Pin_10          //���ڽ����źŶ˿�
#define   RS485_RE_Ccond        GPIO_ResetBits(GPIO_USART_Ccond,USART_RD_Ccond)//485����ʹ��
#define   RS485_SE_Ccond        GPIO_SetBits(GPIO_USART_Ccond,USART_RD_Ccond)  //485����ʹ��
#define   BaudRate_Ccond        115200                             //���ò�����

/////////RS-485ͨ��///////////////
#define   RS485_Receive_Ccond(ENABLE)  GPIO_ResetBits(GPIO_USART_Ccond, USART_RD_Ccond)
#define   RS485_Send_Ccond(ENABLE)     GPIO_SetBits(GPIO_USART_Ccond, USART_RD_Ccond)

#define   LocalAddress_Ccond       0x25      // ������ַ  
#define   BroadcastAddress_Ccond   0xAA      // �㲥��ַ    
#define   Main_ADD_Ccond           0x55

//�û������ã��ɸ�����Ҫ�������鳤��
#define   TxBufferSize_Ccond     20          //���ͻ��������鳤�� 
#define   RxBufferSize_Ccond     20          //���ջ��������鳤��

#define   WR0_BufferSize_Ccond   0x0F        // д����0�Ľӿ����鳤�� 
#define   WR1_BufferSize_Ccond   0x0A        // д����1�Ľӿ����鳤��
#define   WR2_BufferSize_Ccond   0x0A        // д����2�Ľӿ����鳤�� 
#define   WR3_BufferSize_Ccond   0x0A        // д����3�Ľӿ����鳤��
#define   WR4_BufferSize_Ccond   0x0A        // д����4�Ľӿ����鳤�� 
#define   WR5_BufferSize_Ccond   0x0A        // д����5�Ľӿ����鳤��
#define   WR6_BufferSize_Ccond   0x0A        // д����6�Ľӿ����鳤��
#define   WR7_BufferSize_Ccond   0x0A        // д����7�Ľӿ����鳤��

#define   RD0_BufferSize_Ccond   0x0F        // ������0�Ľӿ����鳤��
#define   RD1_BufferSize_Ccond   0x0A        // ������1�Ľӿ����鳤�� 
#define   RD2_BufferSize_Ccond   0x0A        // ������2�Ľӿ����鳤�� 
#define   RD3_BufferSize_Ccond   0x0A        // ������3�Ľӿ����鳤�� 
#define   RD4_BufferSize_Ccond   0x0A        // ������4�Ľӿ����鳤�� 
#define   RD5_BufferSize_Ccond   0x0A        // ������5�Ľӿ����鳤�� 
#define   RD6_BufferSize_Ccond   0x0A        // ������6�Ľӿ����鳤�� 
#define   RD7_BufferSize_Ccond   0x0c        // ������7�Ľӿ����鳤�� 

#define   MIN_BufferSize_Ccond   0x04        // ���ж���д����Ľӿ��������С����


//extern  u8  EnterInterruptFlag;        //�����жϱ�־
extern  u8  TxBuffer_Ccond[TxBufferSize_Ccond];    //�������ݻ���
extern  u8  RxBuffer_Ccond[RxBufferSize_Ccond];    //�������ݻ���
extern  u8  TxBufferLength_Ccond;            //�������ݳ���
extern  u8  RxBufferLength_Ccond;            //�������ݳ���
//extern  u16 CRC_Result;                //CRCУ����

extern struct COM_Array_Ccond BUS_Buffer_Ccond;

struct COM_Array_Ccond
{
  u8 WR0[WR0_BufferSize_Ccond];    // д����0�Ľӿ�����
  u8 WR1[WR1_BufferSize_Ccond];    // д����1�Ľӿ�����
  u8 WR2[WR2_BufferSize_Ccond];    // д����2�Ľӿ�����
  u8 WR3[WR3_BufferSize_Ccond];    // д����3�Ľӿ�����
  u8 WR4[WR4_BufferSize_Ccond];    // д����4�Ľӿ�����
  u8 WR5[WR5_BufferSize_Ccond];    // д����5�Ľӿ�����
  u8 WR6[WR6_BufferSize_Ccond];    // д����6�Ľӿ�����
  u8 WR7[WR7_BufferSize_Ccond];    // д����7�Ľӿ�����
  
  u8 RD0[RD0_BufferSize_Ccond];    // ������0�Ľӿ�����
  u8 RD1[RD1_BufferSize_Ccond];    // ������1�Ľӿ�����
  u8 RD2[RD2_BufferSize_Ccond];    // ������2�Ľӿ�����
  u8 RD3[RD3_BufferSize_Ccond];    // ������3�Ľӿ�����
  u8 RD4[RD4_BufferSize_Ccond];    // ������4�Ľӿ�����
  u8 RD5[RD5_BufferSize_Ccond];    // ������5�Ľӿ�����
  u8 RD6[RD6_BufferSize_Ccond];    // ������6�Ľӿ�����
  u8 RD7[RD7_BufferSize_Ccond];    // ������7�Ľӿ����� 
};
/*    //������Щ��������swsbus.h�ж��� - ����ע�͵�
//�����붨�壬�û����ɸ���
#define   BUS_INI     0x20     // ͨ�ų�ʼ��״̬           
#define   BUS_ASK     0x21     // ��ӻ�����������Ϣ 
#define   CRC_ERR     0x22     // CRCУ����� 

#define   BUS_WR0     0x30     // ��ӻ�д0 
#define   BUS_WR1     0x31     // ��ӻ�д1
#define   BUS_WR2     0x32     // ��ӻ�д2
#define   BUS_WR3     0x33     // ��ӻ�д3
#define   BUS_WR4     0x34     // ��ӻ�д4
#define   BUS_WR5     0x35     // ��ӻ�д5
#define   BUS_WR6     0x36     // ��ӻ�д6
#define   BUS_WR7     0x37     // ��ӻ�д7

#define   BUS_RD0     0x40     // �Դӻ���0
#define   BUS_RD1     0x41     // �Դӻ���1
#define   BUS_RD2     0x42     // �Դӻ���2
#define   BUS_RD3     0x43     // �Դӻ���3
#define   BUS_RD4     0x44     // �Դӻ���4
#define   BUS_RD5     0x45     // �Դӻ���5
#define   BUS_RD6     0x46     // �Դӻ���6
#define   BUS_RD7     0x47     // �Դӻ���7
*/
extern void BUS_DataResponse_Ccond(void);            //�������ݽ���
extern u16 CRC16(u8 *Frame,u8 Length);         //CRCУ��
extern void USART1_SendData(u8 SendDataLength);//��������   
extern u8 CSTX_Mode;                //������ģʽ��־λ,lym20100322
void Delay_485_1(u32 dly);
void RS485_Init_Ccond(void);


#endif