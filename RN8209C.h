/*************** (C) COPYRIGHT  ������������������޹�˾���޹�˾ **************
 * �ļ���  ��RN8209C.c
 * ����    ��RN8209C����������
 * ʵ��ƽ̨��X965M X964A X963B
 * ��汾  ��V2.1.0
 * ����    ��MoYi  QQ��29577332
 * ����    ��
 * �Ա�    ��
 * �޸�ʱ�� ��2018-03-25
 * ע��    ��RN8209C �����ǲ��ô��������ģ�
                        ������Ϊcpuֻ��һ�鴮�����Ա������ǲ���ioģ��ķ�ʽ����
  X963MV1 Ӳ������
    |--------------------|
    |  TXD0-PB0          |
    |  RXD0-PB1          |
    |--------------------|

*******************************************************************************/

#ifndef __RN8209C_H
#define __RN8209C_H

/* ����ϵͳͷ�ļ� */

/* �����Զ���ͷ�ļ� */
#include "stm8s.h"
#include "main.h"

/* �Զ��������� */

/* �Զ���� */
/* 1629 оƬ���ӵ�GPIO�˿ڶ��� */
#define RN8209C_PORTB   GPIOB   /* ����1629оƬ����GPIO�˿� */
/* 1629���ӵ�GPIO���Ŷ��� */
#define RN8209CTXD      GPIO_PIN_1
#define RN8209CRXD      GPIO_PIN_0

/* ȫ�ֱ������� */

/* 8209C�ڲ��Ĵ����ĵ�ַ���� */
#define SYSCON      0x00
#define EMUCON      0x01
#define HFConst      0x02
#define PStart         0x03
#define DStart         0x04
#define GPQA          0x05
#define GPQB          0x06
#define PhsA           0x07
#define PhsB            0x08
#define QPhsCal     0x09
#define APOSA       0x0A
#define APOSB       0x0B
#define RPOSA       0x0C
#define RPOSB       0x0D
#define IARMSOS     0x0E
#define IBRMSOS     0x0F
#define IBGain        0x10
#define D2FPL         0x11
#define D2FPH        0x12
#define DCIAH        0x13
#define DCIBH        0x14
#define DCUH         0x15
#define DCL            0x16
#define EMUCON2   0x17
/* ����������״̬�Ĵ��� */
#define PFCnt       0x20
#define DFCnt       0x21
#define IARMS      0x22
#define IBRMS       0x23
#define URMS        0x24
#define UFreq       0x25
#define PowerPA     0x26
#define PowerPB     0x27
#define PowerQ      0x28
#define EnergyP     0x29
#define EnergyP2   0x2A
#define EnergyD     0x2B
#define EnergyD2    0x2C
#define EMUStatus   0x2D
#define SPL_IA       0x30
#define SPL_IB       0x31
#define SPL_U        0x32
/* �жϼĴ��� */
#define IE8209      0x40
#define IF8209      0x41
#define RIF            0x42
/* ϵͳ״̬�Ĵ��� */
#define SysStatus     0x43
#define RData           0x44
#define WData          0x45
#define DeviceID      0x7F

/* ��������Ĵ��� */
#define CMD8209     0xEA

/* �������ⲿʹ�õĺ��� */

/*******************************************************************************
 * ����: RN8209C_Init
 * ����: RN8209C����GPIO���ų�ʼ������
 * �β�: ��
 * ����: ��
 * ˵��: ��
           1 ��8209 rx ����һ��Լ 60ms����ʱ
           2 ��ʼ��uart3
           3 ��ʼ��8209������
 ******************************************************************************/
void RN8209C_Init(void);

/*******************************************************************************
 * ����: USART_TXITConfig
 * ����: ��ʼ tx ���� �ر�tx
 * �β�: newstate  disable enable
 * ����: ��
 * ˵��: 1 ��ʼ����ʱ ֱ�Ӿͷ��͵�һ���ֽڲ����ж�
 *         2 �رշ���ʱ ֱ�ӹ��ж�
 *
  ******************************************************************************/
void USART_TXITConfig(FunctionalState NewState);

/*******************************************************************************
 * ����: fnRN8209_Write
 * ����: ��8209 �Ķ�Ӧ��ַд����Ӧ�����ݲ���֤
 * �β�: dst ��ַ; src ���ݻ���; datlen ;���ݳ��� repeat �ظ�����
 * ����: ��
 * ˵��: 1 ע��stm8�Ǵ��ģʽ
 *         2 ��λ���ݷ���[0]
 *         3 ��ʱû����֤
 *
  ******************************************************************************/
ErrorStatus fnRn8209_Write(u8 Dst , u8 *Src , u8 DatLen);

/*******************************************************************************
 * ����: fnRN8209_read1
 * ����: ��8209 �Ķ�Ӧ��ַ��������
 * �β�: dst ��ַ; src ���ݻ���; datlen ;���ݳ���
 * ����: success error
 * ˵��: 1 ע��stm8�Ǵ��ģʽ
 *         2 ��λ���ݷ���[0]
 *         3 ����Ч�����֤
 *
  ******************************************************************************/
ErrorStatus fnRn8209_Read1(u8 Dst, u8 *data , u8 ucLen);

#endif

/*************** (C) COPYRIGHT �����㱦΢���Լ������޹�˾ *****END OF FILE****/

