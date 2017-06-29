#ifndef KG_SCRIPTS_DUKTAPE_EXTRAS_MODULES_JS_HEADER_HPP
#define KG_SCRIPTS_DUKTAPE_EXTRAS_MODULES_JS_HEADER_HPP
#include "loader.hpp"

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
				//delete (js_loader_t*)p;
			}

			/**
			*	\brief 返回 能夠加載的 模塊 類型\n
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
			*
			*	\note	如返回 KG_DUK_JS_PATH=/lib/kg-duk:/usr/lib/kg-duk:. 在在下面三個目錄中 查找模塊
			*			/lib/kg-duk
			*			/usr/lib/kg-duk
			*			.
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
				return lib + "/" + pkg + ".js";
			}

			/**
			*	\brief 加載模塊\n
			*
			*/
			virtual bool module_import(const std::string& pkg,const std::string& path)
			{
				return true;
			}

		};
	};
	};
	};
};
#endif // KG_SCRIPTS_DUKTAPE_EXTRAS_MODULES_JS_HEADER_HPP
