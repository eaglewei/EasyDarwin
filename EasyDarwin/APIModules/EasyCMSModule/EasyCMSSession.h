/*
	Copyright (c) 2012-2016 EasyDarwin.ORG.  All rights reserved.
	Github: https://github.com/EasyDarwin
	WEChat: EasyDarwin
	Website: http://www.easydarwin.org
*/
/*
    File:       EasyCMSSession.h
    Contains:   CMS Session
*/

#undef COMMON_UTILITIES_LIB

#include "Task.h"
#include "TimeoutTask.h"

#include "QTSSModuleUtils.h"
#include "OSArrayObjectDeleter.h"
#include "OSMemory.h"
#include "QTSSMemoryDeleter.h"
#include "OSRef.h"
#include "StringParser.h"
#include "MyAssert.h"

#include "QTSServerInterface.h"
#include "HTTPProtocol.h"
#include "OSHeaders.h"
#include "QTSS.h"
#include "SocketUtils.h"
#include "EasyProtocol.h"

#include "HTTPRequestStream.h"
#include "HTTPResponseStream.h"
#include "HTTPRequest.h"

using namespace EasyDarwin::Protocol;
using namespace std;

#ifndef __EASY_CMS_SESSION__
#define __EASY_CMS_SESSION__

class EasyCMSSession : public Task
{
public:
    EasyCMSSession();
    virtual ~EasyCMSSession();

	ClientSocket* fSocket;

	TimeoutTask fTimeoutTask;
    
	enum
    {
		kIdle						= 0,
		kReadingMessage				= 1,
		kProcessingMessage          = 2,
		kSendingMessage             = 3,
		kCleaningUp                 = 4
    };
		
	//需要和CMS进行其他通讯的在这里填充类型，并调用相应的接口函数
	typedef enum 
	{
		kStreamStopMsg = 0

	}fEnumMsg;

	UInt32 fState;

	void CleanupRequest();

	//向EasyCMS发送停止推流请求
	QTSS_Error CSFreeStream();

	// 处理HTTPRequest请求报文
	QTSS_Error ProcessMessage();
	

	// 为CMSSession专门进行网络数据包读取的对象
	HTTPRequestStream   fInputStream;
	// 为CMSSession专门进行网络数据包发送的对象
	HTTPResponseStream  fOutputStream;

	// 初始化时为NULL
	// 在每一次请求发出或者接收命令时,都有可能生成HTTPRequest对象并进行处理
	// 每一次状态机流程在处理完成kIdle~kCleanUp的流程都需要清理HTTPRequest对象
    HTTPRequest*        fRequest;
	
	// 读取网络报文前先锁住Session防止重入读取
    OSMutex             fReadMutex;

	// Session锁
	OSMutex             fMutex;

	// 请求报文的Content部分
	char*				fContentBuffer;

	// 请求报文的Content读取偏移量,在多次读取到完整Content部分时用到
	UInt32				fContentBufferOffset;

	//接口函数
	void SetStreamStopInfo(const char * chSerial,const char * chChannel);

	void SetMsgType(fEnumMsg msg);
private:
    virtual SInt64 Run();
	char*	fSerial;//需要停止推流的设备序列号
	char*   fChannel;//需要停止推流的通道号
	fEnumMsg fMsg;
	Bool16              fLiveSession;

	// 初步判断Session Socket是否已连接
	Bool16 IsConnected() { return fSocket->GetSocket()->IsConnected(); }


};

#endif

