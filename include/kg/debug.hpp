/**
*	\file kg/debug.hpp
*   \brief 日誌調試庫
*
*	使用宏定義的一個日誌調試庫 以便在 Release 版本時 將日誌全部刪除\n
*	所有 宏只在 定義了 相應日誌等級 時 才工作\n
*	在沒 定義 時 被替換為 空
*
*   \code
#include <iostream>

#define KG_DEBUG_USE_DEBUG
//引入日誌庫
#include <kg/debug.hpp>

//啓用 自定義的 日誌類型
#define KG_DEBUG_MY

//擴展一個 新的 日誌類型
#ifdef KG_DEBUG_MY
#define KG_MY_SECTION(XXX) do{XXX}while(0);
#else
#define KG_MY_SECTION(XXX)
#endif // KG_DEBUG_MY

#ifdef KG_DEBUG_MY
#define KG_MY(XXX) KG_DEBUG_COUT("[my]",XXX)
#define KG_FORMAT_MY(...) KG_DEBUG_FORMAT_COUT("[my]",__VA_ARGS__)
#else
#define KG_MY(XXX)
#define KG_FORMAT_MY(...)
#endif // KG_DEBUG_MY


//取消預定義 選項
#undef	KG_DEBUG_STREAM
#undef	KG_DEBUG_SHOW_FILE
#undef	KG_DEBUG_SHOW_LINE
#undef	KG_DEBUG_COUT_FLUSH
#undef	KG_DEBUG_STREAM

//設置日誌選項
#define KG_DEBUG_SHOW_FILE 1
#define KG_DEBUG_SHOW_LINE 1
#define KG_DEBUG_COUT_FLUSH 1

//自定義 日誌輸出位置
class out_t
{
public:
	//實現輸出
	template<typename T>
	out_t& operator<<(const T& t)
	{
		std::cout<<t;
		return *this;
	}

	//out_t& operator<<(const char* str)
	//{
	//	std::cout<<str;
	//	return *this;
	//}
	//...




	//實現刷新緩衝區
	void flush()
	{
		std::cout.flush();
	}
};
out_t _kg_logout;
#define KG_DEBUG_STREAM   _kg_logout

int main()
{
	//fmt
	KG_FORMAT_TRACE("%d %s",1,"trace");
	KG_FORMAT_DEBUG("%d %s",2,"debug");
	KG_FORMAT_INFO("%d %s",3,"info");
	KG_FORMAT_ERROR("%d %s",4,"error");
	KG_FORMAT_FAULT("%d %s",5,"fault");
	KG_FORMAT_MY("%d %s",0,"my");

	//stream
	KG_TRACE(1<<" trace");
	KG_DEBUG(2<<" debug");
	KG_INFO(3<<" info");
	KG_ERROR(4<<" error");
	KG_FAULT(5<<" fault");
	KG_MY(0<<" my");


	//section
	KG_TRACE_SECTION(
					std::cout<<"section";
					std::cout<<" trace"<<std::endl;
	);
	KG_DEBUG_SECTION(
					std::cout<<"section";
					std::cout<<" debug"<<std::endl;
	);
	KG_INFO_SECTION(
					std::cout<<"section";
					std::cout<<" info"<<std::endl;
	);
	KG_TRACE_SECTION(
					std::cout<<"section";
					std::cout<<" trace"<<std::endl;
	);
	KG_ERROR_SECTION(
					std::cout<<"section";
					std::cout<<" error"<<std::endl;
	);
	KG_FAULT_SECTION(
					std::cout<<"section";
					std::cout<<" fault"<<std::endl;
	);
	KG_MY_SECTION(
					std::cout<<"section";
					std::cout<<" my"<<std::endl;
	);

	return 0;
}

*   \endcode
*/
#ifndef KG_LIB_HEADER_DEBUG
#define KG_LIB_HEADER_DEBUG

#include <iostream>
#include <cstdio>


