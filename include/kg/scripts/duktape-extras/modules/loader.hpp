#ifndef KG_SCRIPTS_DUKTAPE_EXTRAS_MODULES_LOADER_HEADER_HPP
#define KG_SCRIPTS_DUKTAPE_EXTRAS_MODULES_LOADER_HEADER_HPP
#include "../../duktape/duktape.h"

namespace kg
{
	namespace scripts
	{

	/**
	*	\brief 對 duktape 擴展的實現代碼
	*
	*/
	namespace duktape
	{

	/**
	*	\brief 默認實現的 duk 模塊 架構
	*
	*/
	namespace modules
	{
		/**
		*	\brief 模塊 加載器 接口
		*
		*/
		class loader_i
		{
		public:
			/**
			*	\brief 返回 能夠加載的 模塊 類型\n
			*	例如\n
			*	 返回 字符串 js 代表加載 js 模塊\n
			*	 返回 字符串 c 代表加載 c 模塊\n
			*/
			virtual const char* module_type() = 0;

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
			virtual const char* module_env() = 0;

			/**
			*	\brief 返回 模塊 路徑\n
			*
			*	\param lib	當前搜索的 庫 根目錄
			*	\param pkg	要加載的 模塊 名
			*	\return		模塊路徑
			*/
			virtual std::string module_path(const std::string& lib,const std::string& pkg) = 0;

			/**
			*	\brief 加載模塊\n
			*
			*	\param pkg 模塊名
			*	\param path 模塊路徑
			*/
			virtual bool module_import(const std::string& pkg,const std::string& path) = 0;

		};
	};
	};
	};
};
#endif // KG_SCRIPTS_DUKTAPE_EXTRAS_MODULES_LOADER_HEADER_HPP
