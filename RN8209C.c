/*************** (C) COPYRIGHT  杭州中深电力技术有限公司有限公司 **************
 * 文件名  ：RN8209C.c
 * 描述    ：RN8209C的驱动函数
 * 实验平台：X965M X964A X963B
 * 库版本  ：V2.1.0
 * 作者    ：MoYi  QQ：29577332
 * 博客    ：
 * 淘宝    ：
 * 修改时间 ：2018-03-25
 * 注意    ：RN8209C 本身是采用串口驱动的，
                        但是因为cpu只有一组串口所以本驱动是采用io模拟的方式驱动
  X963MV1 硬件连接
    |--------------------|
    |  TXD0-PB0          |
    |  RXD0-PB1          |
    |--------------------|

*******************************************************************************/

/* 包含系统头文件 */

/* 包含自定义头文件 */
#include "RN8209C.h"

/* 自定义新类型 */

/* 自定义宏 */

/* 全局变量定义 */


/*******************************************************************************
 * 名称: Delay
 * 功能: 简单的延时函数
 * 形参: nCount -> 延时时间数
 * 返回: 无
 * 说明: 无
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
    Tmp = UART3_ReceiveData9();         // 读取数据区,自动清空中断标志
    return Tmp;
}


void USART_RXITConfig(FunctionalState NewState)
{
    if(ENABLE == NewState)
    {
        UART3_ITConfig(UART3_IT_RXNE_OR, ENABLE);    // 开接收中断
    }
    else
    {
        UART3_ITConfig(UART3_IT_RXNE_OR, DISABLE);     // 关接收中断
    }                                                                                  // 清空中断标志及接收错误标志
    return;
}


/*******************************************************************************
 * 名称: USART_TXITConfig
 * 功能: 开始 tx 或者 关闭tx
 * 形参: newstate  disable enable
 * 返回: 无
 * 说明: 1 开始发送时 直接就发送第一个字节并开中断
 *         2 关闭发送时 直接关中断
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
        UART3_SendData9(tmp);  // 清空并发送第一个字节
        DM.Uart8209.TxLen--;
        UART3_ITConfig(UART3_IT_TC, ENABLE);    // 开发送中断
    }
    else
    {
        UART3_ITConfig(UART3_IT_TC, DISABLE);     // 关发射中断
    }
    return;
}




/*******************************************************************************
 * 名称: fnRN8209_Write
 * 功能: 向8209 的对应地址写入相应的数据并验证
 * 形参: dst 地址; src 数据缓冲; datlen ;数据长度 repeat 重复次数
 * 返回: 无
 * 说明: 1 注意stm8是大端模式
 *         2 高位数据放在[0]
 *         3 暂时没有验证
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
        //写数据
        Tmp = Dst | 0x80;
        *(DM.Uart8209.pTx++) = Tmp;
        chksum = Tmp;
        for(i = 0; i < DatLen; i++ )
        {
            *(DM.Uart8209.pTx++) = Src[i];	//组织向RN8209发送数据
            chksum += Src[i];
        }
        chksum = ~chksum;
        *(DM.Uart8209.pTx++) = chksum;
        DM.Uart8209.TxLen = DatLen + 2;
        DM.Uart8209.pTx = &DM.Uart8209.TxBuf[0];
        USART_TXITConfig(ENABLE);       // 发送使能 并发送第一个字节
        Delay8209(50);           // 等待串口发送完成
    }
    USART_RXITConfig(DISABLE);
    USART_TXITConfig(DISABLE);
    return(err);
}

/*******************************************************************************
 * 名称: fnRN8209_read1
 * 功能: 向8209 的对应地址读出数据
 * 形参: dst 地址; src 数据缓冲; datlen ;数据长度
 * 返回: success error
 * 说明: 1 注意stm8是大端模式
 *         2 高位数据放在[0]
 *         3 采用效验和验证
 *
  ******************************************************************************/