/**
*	\def	KG_DEBUG_USE_RELEASE
*
*	\brief	使用 release 模式 輸出日誌
*
*	如果定義了 KG_DEBUG_USE_RELEASE 將自動定義	\n
*	KG_DEBUG_INFO	\n
*	KG_DEBUG_ERROR	\n
*	KG_DEBUG_FAULT	\n
*/
#ifdef KG_DEBUG_USE_RELEASE
#define KG_DEBUG_INFO
#define KG_DEBUG_FAULT
#define KG_DEBUG_ERROR
#else
#define KG_DEBUG_USE_RELEASE
#endif // KG_DEBUG_USE_RELEASE

/**
*	\def	KG_DEBUG_USE_DEBUG
*
*	\brief	使用 debug 模式 輸出日誌
*
*	如果定義了 KG_DEBUG_USE_DEBUG 將自動定義	\n
*	KG_DEBUG_TRACE	\n
*	KG_DEBUG_DEBUG	\n
*	KG_DEBUG_INFO	\n
*	KG_DEBUG_ERROR	\n
*	KG_DEBUG_FAULT	\n
*/
#ifdef KG_DEBUG_USE_DEBUG
#define KG_DEBUG_TRACE
#define KG_DEBUG_DEBUG
#define KG_DEBUG_INFO
#define KG_DEBUG_FAULT
#define KG_DEBUG_ERROR
#else
#define KG_DEBUG_USE_DEBUG
#endif // KG_DEBUG_USE_DEBUG




//數據 過濾 等級
#define _IGONE_KG_DEBUG_LV_TRACE  1
#define _IGONE_KG_DEBUG_LV_DEBUG  2
#define _IGONE_KG_DEBUG_LV_INFO   3
#define _IGONE_KG_DEBUG_LV_ERROR  4
#define _IGONE_KG_DEBUG_LV_FAULT  5

/**
*	\brief TRACE 日誌 默認的 tag
*/
#ifndef KG_DEBUG_TAG_TRACE
#define KG_DEBUG_TAG_TRACE "[trace]"
#endif // KG_DEBUG_TAG_TRACE

/**
*	\brief DEBUG 日誌 默認的 tag
*/
#ifndef KG_DEBUG_TAG_DEBUG
#define KG_DEBUG_TAG_DEBUG "[debug]"
#endif // KG_DEBUG_TAG_DEBUG

/**
*	\brief INFO 日誌 默認的 tag
*/
#ifndef KG_DEBUG_TAG_INFO
#define KG_DEBUG_TAG_INFO "[info]"
#endif // KG_DEBUG_TAG_INFO

/**
*	\brief FAULT 日誌 默認的 tag
*/
#ifndef KG_DEBUG_TAG_FAULT
#define KG_DEBUG_TAG_FAULT "[fault]"
#endif // KG_DEBUG_TAG_FAULT

/**
*	\brief ERROR 日誌 默認的 tag
*/
#ifndef KG_DEBUG_TAG_ERROR
#define KG_DEBUG_TAG_ERROR "[error]"
#endif // KG_DEBUG_TAG_ERROR



/**
*	\brief 非0 日誌將顯示 檔案名
*/
#ifndef KG_DEBUG_SHOW_FILE
#define KG_DEBUG_SHOW_FILE 0
#endif // KG_DEBUG_SHOW_FILE

/**
*	\brief 非0 日誌將顯示 所在行
*/
#ifndef KG_DEBUG_SHOW_LINE
#define KG_DEBUG_SHOW_LINE 0
#endif // KG_DEBUG_SHOW_LINE

/**
*	\brief 非0 日誌將顯示 tag
*/
#ifndef KG_DEBUG_SHOW_TAG
#define KG_DEBUG_SHOW_TAG 1
#endif // KG_DEBUG_SHOW_TAG

