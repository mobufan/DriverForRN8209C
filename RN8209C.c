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

/* ����ϵͳͷ�ļ� */

/* �����Զ���ͷ�ļ� */
#include "RN8209C.h"

/* �Զ��������� */

/* �Զ���� */

/* ȫ�ֱ������� */


/*******************************************************************************
 * ����: Delay
 * ����: �򵥵���ʱ����
 * �β�: nCount -> ��ʱʱ����
 * ����: ��
 * ˵��: ��
 ******************************************************************************/
static void Delay8209(u32 nCount)
{
    /* Decrement nCount value */
    u16 i;
    while(nCount--)
    {
        for (i = 0; i < 400; i++)
            ;
    }
}

void fnUart3_WriteByte(u8 Dat)
{
    if(UART3_GetFlagStatus(UART3_FLAG_TXE) == RESET)
        UART3_SendData9(Dat);
    else
    {
        Delay8209(20);
        UART3_SendData9(Dat);
    }
}

unsigned char fnUart3_ReadByte(void)
{
    unsigned char Tmp;
    unsigned int  waitnum = 20;
    while(UART3_GetFlagStatus(UART3_FLAG_RXNE) == RESET)
    {
        Delay8209(1);
        waitnum--;
        if(waitnum == 0)    break;
    }
    Tmp = UART3_ReceiveData9();         // ��ȡ������,�Զ�����жϱ�־
    return Tmp;
}


void USART_RXITConfig(FunctionalState NewState)
{
    if(ENABLE == NewState)
    {
        UART3_ITConfig(UART3_IT_RXNE_OR, ENABLE);    // �������ж�
    }
    else
    {
        UART3_ITConfig(UART3_IT_RXNE_OR, DISABLE);     // �ؽ����ж�
    }                                                                                  // ����жϱ�־�����մ����־
    return;
}


/*******************************************************************************
 * ����: USART_TXITConfig
 * ����: ��ʼ tx ���� �ر�tx
 * �β�: newstate  disable enable
 * ����: ��
 * ˵��: 1 ��ʼ����ʱ ֱ�Ӿͷ��͵�һ���ֽڲ����ж�
 *         2 �رշ���ʱ ֱ�ӹ��ж�
 *
  ******************************************************************************/
