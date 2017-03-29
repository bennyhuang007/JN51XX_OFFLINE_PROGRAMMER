#include "fifo.h"

// Caution:Start/End 指针均为满递增指针
// Start:当前指向上次读出数据的地址，要先递增才能读出下一个数据
// End  :当前指向上次写入数据的地址，要先递增才能写入下一个数据
// 所以当两个值相等时，有两种情况:Flag_Over = 0:FIFO中没有数据;Flag_Over = 1:FIFO中数据满

uint32_t FIFO_GetFree(FIFO *pFIFO);
uint32_t FIFO_GetExist(FIFO *pFIFO);

void FIFO_Config(FIFO *pFIFO,uint8_t *pBuffer,uint32_t iLenght)
{
    pFIFO->pBuffer = pBuffer;
    pFIFO->Size = iLenght;
    pFIFO->Start = 0;
    pFIFO->End = 0;
    pFIFO->Flag_Over = 0;
}

void FIFO_Reset(FIFO *pFIFO)
{
    pFIFO->Start = 0;
    pFIFO->End = 0;
    pFIFO->Flag_Over = 0;
}

ERR_CODE FIFO_WriteByte(FIFO *pFIFO,uint8_t Data)
{
    if(FIFO_GetFree(pFIFO) > 0)    
    {
        pFIFO->End++;
        if(pFIFO->End == pFIFO->Size)
        {
            pFIFO->End = 0;
            pFIFO->Flag_Over = 1;
        }
        pFIFO->pBuffer[pFIFO->End] = Data;
        return ERR_NORMAL;
    }
    else
        return ERR_FIFO_NOENOUGHSPACE;
}

ERR_CODE FIFO_WriteBuffer(FIFO *pFIFO,uint8_t *pData,uint32_t iLenght)
{
    uint32_t i;
    if(FIFO_GetFree(pFIFO) >= iLenght)
    {
        for(i = 0;i < iLenght;i++)
        {
            pFIFO->End++;
            if(pFIFO->End == pFIFO->Size)
            {
                pFIFO->End = 0;
                pFIFO->Flag_Over = 1;
            }
            pFIFO->pBuffer[pFIFO->End] = *(pData + i);
        }
        return ERR_NORMAL;
    }
    else
        return ERR_FIFO_NOENOUGHSPACE;
}

ERR_CODE FIFO_ReadByte(FIFO *pFIFO,uint8_t *pData)
{
    if(FIFO_GetExist(pFIFO) > 0)    
    {
        pFIFO->Start++;
        if(pFIFO->Start == pFIFO->Size)
        {
            pFIFO->Start = 0;
            pFIFO->Flag_Over = 0;
        }
        *pData = pFIFO->pBuffer[pFIFO->Start];
        return ERR_NORMAL;
    }
    else
        return ERR_FIFO_NOENOUGHDATA;
}

uint32_t FIFO_ReadBuffer(FIFO *pFIFO,uint8_t *pData,uint32_t iLenght)
{
    uint32_t i;
    uint32_t iActLen;
    iActLen = FIFO_GetExist(pFIFO);
    iActLen = (iActLen > iLenght?iLenght:iActLen);
    for(i = 0;i < iActLen;i++)
    {
        pFIFO->Start++;
        if(pFIFO->Start == pFIFO->Size)
        {
            pFIFO->Start = 0;
            pFIFO->Flag_Over = 0;
        }
        *(pData + i) = pFIFO->pBuffer[pFIFO->Start];
    }
    return iActLen;
}

// 获取FIFO剩余空间
uint32_t FIFO_GetFree(FIFO *pFIFO)
{
    if(pFIFO->End > pFIFO->Start)
        return (pFIFO->Size - pFIFO->End + pFIFO->Start);
    else if(pFIFO->End < pFIFO->Start)
        return (pFIFO->Start - pFIFO->End);
    else
    {
        if(pFIFO->Flag_Over)
            return 0;
        else
            return pFIFO->Size;
    }
}
// 获取FIFO已经使用的空间
uint32_t FIFO_GetExist(FIFO *pFIFO)
{
    if(pFIFO->End > pFIFO->Start)
        return (pFIFO->End - pFIFO->Start);
    else if(pFIFO->End < pFIFO->Start)
        return (pFIFO->Size + pFIFO->End - pFIFO->Start);
    else
    {
        if(pFIFO->Flag_Over)
            return pFIFO->Size;
        else
            return 0;
    }
}

// 从Start指针起读取一个数据包(\r\n结尾)
// 返回数据包长度
uint32_t FIFO_GetPacket(FIFO *pFIFO,uint8_t *pData,uint32_t iBufferSize)
{
    uint8_t Temp,TempOld = 0;
    uint32_t Start_Backup = pFIFO->Start;
    uint32_t Count = 0;
 
    if(FIFO_GetExist(pFIFO) == 0)
        return 0;
    
    while(1)
    {
        if((pFIFO->Start != pFIFO->End) || (pFIFO->Flag_Over == 1))
            pFIFO->Start++;
        else
        {
            // 将FIFO数据全部读出来没有发现"\r\n"标志
            // 或者FIFO数据为空，返回0，恢复FIFO状态数据
            pFIFO->Start = Start_Backup;
            return 0;
        }
        
        if(pFIFO->Start == pFIFO->Size)
        {
            pFIFO->Start = 0;
            pFIFO->Flag_Over = 0;
        }
        
        Temp = pFIFO->pBuffer[pFIFO->Start];
        if(Count < iBufferSize)		// Buffer还能放数据
            *(pData + Count) = Temp;
        Count ++;
        
        if((Temp == '\n')&&(TempOld == '\r'))      // 复制完成
        {
            if(Count <= iBufferSize)
            	return Count;
            else
                return 0;	// 数据包长度超过Buffer大小，直接丢弃
        }
        TempOld = Temp;
    }
}