/**
*	\brief 非0 日誌 每次輸出後 將 調用 KG_DEBUG_STREAM.flush() 進行刷新
*/
#ifndef KG_DEBUG_COUT_FLUSH
#define KG_DEBUG_COUT_FLUSH 0
#endif // KG_DEBUG_COUT_FLUSH


/**
*	\brief 默認的日誌輸出 加鎖操作 (默認爲空)
*/
#ifndef KG_DEBUG_LOCK
#define KG_DEBUG_LOCK
#endif // KG_DEBUG_LOCK

/**
*	\brief 默認的日誌輸出 解鎖操作 (默認爲空)
*/
#ifndef KG_DEBUG_UNLOCK
#define KG_DEBUG_UNLOCK
#endif // KG_DEBUG_UNLOCK


/**
*	\brief 默認的日誌輸出 槽
*/
#ifndef KG_DEBUG_STREAM
#define KG_DEBUG_STREAM   std::cout
#endif // KG_DEBUG_STREAM

/**
*	\brief 輸出日誌
*
*	\param TAG 日誌tag
*	\param XXX 日誌內容
*/
#define KG_DEBUG_COUT(TAG,XXX) do{\
	KG_DEBUG_LOCK\
	if(KG_DEBUG_SHOW_FILE)KG_DEBUG_STREAM<<__FILE__<<" ";\
	if(KG_DEBUG_SHOW_LINE)KG_DEBUG_STREAM<<"line:"<<__LINE__<<" ";\
	if(KG_DEBUG_SHOW_TAG)KG_DEBUG_STREAM<<TAG<<"\t";\
		KG_DEBUG_STREAM<<XXX<<"\n";\
	if(KG_DEBUG_COUT_FLUSH)\
		KG_DEBUG_STREAM.flush();\
	KG_DEBUG_UNLOCK \
}while(0);

#define _IGONE_KG_DEBUG_ROUTER_COUT(TAG,LV,XXX) switch(LV){\
	case _IGONE_KG_DEBUG_LV_TRACE:KG_DEBUG_COUT(TAG,XXX);break;\
	case _IGONE_KG_DEBUG_LV_DEBUG:KG_DEBUG_COUT(TAG,XXX);break;\
	case _IGONE_KG_DEBUG_LV_INFO:KG_DEBUG_COUT(TAG,XXX);break;\
	case _IGONE_KG_DEBUG_LV_FAULT:KG_DEBUG_COUT(TAG,XXX);break;\
	case _IGONE_KG_DEBUG_LV_ERROR:KG_DEBUG_COUT(TAG,XXX);break;\
}

/**
*	\brief 使用 sprintf 輸出日誌時 緩衝區大小
*/
#ifndef KG_DEBUG_FORMAT_BUFFER
#define KG_DEBUG_FORMAT_BUFFER 1024
#endif // KG_DEBUG_FORMAT_BUFFER

/**
*	\brief 輸出日誌
*
*	\param TAG 日誌tag
*	\param ... 日誌內容 sprintf 參數
*/
#define KG_DEBUG_FORMAT_COUT(TAG,...) {\
	char buf[KG_DEBUG_FORMAT_BUFFER];\
	sprintf(buf,__VA_ARGS__ );\
	KG_DEBUG_COUT(TAG,buf);\
}
#define _IGONE_KG_DEBUG_FORMAT_ROUTER_COUT(TAG,LV,...) switch(LV){\
	case _IGONE_KG_DEBUG_LV_TRACE:KG_DEBUG_FORMAT_COUT(TAG,__VA_ARGS__);break;\
	case _IGONE_KG_DEBUG_LV_DEBUG:KG_DEBUG_FORMAT_COUT(TAG,__VA_ARGS__);break;\
	case _IGONE_KG_DEBUG_LV_INFO:KG_DEBUG_FORMAT_COUT(TAG,__VA_ARGS__);break;\
	case _IGONE_KG_DEBUG_LV_FAULT:KG_DEBUG_FORMAT_COUT(TAG,__VA_ARGS__);break;\
	case _IGONE_KG_DEBUG_LV_ERROR:KG_DEBUG_FORMAT_COUT(TAG,__VA_ARGS__);break;\
}