void USART_TXITConfig(FunctionalState NewState)
{
    u8 tmp;
    if(ENABLE == NewState)
    {
        UART3_GetFlagStatus(UART3_FLAG_TC);
        tmp = *DM.Uart8209.pTx;
        DM.Uart8209.pTx++;
        UART3_SendData9(tmp);  // ��ղ����͵�һ���ֽ�
        DM.Uart8209.TxLen--;
        UART3_ITConfig(UART3_IT_TC, ENABLE);    // �������ж�
    }
    else
    {
        UART3_ITConfig(UART3_IT_TC, DISABLE);     // �ط����ж�
    }
    return;
}




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
ErrorStatus fnRn8209_Write(u8 Dst , u8 *Src , u8 DatLen)
{
    u8  i , Tmp , Repeat, chksum;
    ErrorStatus err = SUCCESS;
    if( (DatLen == 0) || (DatLen > 4) ) return(ERROR);

    for(Repeat = 1; Repeat != 0 ; Repeat--)
    {
        err = SUCCESS;
        DM.Uart8209.pTx = &DM.Uart8209.TxBuf[0];
        //д����
        Tmp = Dst | 0x80;
        *(DM.Uart8209.pTx++) = Tmp;
        chksum = Tmp;
        for(i = 0; i < DatLen; i++ )
        {
            *(DM.Uart8209.pTx++) = Src[i];	//��֯��RN8209��������
            chksum += Src[i];
        }
        chksum = ~chksum;
        *(DM.Uart8209.pTx++) = chksum;
        DM.Uart8209.TxLen = DatLen + 2;
        DM.Uart8209.pTx = &DM.Uart8209.TxBuf[0];
        USART_TXITConfig(ENABLE);       // ����ʹ�� �����͵�һ���ֽ�
        Delay8209(50);           // �ȴ����ڷ������
    }
    USART_RXITConfig(DISABLE);
    USART_TXITConfig(DISABLE);
    return(err);
}

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
ErrorStatus fnRn8209_Read1(u8 Dst, u8 *data , u8 ucLen)
{
    u8  i , Tmp, Repeat, chksum = 0, j = 0;
    ErrorStatus err = SUCCESS;
    if( (ucLen == 0) || (ucLen > 4) ) return(ERROR);

    for(Repeat = 1; Repeat != 0 ; Repeat--)
    {
        //---������------------------------------------
        Tmp = Dst;
        chksum = Dst;
        DM.Uart8209.pTx = &DM.Uart8209.TxBuf[0];
        *(DM.Uart8209.pTx) = Tmp;
        DM.Uart8209.TxLen = 1;
        USART_RXITConfig(ENABLE);       // ����ʹ��
        USART_TXITConfig(ENABLE);       // ����ʹ�� �����͵�һ���ֽ�
        Delay8209(100);                          // �ȴ�������� �������ж���
        for(i = 0; i < ucLen ; i++)
        {
            data[i] = DM.Uart8209.RxBuf[j++]; //�����յ������ݱ��浽����pBuf[]
            chksum += data[i];//����������ݵ�У���
        }
        chksum = ~chksum;
        Tmp = DM.Uart8209.RxBuf[j++];

        if(Tmp != chksum)
        {
            //��У��ʹ������������
            err = ERROR;
            for(i = 0; i < ucLen ; i++) data[i] = 0;
        }
    }
    DM.Uart8209.RxLen = 0;
    USART_RXITConfig(DISABLE);
    USART_TXITConfig(DISABLE);
    return(err);
}


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
void RN8209C_Init(void)
{
    unsigned char cTemp[4];

    // ��λ8209C TX �ܽ���������25ms
    UART3_DeInit();
    GPIO_Init(GPIOD, GPIO_PIN_5, GPIO_MODE_OUT_PP_HIGH_FAST);
    GPIO_WriteHigh(GPIOD, GPIO_PIN_5);
    GPIO_WriteLow(GPIOD, GPIO_PIN_5);
    Delay8209(200);             //ʵ�� 200ʱԼ60ms
    GPIO_WriteHigh(GPIOD, GPIO_PIN_5);

    UART3_DeInit();         /* ���Ĵ�����ֵ��λ */
    /*
     * ��UART3����Ϊ��
     * ������ = 4800
     * ����λ = 8
     * 1λֹͣλ
     * ��У��λ
     * ʹ�ܽ��պͷ���
     * ʹ�ܽ����ж�
     */
    UART3_Init(4800, UART3_WORDLENGTH_9D, UART3_STOPBITS_1, UART3_PARITY_EVEN,  UART3_MODE_TXRX_ENABLE);
    UART3_ITConfig(UART3_IT_RXNE_OR, DISABLE);      //�ؽ����ж�
    UART3_ITConfig(UART3_IT_TC, DISABLE);               // �ط����ж�
    UART3_ITConfig(UART3_IT_TXE, DISABLE);              // �ط����ж�
    UART3_Cmd(ENABLE);

    // ��ʼ��
    cTemp[0] = 0xE5;
    fnRn8209_Write(CMD8209, cTemp, 1);              // �ر�д����
    cTemp[0] = 0x00,    cTemp[1] = 0x54;
    fnRn8209_Write(HFConst, cTemp, 2);               // д��ϵͳ���ƼĴ���
    cTemp[0] = 0x15,    cTemp[1] = 0x55;
    fnRn8209_Write(SYSCON, cTemp, 2);               // д��ϵͳ���ƼĴ���
    cTemp[0] = 0xDC;
    fnRn8209_Write(CMD8209, cTemp, 1);              // ��д����


    cTemp[0] = 0x0,     cTemp[1] = 0x0,     cTemp[2] = 0x00,    cTemp[3] = 0x00;
    fnRn8209_Read1(DeviceID, cTemp, 3);              // ��ϵͳid
    cTemp[0] = 0x0,     cTemp[1] = 0x0,     cTemp[2] = 0x00,    cTemp[3] = 0x00;
    fnRn8209_Read1(SYSCON, cTemp, 2);              // ��ϵͳ������
    cTemp[0] = 0x0,     cTemp[1] = 0x0,     cTemp[2] = 0x00,    cTemp[3] = 0x00;
    fnRn8209_Read1(PhsA, cTemp, 1);              // ��ϵͳid
    cTemp[0] = 0x0,     cTemp[1] = 0x0,     cTemp[2] = 0x00,    cTemp[3] = 0x00;
    fnRn8209_Read1(EMUStatus, cTemp, 4);              // ��ϵͳid

    cTemp[0] = 0x0,     cTemp[1] = 0x0,     cTemp[2] = 0x00,    cTemp[3] = 0x00;
    fnRn8209_Read1(IARMS, cTemp, 3);              // ��ϵͳid
    cTemp[0] = 0x0,     cTemp[1] = 0x0,     cTemp[2] = 0x00,    cTemp[3] = 0x00;
    fnRn8209_Read1(IBRMS, cTemp, 3);              // ��ϵͳid
    cTemp[0] = 0x0,     cTemp[1] = 0x0,     cTemp[2] = 0x00,    cTemp[3] = 0x00;
    fnRn8209_Read1(URMS, cTemp, 3);              // ��ϵͳid
}


/*************** (C) COPYRIGHT �����㱦΢���Լ������޹�˾ *****END OF FILE****/







