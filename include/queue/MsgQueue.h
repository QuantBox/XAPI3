#pragma once

#ifndef _QUEUE_H_
#define _QUEUE_H_

#include "../CrossPlatform.h"
#include <thread>
#include <mutex>
#include <cstring>
#include <condition_variable>
#include <atomic>

#include "ArrayLockFreeQueue.h"

struct ResponeItem
{
	char		type;	// (RequestType)

	void*		pApi1;	// 执行者，不能delete
	void*		pApi2;	// 源头，不能delete

	double		double1;
	double		double2;

	void*		ptr1;	// 需要delete
	int			size1;
	void*		ptr2;	// 需要delete
	int			size2;
	void*		ptr3;	// 需要delete
	int			size3;

	bool		bNeedDelete;	// 是否需要删除
};


#ifdef __cplusplus
extern "C" {
#endif

	typedef void* (__stdcall *fnOnResponse) (char type,
		void* pApi1, void* pApi2,
		double double1, double double2,
		void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3);
#ifdef __cplusplus
}
#endif


using namespace std;

#pragma warning(push)
#pragma warning(disable:4251)

// 在队列dll中是导出函数
// 在api dll中不是导出函数，是导入函数

class CMsgQueue
{
public:
	bool m_bDirectOutput;
	FILE* m_infile;
	FILE* m_outfile;

	CMsgQueue()//:m_queue(1024)
	{
		m_hThread = nullptr;
		m_bRunning = false;
		m_bDirectOutput = false;

		m_infile = nullptr;
		m_outfile = nullptr;

		//回调函数地址指针
		m_fnOnResponse = nullptr;
	}
	virtual ~CMsgQueue()
	{
		if (m_infile)
		{
			fclose(m_infile);
			m_infile = nullptr;
		}
		if (m_outfile)
		{
			fclose(m_outfile);
			m_outfile = nullptr;
		}
		StopThread();
		Clear();
	}

	void SaveFileTo(const char* filename)
	{
		m_outfile = fopen(filename, "wb");
	}

public:
	//清空队列
	void Clear()
	{
		ResponeItem* pItem = nullptr;
		//清空队列
		while (m_queue.try_dequeue(pItem))
		{
			if (pItem->bNeedDelete)
			{
				delete[] pItem->ptr1;
				delete[] pItem->ptr2;
				delete[] pItem->ptr3;
			}
			delete pItem;
		}
	}

	unsigned long Size()
	{
		return m_queue.size();
	}

	void Save(FILE* outfile, ResponeItem* pItem)
	{
		if (outfile == nullptr)
			return;

		if (pItem == nullptr)
			return;

		int rc = fwrite(pItem, sizeof(ResponeItem), 1, outfile);
		// 文件没有写进去
		if (rc == 0)
			return;
		if (pItem->size1 > 0)
		{
			fwrite(pItem->ptr1, sizeof(char), pItem->size1, outfile);
		}
		if (pItem->size2 > 0)
		{
			fwrite(pItem->ptr2, sizeof(char), pItem->size2, outfile);
		}
		if (pItem->size3 > 0)
		{
			fwrite(pItem->ptr3, sizeof(char), pItem->size3, outfile);
		}
		// 主要是为了保存数据，后期调试要用
		fflush(outfile);
	}

	ResponeItem* Load(FILE* infile)
	{
		if (infile == nullptr)
			return nullptr;

		ResponeItem* pItem = new ResponeItem();
		int rc = fread(pItem, sizeof(ResponeItem), 1, infile);
		// 到文件结尾了
		if (rc == 0)
			return nullptr;
		if (pItem->size1 > 0)
		{
			char* ptr1 = new char[pItem->size1];
			fread(ptr1, sizeof(char), pItem->size1, infile);
			pItem->ptr1 = ptr1;
		}
		if (pItem->size2 > 0)
		{
			char* ptr2 = new char[pItem->size2];
			fread(ptr2, sizeof(char), pItem->size2, infile);
			pItem->ptr2 = ptr2;
		}
		if (pItem->size3 > 0)
		{
			char* ptr3 = new char[pItem->size3];
			fread(ptr3, sizeof(char), pItem->size3, infile);
			pItem->ptr3 = ptr3;
		}
		return pItem;
	}

