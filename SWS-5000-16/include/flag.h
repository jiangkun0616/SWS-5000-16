#ifndef __FLAG_H
#define __FLAG_H

#define CODE             1023      //������ʶ
#define H_VER            0100      //Ӳ���汾
#define S_VER            0100      //����汾   

#define  NEW_CMD       BUS_Buffer.WR0[0] //������Ϣ(1:�������0��û��������)
#define  NEW_CMD_SFP   BUS_Buffer.WR1[0] //�������־(1:�������0��û��������)
#define  NEW_CMD_SBP   BUS_Buffer.WR2[0] //�������־(1:�������0��û��������)
#define  NEW_CMD_UFP   BUS_Buffer.WR3[0] //�������־(1:�������0��û��������)
#define  NEW_CMD_BPP   BUS_Buffer.WR4[0] //�������־(1:�������0��û��������)
#define  NEW_CMD_HP    BUS_Buffer.WR5[0] //�������־(1:�������0��û��������)
#define  NEW_CMD2_HP   BUS_Buffer.WR6[0] //�������־(1:�������0��û��������)

#define INSTALL_Speed   50      //��·��װʱ��ת��
#define INSTALL_Count  6400    //��·��װʱת��Ȧ
#define SELF_Speed   50        //���Լ�ʱת��



#define NORMAL_RUN   0       //������תģʽ
#define SLOW_RUN     1       //��·��װ����ģʽ,��Ը��ر��ǿ��ģʽ
#define C_TEST       2       //�ù�ϵ������ģʽ
#define SELF_TEST    3       //�Լ�ģʽ

#define SELF_TEST_NORMAL    0    //����״̬
#define SELF_TEST_RUNING    1    //�������Լ�
#define SELF_TEST_PASS      2    //������Լ�
#define SELF_TEST_ERROR     3    //���Լ��쳣

extern void Down_flag();
extern void Up_flag();
extern void Motor_Run_Mode();

#endif 