/**
*	\def KG_TRACE(XXX)
*
*	\brief 在定義了 KG_DEBUG_TRACE 時 使用 << 風格 輸出日誌
*/
/**
*	\def KG_FORMAT_TRACE(...)
*
*	\brief 在定義了 KG_DEBUG_TRACE 時 使用 sprintf 風格 輸出日誌
*/
#ifdef KG_DEBUG_TRACE
#define KG_TRACE(XXX) _IGONE_KG_DEBUG_ROUTER_COUT(KG_DEBUG_TAG_TRACE,_IGONE_KG_DEBUG_LV_TRACE,XXX)
#define KG_FORMAT_TRACE(...) _IGONE_KG_DEBUG_FORMAT_ROUTER_COUT(KG_DEBUG_TAG_TRACE,_IGONE_KG_DEBUG_LV_TRACE,__VA_ARGS__)
#else
#define KG_TRACE(XXX)
#define KG_FORMAT_TRACE(...)
#endif // KG_DEBUG_TRACE

/**
*	\def KG_DEBUG(XXX)
*
*	\brief 在定義了 KG_DEBUG_DEBUG 時 使用 << 風格 輸出日誌
*/
/**
*	\def KG_FORMAT_DEBUG(...)
*
*	\brief 在定義了 KG_DEBUG_DEBUG 時 使用 sprintf 風格 輸出日誌
*/
#ifdef KG_DEBUG_DEBUG
#define KG_DEBUG(XXX) _IGONE_KG_DEBUG_ROUTER_COUT(KG_DEBUG_TAG_DEBUG,_IGONE_KG_DEBUG_LV_DEBUG,XXX)
#define KG_FORMAT_DEBUG(...) _IGONE_KG_DEBUG_FORMAT_ROUTER_COUT(KG_DEBUG_TAG_DEBUG,_IGONE_KG_DEBUG_LV_DEBUG,__VA_ARGS__)
#else
#define KG_DEBUG(XXX)
#define KG_FORMAT_DEBUG(...)
#endif // KG_DEBUG_DEBUG

/**
*	\def KG_INFO(XXX)
*
*	\brief 在定義了 KG_DEBUG_INFO 時 使用 << 風格 輸出日誌
*/
/**
*	\def KG_FORMAT_INFO(...)
*
*	\brief 在定義了 KG_DEBUG_INFO 時 使用 sprintf 風格 輸出日誌
*/
#ifdef KG_DEBUG_INFO
#define KG_INFO(XXX) _IGONE_KG_DEBUG_ROUTER_COUT(KG_DEBUG_TAG_INFO,_IGONE_KG_DEBUG_LV_INFO,XXX)
#define KG_FORMAT_INFO(...) _IGONE_KG_DEBUG_FORMAT_ROUTER_COUT(KG_DEBUG_TAG_INFO,_IGONE_KG_DEBUG_LV_INFO,__VA_ARGS__)
#else
#define KG_INFO(XXX)
#define KG_FORMAT_INFO(...)
#endif // KG_DEBUG_INFO

/**
*	\def KG_FAULT(XXX)
*
*	\brief 在定義了 KG_DEBUG_FAULT 時 使用 << 風格 輸出日誌
*/
/**
*	\def KG_FORMAT_FAULT(...)
*
*	\brief 在定義了 KG_DEBUG_FAULT 時 使用 sprintf 風格 輸出日誌
*/
#ifdef KG_DEBUG_FAULT
#define KG_FAULT(XXX) _IGONE_KG_DEBUG_ROUTER_COUT(KG_DEBUG_TAG_FAULT,_IGONE_KG_DEBUG_LV_FAULT,XXX)
#define KG_FORMAT_FAULT(...) _IGONE_KG_DEBUG_FORMAT_ROUTER_COUT(KG_DEBUG_TAG_FAULT,_IGONE_KG_DEBUG_LV_FAULT,__VA_ARGS__)
#else
#define KG_FAULT(XXX)
#define KG_FORMAT_FAULT(...)
#endif // KG_DEBUG_FAULT