	//可以由外部发起，顺序处理队列触发回调函数
	bool Process()
	{
		try
		{
			ResponeItem* pItem = nullptr;
			if (m_queue.try_dequeue(pItem))
			{
				Output(pItem);
				if (pItem->bNeedDelete)
				{
					// 删除的是数组，目前是自己模块创建的自己删
					delete[] pItem->ptr1;
					delete[] pItem->ptr2;
					delete[] pItem->ptr3;
				}
				delete pItem;
				return true;
			}
			return false;
		}
		catch (...)
		{
			return false;
		}
	}

	//由内部启动线程，内部主动调用Process触发回调
	void StartThread()
	{
		if (nullptr == m_hThread)
		{
			m_bRunning = true;
			m_hThread = new thread(ProcessThread, this);
		}
	}

	void StopThread()
	{
		m_bRunning = false;
		m_cv.notify_all();
		this_thread::sleep_for(chrono::milliseconds(10));
		lock_guard<mutex> cl(m_mtx_del);
		if (m_hThread)
		{
			// 这里很少有机会执行
			m_hThread->join();
			m_hThread = nullptr;
		}
	}

	//将外部的函数地址注册到队列
	void Register(fnOnResponse pCallback)
	{
		m_fnOnResponse = pCallback;
	}

	void* new_block(int size)
	{
		// 下次改用内存池
		void* p = new char[size];
		if (p == nullptr)
			return nullptr;

		memset(p, 0, size);
		return p;
	}

	void delete_block(void* p)
	{
		if (p != nullptr)
			delete[] p;
	}

