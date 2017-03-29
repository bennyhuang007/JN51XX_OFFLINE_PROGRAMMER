#include "fifo.h"

// Caution:Start/End ָ���Ϊ������ָ��
// Start:��ǰָ���ϴζ������ݵĵ�ַ��Ҫ�ȵ������ܶ�����һ������
// End  :��ǰָ���ϴ�д�����ݵĵ�ַ��Ҫ�ȵ�������д����һ������
// ���Ե�����ֵ���ʱ�����������:Flag_Over = 0:FIFO��û������;Flag_Over = 1:FIFO��������

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

// ��ȡFIFOʣ��ռ�
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
// ��ȡFIFO�Ѿ�ʹ�õĿռ�
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

// ��Startָ�����ȡһ�����ݰ�(\r\n��β)
// �������ݰ�����
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
            // ��FIFO����ȫ��������û�з���"\r\n"��־
            // ����FIFO����Ϊ�գ�����0���ָ�FIFO״̬����
            pFIFO->Start = Start_Backup;
            return 0;
        }
        
        if(pFIFO->Start == pFIFO->Size)
        {
            pFIFO->Start = 0;
            pFIFO->Flag_Over = 0;
        }
        
        Temp = pFIFO->pBuffer[pFIFO->Start];
        if(Count < iBufferSize)		// Buffer���ܷ�����
            *(pData + Count) = Temp;
        Count ++;
        
        if((Temp == '\n')&&(TempOld == '\r'))      // �������
        {
            if(Count <= iBufferSize)
            	return Count;
            else
                return 0;	// ���ݰ����ȳ���Buffer��С��ֱ�Ӷ���
        }
        TempOld = Temp;
    }
}