ErrorStatus fnRn8209_Read1(u8 Dst, u8 *data , u8 ucLen)
{
    u8  i , Tmp, Repeat, chksum = 0, j = 0;
    ErrorStatus err = SUCCESS;
    if( (ucLen == 0) || (ucLen > 4) ) return(ERROR);

    for(Repeat = 1; Repeat != 0 ; Repeat--)
    {
        //---读数据------------------------------------
        Tmp = Dst;
        chksum = Dst;
        DM.Uart8209.pTx = &DM.Uart8209.TxBuf[0];
        *(DM.Uart8209.pTx) = Tmp;
        DM.Uart8209.TxLen = 1;
        USART_RXITConfig(ENABLE);       // 接收使能
        USART_TXITConfig(ENABLE);       // 发送使能 并发送第一个字节
        Delay8209(100);                          // 等待接收完成 接收在中断中
        for(i = 0; i < ucLen ; i++)
        {
            data[i] = DM.Uart8209.RxBuf[j++]; //将接收到的数据保存到数组pBuf[]
            chksum += data[i];//计算接收数据的校验和
        }
        chksum = ~chksum;
        Tmp = DM.Uart8209.RxBuf[j++];

        if(Tmp != chksum)
        {
            //若校验和错误，清接收数据
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
 * 名称: RN8209C_Init
 * 功能: RN8209C外设GPIO引脚初始化操作
 * 形参: 无
 * 返回: 无
 * 说明: 无
           1 对8209 rx 加入一个约 60ms的延时
           2 初始化uart3
           3 初始化8209的配置
 ******************************************************************************/
void RN8209C_Init(void)
{
    unsigned char cTemp[4];

    // 复位8209C TX 管脚下拉超过25ms
    UART3_DeInit();
    GPIO_Init(GPIOD, GPIO_PIN_5, GPIO_MODE_OUT_PP_HIGH_FAST);
    GPIO_WriteHigh(GPIOD, GPIO_PIN_5);
    GPIO_WriteLow(GPIOD, GPIO_PIN_5);
    Delay8209(200);             //实测 200时约60ms
    GPIO_WriteHigh(GPIOD, GPIO_PIN_5);

    UART3_DeInit();         /* 将寄存器的值复位 */
    /*
     * 将UART3配置为：
     * 波特率 = 4800
     * 数据位 = 8
     * 1位停止位
     * 无校验位
     * 使能接收和发送
     * 使能接收中断
     */
    UART3_Init(4800, UART3_WORDLENGTH_9D, UART3_STOPBITS_1, UART3_PARITY_EVEN,  UART3_MODE_TXRX_ENABLE);
    UART3_ITConfig(UART3_IT_RXNE_OR, DISABLE);      //关接收中断
    UART3_ITConfig(UART3_IT_TC, DISABLE);               // 关发射中断
    UART3_ITConfig(UART3_IT_TXE, DISABLE);              // 关发射中断
    UART3_Cmd(ENABLE);

    // 初始化
    cTemp[0] = 0xE5;
    fnRn8209_Write(CMD8209, cTemp, 1);              // 关闭写保护
    cTemp[0] = 0x00,    cTemp[1] = 0x54;
    fnRn8209_Write(HFConst, cTemp, 2);               // 写入系统控制寄存器
    cTemp[0] = 0x15,    cTemp[1] = 0x55;
    fnRn8209_Write(SYSCON, cTemp, 2);               // 写入系统控制寄存器
    cTemp[0] = 0xDC;
    fnRn8209_Write(CMD8209, cTemp, 1);              // 打开写保护


    cTemp[0] = 0x0,     cTemp[1] = 0x0,     cTemp[2] = 0x00,    cTemp[3] = 0x00;
    fnRn8209_Read1(DeviceID, cTemp, 3);              // 读系统id
    cTemp[0] = 0x0,     cTemp[1] = 0x0,     cTemp[2] = 0x00,    cTemp[3] = 0x00;
    fnRn8209_Read1(SYSCON, cTemp, 2);              // 读系统控制字
    cTemp[0] = 0x0,     cTemp[1] = 0x0,     cTemp[2] = 0x00,    cTemp[3] = 0x00;
    fnRn8209_Read1(PhsA, cTemp, 1);              // 读系统id
    cTemp[0] = 0x0,     cTemp[1] = 0x0,     cTemp[2] = 0x00,    cTemp[3] = 0x00;
    fnRn8209_Read1(EMUStatus, cTemp, 4);              // 读系统id

    cTemp[0] = 0x0,     cTemp[1] = 0x0,     cTemp[2] = 0x00,    cTemp[3] = 0x00;
    fnRn8209_Read1(IARMS, cTemp, 3);              // 读系统id
    cTemp[0] = 0x0,     cTemp[1] = 0x0,     cTemp[2] = 0x00,    cTemp[3] = 0x00;
    fnRn8209_Read1(IBRMS, cTemp, 3);              // 读系统id
    cTemp[0] = 0x0,     cTemp[1] = 0x0,     cTemp[2] = 0x00,    cTemp[3] = 0x00;
    fnRn8209_Read1(URMS, cTemp, 3);              // 读系统id
}


/*************** (C) COPYRIGHT 杭州浙宝微电脑技术有限公司 *****END OF FILE****/