	// 直接发送，不入队列
	void* Input_Output(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
	{
		try
		{
			if (m_fnOnResponse)
				return (*m_fnOnResponse)(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
		}
		catch (...)
		{
			m_fnOnResponse = nullptr;
		}
		return nullptr;
	}

	// 对输入的数据做了一次复制，主要是为了解决转过来的指针可能失效的问题。
	// 比如说STL中的指针跨线程指向的地址就无效了。所以从map中取的OrderField等都是做了次复制
	void Input_Copy(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
	{
		if (m_bDirectOutput)
		{
			Input_Output(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
			delete_block(ptr1);
			delete_block(ptr2);
			delete_block(ptr3);
			return;
		}
		ResponeItem* pItem = new ResponeItem;
		memset(pItem, 0, sizeof(ResponeItem));
		pItem->bNeedDelete = true;

		pItem->type = type;

		pItem->pApi1 = pApi1;
		pItem->pApi2 = pApi2;

		pItem->double1 = double1;
		pItem->double2 = double2;

		if (ptr1 && size1)
		{
			pItem->size1 = size1;
			pItem->ptr1 = new char[size1];
			memcpy(pItem->ptr1, ptr1, size1);
		}

		if (ptr2 && size2)
		{
			pItem->size2 = size2;
			pItem->ptr2 = new char[size2];
			memcpy(pItem->ptr2, ptr2, size2);
		}

		if (ptr3 && size3)
		{
			pItem->size3 = size3;
			pItem->ptr3 = new char[size3];
			memcpy(pItem->ptr3, ptr3, size3);
		}

		Save(m_outfile, pItem);
		m_queue.enqueue(pItem);
		// 将Sleep改成用条件变量
		m_cv.notify_all();
	}

	// 对于过来的指针不做复制，直接使用
	// 由于是跨DLL进行操作，由另一个DLL创建的内存块交给此队列进行处理时delete可能实现有变化导致出错
	// 所以必须是由此DLL new出来的内存块交给另一DLL修改返回后，再由此DLL delete，所以提前用到new_block
	void Input_NoCopy(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
	{
		if (m_bDirectOutput)
		{
			Input_Output(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
			delete_block(ptr1);
			delete_block(ptr2);
			delete_block(ptr3);
			return;
		}
		ResponeItem* pItem = new ResponeItem;
		//memset(pItem, 0, sizeof(ResponeItem));
		pItem->bNeedDelete = true;

		pItem->type = type;

		pItem->pApi1 = pApi1;
		pItem->pApi2 = pApi2;

		pItem->double1 = double1;
		pItem->double2 = double2;

		pItem->ptr1 = ptr1;
		pItem->size1 = size1;
		pItem->ptr2 = ptr2;
		pItem->size2 = size2;
		pItem->ptr3 = ptr3;
		pItem->size3 = size3;

		Save(m_outfile, pItem);
		m_queue.enqueue(pItem);
		// 将Sleep改成用条件变量
		m_cv.notify_all();
	}

	// 不做拷贝也不做delete，这种必须由其它DLL进行delete
	void Input_NoCopy_NoDelete(char type, void* pApi1, void* pApi2, double double1, double double2, void* ptr1, int size1, void* ptr2, int size2, void* ptr3, int size3)
	{
		if (m_bDirectOutput)
		{
			Input_Output(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
			return;
		}
		ResponeItem* pItem = new ResponeItem;
		//memset(pItem, 0, sizeof(ResponeItem));
		pItem->bNeedDelete = false;

		pItem->type = type;

		pItem->pApi1 = pApi1;
		pItem->pApi2 = pApi2;

		pItem->double1 = double1;
		pItem->double2 = double2;

		pItem->ptr1 = ptr1;
		pItem->size1 = size1;
		pItem->ptr2 = ptr2;
		pItem->size2 = size2;
		pItem->ptr3 = ptr3;
		pItem->size3 = size3;

		Save(m_outfile, pItem);
		m_queue.enqueue(pItem);
		// 将Sleep改成用条件变量
		m_cv.notify_all();
	}

protected:
	virtual void RunInThread()
	{
		try
		{
			while (m_bRunning)
			{
				if (Process())
				{
				}
				else
				{
					// 空闲时等1ms,如果立即有事件过来就晚了1ms
					//this_thread::sleep_for(chrono::milliseconds(1));

					// 空闲时过来等1ms,没等到就回去再试
					// 如过正好等到了，就立即去试，应当会快一点吧?

					// wait_for的效果为何不是遇到事件后就立即返回？而是一定要等满时间？太奇怪了，这个地方可以改成seconds(10)就知道效果了
					unique_lock<mutex> lck(m_mtx);
					//m_cv.wait_for(lck, std::chrono::seconds(10), [this]{return m_bRunning == false; }); //这种写法会导致收到事件立即处理的功能失效
					m_cv.wait_for(lck, std::chrono::seconds(2));
				}
			}
		}
		catch (...)
		{

		}

		// 这句加了有效果吗？
		// 加了后可能报以下错，但不当了
		/*
		************** Exception Text **************
		System.Runtime.InteropServices.SEHException(0x80004005) : External component has thrown an exception.
		at XAPI.Proxy.XRequest(Byte type, IntPtr pApi1, IntPtr pApi2, Double double1, Double double2, IntPtr ptr1, Int32 size1, IntPtr ptr2, Int32 size2, IntPtr ptr3, Int32 size3) in D : \Users\Kan\Documents\GitHub\XAPI2\languages\CSharp\XAPI_CSharp\Proxy.cs:line 79
		*/
		//m_hThread->detach();


		// 清理线程
		m_hThread = nullptr;
		m_bRunning = false;
	}

	virtual void Output(ResponeItem* pItem)
	{
		try
		{
			if (m_fnOnResponse)
				(*m_fnOnResponse)(pItem->type,
					pItem->pApi1, pItem->pApi2,
					pItem->double1, pItem->double2,
					pItem->ptr1, pItem->size1, pItem->ptr2, pItem->size2, pItem->ptr3, pItem->size3);
		}
		catch (...)
		{
			m_fnOnResponse = nullptr;
		}
	}

private:
	static void ProcessThread(CMsgQueue* lpParam)
	{
		if (lpParam)
			lpParam->RunInThread();
	}
protected:
	atomic_bool							m_bRunning;
	mutex								m_mtx;
	mutex								m_mtx_del;
	condition_variable					m_cv;
	thread*								m_hThread;

private:
	ArrayLockFreeQueue<ResponeItem*>	m_queue;

	fnOnResponse						m_fnOnResponse;

};

#pragma warning(pop)

#endif
