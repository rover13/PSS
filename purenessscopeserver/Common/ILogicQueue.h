#ifndef ILOGICQUEUE_H
#define ILOGICQUEUE_H

#include "define.h"
#include <string>

//add �빤���߳��޹ص�ҵ����Ϣ���е��߼�ҵ����,����̳к�ʵ�� liuruiqi
enum ThreadReturn
{
    THREAD_Task_Exit = 0,    //��ǰ�߳��˳�
    THREAD_Task_Finish,      //��ǰ�߳��������
};

enum ThreadError
{
    THREAD_Error_Timeout = 0,    //�߳�����
};

class ILogicQueue
{
public:
    //��ʼ������ ����:�߼�ID, ����, ���ʱ��(��)
    ILogicQueue(uint32 u4LogicThreadID, uint32 u4Timeout, std::string strDesc);
    virtual ~ILogicQueue();
public:
    //��ȡ�߳�ID
    uint32 GetLogicThreadID();
    //��ʱ
    void SetTimeOut(uint32 u4Timeout);
    uint32 GetTimeOut();
    //����
    std::string GetDescriptor();

public:
    //��ʼ������
    virtual bool Init(int nLogicThread) = 0;
    //ִ���߼�
    virtual ThreadReturn Run(int nLogicThread, int nMessage, void* arg) = 0;
    //������
    virtual uint32 Error(int nLogicThread, int nErrorID) = 0;
    //�˳��ƺ�
    virtual void Exit(int nLogicThread) = 0;

private:
    uint32      m_u4LogicThreadID;
    uint32      m_u4Timeout;
    std::string m_strDesc;
};


#endif // ILOGIC_H