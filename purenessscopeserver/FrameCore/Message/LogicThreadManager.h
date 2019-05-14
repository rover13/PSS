#ifndef _LOGICTHREADMANAGER_H
#define _LOGICTHREADMANAGER_H

#include "ace/Synch.h"
#include "ace/Malloc_T.h"
#include "ace/Singleton.h"
#include "ace/Thread_Mutex.h"

#include "define.h"
#include "HashTable.h"
#include "ILogicThreadManager.h"
#include "ObjectPoolManager.h"
#include "BaseTask.h"
#include "TimerManager.h"

#define LOGICTHREAD_MAX_COUNT         100
#define LOGICTHREAD_MESSAGE_MAX_COUNT 2000

//�߼��̵߳���Ϣ��
class CLogicThreadMessage
{
public:
    CLogicThreadMessage() : m_nHashID(0), m_nMessageID(0), m_pParam(NULL)
    {
        //ָ���ϵҲ����������ֱ��ָ��������ʹ�õ�ʹ����ָ��
        m_pmbQueuePtr = new ACE_Message_Block(sizeof(CLogicThreadMessage*));

        CLogicThreadMessage** ppMessage = (CLogicThreadMessage**)m_pmbQueuePtr->base();
        *ppMessage = this;
    }

    ~CLogicThreadMessage()
    {
        if (NULL != m_pmbQueuePtr)
        {
            m_pmbQueuePtr->release();
            m_pmbQueuePtr = NULL;
        }
    }

    CLogicThreadMessage(const CLogicThreadMessage& ar)
    {
        (*this) = ar;
    }

    CLogicThreadMessage& operator = (const CLogicThreadMessage& ar)
    {
        if (this != &ar)
        {
            this->m_nMessageID = ar.m_nMessageID;
            this->m_pParam = ar.m_pParam;
        }

        return *this;
    }

    void SetHashID(int nHasnID)
    {
        m_nHashID = nHasnID;
    }

    int GetHashID()
    {
        return m_nHashID;
    }

    void Clear()
    {
        m_nMessageID = 0;
        m_pParam     = NULL;
    }

    ACE_Message_Block* GetQueueMessage()
    {
        return m_pmbQueuePtr;
    }

    int                 m_nHashID;
    int                 m_nMessageID;
    void*               m_pParam;
    ACE_Message_Block*  m_pmbQueuePtr;        //��Ϣ����ָ���
};

//�߼��̲߳���
class CLogicThreadInfo
{
public:
    CLogicThreadInfo() : m_nLogicThreadID(0),
        m_nTimeout(0),
        m_pLogicQueue(NULL)
    {
    }

    ~CLogicThreadInfo()
    {
    }

    CLogicThreadInfo(const CLogicThreadInfo& ar)
    {
        (*this) = ar;
    }

    CLogicThreadInfo& operator = (const CLogicThreadInfo& ar)
    {
        if (this != &ar)
        {
            this->m_nLogicThreadID         = ar.m_nLogicThreadID;
            this->m_nTimeout               = ar.m_nTimeout;
            this->m_pLogicQueue            = ar.m_pLogicQueue;
        }

        return *this;
    }

    int          m_nLogicThreadID;
    int          m_nTimeout;
    ILogicQueue* m_pLogicQueue;
};

//Message�����
class CLogicThreadMessagePool : public CObjectPoolManager<CLogicThreadMessage, ACE_Recursive_Thread_Mutex>
{
public:
    CLogicThreadMessagePool();
    ~CLogicThreadMessagePool();

    CLogicThreadMessagePool(const CLogicThreadMessagePool& ar);

    CLogicThreadMessagePool& operator = (const CLogicThreadMessagePool& ar)
    {
        if (this != &ar)
        {
            ACE_UNUSED_ARG(ar);
        }

        return *this;
    }

    static void Init_Callback(int nIndex, CLogicThreadMessage* pMessage);
    static void Close_Callback(int nIndex, CLogicThreadMessage* pMessage);

    CLogicThreadMessage* Create();
    bool Delete(CLogicThreadMessage* pMakePacket);
};

//����߼��߳�
class CLogicThread : public ACE_Task<ACE_MT_SYNCH>
{
public:
    CLogicThread();
    virtual ~CLogicThread();

    CLogicThread& operator = (const CLogicThread& ar)
    {
        if (this != &ar)
        {
            ACE_UNUSED_ARG(ar);
        }

        return *this;
    }

    void Init(CLogicThreadInfo objThreadInfo);

    bool Start();

    virtual int handle_signal(int signum,
                              siginfo_t* = 0,
                              ucontext_t* = 0);

    virtual int open(void* args = 0);
    virtual int svc(void);
    int Close();

    bool CheckTimeout(ACE_Time_Value tvNow);

    bool PutMessage(int nMessageID, void* pParam);

    CLogicThreadInfo* GetThreadInfo();

private:
    int CloseMsgQueue();
    bool Dispose_Queue();                                  //��������

private:
    CLogicThreadInfo                m_objThreadInfo;
    bool                            m_blRun;
    ACE_Thread_Mutex                m_logicthreadmutex;
    ACE_Condition<ACE_Thread_Mutex> m_logicthreadcond;
    uint32                          m_u4ThreadState;       //��ǰ�����߳�״̬
    ACE_Time_Value                  m_tvUpdateTime;        //�߳���������ݵ�ʱ��
    CLogicThreadMessagePool         m_MessagePool;         //��Ϣ��
};

//�߼��̹߳�����
class CLogicThreadManager : public ILogicThreadManager, public ACE_Task<ACE_MT_SYNCH>
{
public:
    CLogicThreadManager();
    virtual ~CLogicThreadManager();

    virtual int handle_timeout(const ACE_Time_Value& tv, const void* arg);

    void Init();

    void Close();

    //�����߼��߳�
    int CreateLogicThread(int nLogicThreadID,
                          int nTimeout,
                          ILogicQueue* pLogicQueue);

    virtual int CreateLogicThread(ILogicQueue* pLogicQueue)
    {

        return CreateLogicThread(pLogicQueue->GetLogicThreadID(),
                                 pLogicQueue->GetTimeOut(),
                                 pLogicQueue);
    };

    //�ر��߼��߳�
    virtual int KillLogicThread(int nLogicThreadID);

    //�����Ϣ���߼��̵߳�ӳ���ϵ
    virtual int MessageMappingLogicThread(int nLogicThreadID, int nMessageID);

    //�����߳���Ϣ
    virtual int SendLogicThreadMessage(int nMessageID, void* arg);

public:
    CHashTable<CLogicThread>     m_objThreadInfoList;
    CHashTable<CLogicThreadInfo> m_objMessageIDList;
    ACE_Recursive_Thread_Mutex   m_ThreadWriteLock;
    uint32                       m_u4TimerID;
};

typedef ACE_Singleton<CLogicThreadManager, ACE_Null_Mutex> App_LogicThreadManager;

#endif
