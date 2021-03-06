#ifndef KG_SCRIPTS_DUKTAPE_EXTRAS_MODULES_JS_HEADER_HPP
#define KG_SCRIPTS_DUKTAPE_EXTRAS_MODULES_JS_HEADER_HPP
#include "loader.hpp"

#include <boost/algorithm/string.hpp>


#ifndef KG_SCRIPTS_DUKTAPE_MAX_SIZE
#define KG_SCRIPTS_DUKTAPE_MAX_SIZE 1024 * 1024 * 64
#endif // KG_SCRIPTS_DUKTAPE_MAX_SIZE

namespace kg
{
	namespace scripts
	{
	namespace duktape
	{
	namespace modules
	{
		/**
		*	\brief js 模塊 加載器
		*
		*/
		class js_loader_t :public loader_i
		{
		public:
			/**
			*	\brief 創建加載器指針\n
			*/
			static js_loader_t* create()
			{
				return new js_loader_t();
			}
			/**
			*	\brief 創建釋放 加載器指針\n
			*/
			static void destory(void* p)
			{
				delete (js_loader_t*)p;
			}

			/**
			*	\brief 返回 能夠加載的 模塊 類型\n
			*	return "js";\n
			*	例如\n
			*	 返回 字符串 js 代表加載 js 模塊\n
			*	 返回 字符串 c 代表加載 c 模塊\n
			*/
			virtual const char* module_type()
			{
				return "js";
			}

			/**
			*	\brief 返回 模塊 查找的環境變量名\n
			*	return "KG_DUK_JS_PATH";\n
			*
			*	\note	如返回 KG_DUK_JS_PATH=/lib/kg-duk:/usr/lib/kg-duk:. 在在下面三個目錄中 查找模塊\n
			*			/lib/kg-duk\n
			*			/usr/lib/kg-duk\n
			*			.\n
			*
			*	\attention	windows 下使用 ; 其它平臺使用 : 分隔 環境變量值
			*/
			virtual const char* module_env()
			{
				return "KG_DUK_JS_PATH";
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
				if(boost::algorithm::ends_with(lib,"/") || boost::algorithm::ends_with(lib,"\\"))
				{
					return lib + pkg + ".js";
				}
				return lib + "/" + pkg + ".js";
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
				std::ifstream inf(path,std::ios::in | std::ios::binary);
				if(!inf.is_open())
				{
					duk_push_undefined(ctx);
					return false;
				}

				inf.seekg(0,std::ios::end);
				std::istream::pos_type size = inf.tellg();
				if(size > KG_SCRIPTS_DUKTAPE_MAX_SIZE)
				{
					duk_push_string(ctx,"module size must small than KG_SCRIPTS_DUKTAPE_MAX_SIZE");
					return false;
				}

				inf.seekg(0,std::ios::beg);
				char* buf = (char*)malloc(size);
				if(!buf)
				{
					duk_push_string(ctx,"malloc error");
					return false;
				}
				inf.read(buf,size);
				if( duk_peval_lstring(ctx,buf,size) != 0)
				{
					free(buf);
					return false;
				}
				free(buf);

				if(duk_is_c_function(ctx,-1))
				{
					duk_pop(ctx);
					duk_push_string(ctx,(path + " not a module").c_str());
					return false;
				}
				if(duk_pcall(ctx,0) != DUK_EXEC_SUCCESS)
				{
					return false;
				}

				if(!duk_is_object(ctx,-1))
				{
					return false;
				}
				return true;
			}

		};
	};
	};
	};
};
#endif // KG_SCRIPTS_DUKTAPE_EXTRAS_MODULES_JS_HEADER_HPP
