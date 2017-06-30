#ifndef KG_SCRIPTS_DUKTAPE_EXTRAS_MODULES_C_HEADER_HPP
#define KG_SCRIPTS_DUKTAPE_EXTRAS_MODULES_C_HEADER_HPP
#include "loader.hpp"

#include <boost/algorithm/string.hpp>
#include <fstream>

#if defined(_WIN32) || defined(WIN32) || defined(_WIN64) || defined(WIN64) || defined(__WIN32__) || defined(__TOS_WIN__) || defined(__WINDOWS__)
	#include <windows.h>
#else
	#include <dlfcn.h>
#endif // WIN32

namespace kg
{
	namespace scripts
	{
	namespace duktape
	{
	namespace modules
	{
		/**
		*	\brief c 模塊 加載器\n
		* <pre>
		*	windows
		*		#include <windows.h>
		*	linux
		*		#include <dlfcn.h>
		*		-ldl
		* </pre>
		*/
		class c_loader_t :public loader_i
		{
		public:
			/**
			*	\brief c 模塊導出函數 kg_duk_package 的聲明\n
			*/
			typedef duk_ret_t (*kg_duk_package_ft)(duk_context*);
			/**
			*	\brief 創建加載器指針\n
			*/
			static c_loader_t* create()
			{
				return new c_loader_t();
			}
			/**
			*	\brief 創建釋放 加載器指針\n
			*/
			static void destory(void* p)
			{
				delete (c_loader_t*)p;
			}

			/**
			*	\brief 返回 能夠加載的 模塊 類型\n
			*	return "c";\n
			*	例如\n
			*	 返回 字符串 js 代表加載 js 模塊\n
			*	 返回 字符串 c 代表加載 c 模塊\n
			*/
			virtual const char* module_type()
			{
				return "c";
			}

			/**
			*	\brief 返回 模塊 查找的環境變量名\n
			*	return "KG_DUK_C_PATH";\n
			*
			*	\note	如返回 KG_DUK_JS_PATH=/lib/kg-c:/usr/lib/kg-c:. 在在下面三個目錄中 查找模塊\n
			*			/lib/kg-c\n
			*			/usr/lib/kg-c\n
			*			.\n
			*
			*	\attention	windows 下使用 ; 其它平臺使用 : 分隔 環境變量值
			*/
			virtual const char* module_env()
			{
				return "KG_DUK_C_PATH";
			}

			/**
			*	\brief 返回 模塊 路徑\n
			*
			*	\param lib	當前搜索的 庫 根目錄
			*	\param pkg	要加載的 模塊 名
			*	\return		模塊路徑
			*/
			virtual std::string module_path(const std::string& lib,const std::string& pkg)
			{
#if defined(_WIN32) || defined(WIN32) || defined(_WIN64) || defined(WIN64) || defined(__WIN32__) || defined(__TOS_WIN__) || defined(__WINDOWS__)
				if(boost::algorithm::ends_with(lib,"/") || boost::algorithm::ends_with(lib,"\\"))
				{
					return lib + pkg + ".dll";
				}
				return lib + "/" + pkg + ".dll";
#else
				if(boost::algorithm::ends_with(lib,"/") || boost::algorithm::ends_with(lib,"\\"))
				{
					return lib + pkg + ".so";
				}
				return lib + "/" + pkg + ".so";
#endif // WIN32

			}

			/**
			*	\brief 加載模塊\n
			*	... -> ... obj			加載成功 將 模塊 入棧\n
			*	... -> ... undefined	加載失敗 將 模塊不存在 入棧\n
			*	... -> ... emsg			加載失敗 將 錯誤描述字符串 入棧\n
			*
			*	\param ctx	duk運行環境
			*	\param path 模塊路徑
			*/
			virtual bool module_import(duk_context* ctx,const std::string& path)
			{

#if defined(_WIN32) || defined(WIN32) || defined(_WIN64) || defined(WIN64) || defined(__WIN32__) || defined(__TOS_WIN__) || defined(__WINDOWS__)
				duk_push_undefined(ctx);
				return false;
#else
				void* handle = dlopen(path.c_str(),RTLD_LAZY);
				if(!handle)
				{
					duk_push_undefined(ctx);
					return false;
				}

				kg_duk_package_ft duk_func = (kg_duk_package_ft)dlsym(handle,"kg_duk_package");
				if(!duk_func)
				{
					dlclose(handle);

					duk_push_string(ctx,(path + " not a module").c_str());
					return false;
				}

				duk_push_c_function(ctx,duk_func,0);
				if(duk_pcall(ctx,0) != DUK_EXEC_SUCCESS)
				{
					return false;
				}

				if(!duk_is_object(ctx,-1))
				{
					return false;
				}

				return true;
#endif // WIN32
			}

		};
	};
	};
	};
};
#endif // KG_SCRIPTS_DUKTAPE_EXTRAS_MODULES_C_HEADER_HPP