/**
*	\def KG_ERROR(XXX)
*
*	\brief 在定義了 KG_DEBUG_ERROR 時 使用 << 風格 輸出日誌
*/
/**
*	\def KG_FORMAT_ERROR(...)
*
*	\brief 在定義了 KG_DEBUG_ERROR 時 使用 sprintf 風格 輸出日誌
*/
#ifdef KG_DEBUG_ERROR
#define KG_ERROR(XXX) _IGONE_KG_DEBUG_ROUTER_COUT(KG_DEBUG_TAG_ERROR,_IGONE_KG_DEBUG_LV_ERROR,XXX)
#define KG_FORMAT_ERROR(...) _IGONE_KG_DEBUG_FORMAT_ROUTER_COUT(KG_DEBUG_TAG_ERROR,_IGONE_KG_DEBUG_LV_ERROR,__VA_ARGS__)
#else
#define KG_ERROR(XXX)
#define KG_FORMAT_ERROR(...)
#endif // KG_DEBUG_ERROR





/**
*	\def KG_TRACE_SECTION(XXX)
*
*	\brief 定義一段代碼 使之只在定義了 KG_DEBUG_TRACE 時生效
*/
#ifdef KG_DEBUG_TRACE
#define KG_TRACE_SECTION(XXX) do{XXX}while(0);
#else
#define KG_TRACE_SECTION(XXX)
#endif // KG_DEBUG_TRACE

/**
*	\def KG_DEBUG_SECTION(XXX)
*
*	\brief 定義一段代碼 使之只在定義了 KG_DEBUG_DEBUG 時生效
*/
#ifdef KG_DEBUG_DEBUG
#define KG_DEBUG_SECTION(XXX) do{XXX}while(0);
#else
#define KG_DEBUG_SECTION(XXX)
#endif // KG_DEBUG_DEBUG

/**
*	\def KG_INFO_SECTION(XXX)
*
*	\brief 定義一段代碼 使之只在定義了 KG_DEBUG_INFO 時生效
*/
#ifdef KG_DEBUG_INFO
#define KG_INFO_SECTION(XXX) do{XXX}while(0);
#else
#define KG_INFO_SECTION(XXX)
#endif // KG_DEBUG_INFO

/**
*	\def KG_FAULT_SECTION(XXX)
*
*	\brief 定義一段代碼 使之只在定義了 KG_DEBUG_FAULT 時生效
*/
#ifdef KG_DEBUG_FAULT
#define KG_FAULT_SECTION(XXX) do{XXX}while(0);
#else
#define KG_FAULT_SECTION(XXX)
#endif // KG_DEBUG_FAULT

/**
*	\def KG_ERROR_SECTION(XXX)
*
*	\brief 定義一段代碼 使之只在定義了 KG_DEBUG_ERROR 時生效
*/
#ifdef KG_DEBUG_ERROR
#define KG_ERROR_SECTION(XXX) do{XXX}while(0);
#else
#define KG_ERROR_SECTION(XXX)
#endif // KG_DEBUG_ERROR


/**
*	\def KG_DEBUG_X_SECTION(XXX)
*
*	\brief 定義一段代碼 使之只在定義了 KG_DEBUG_X 時生效
*/
#ifdef KG_DEBUG_X
#define KG_DEBUG_X_SECTION(XXX) do{XXX}while(0);
#else
#define KG_DEBUG_X_SECTION(XXX)
#endif // KG_DEBUG_X


#endif // KG_LIB_HEADER_DEBUG
