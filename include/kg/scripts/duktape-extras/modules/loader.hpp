#ifndef KG_SCRIPTS_EXTRAS_MODULES_LOADER_HEADER_HPP
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
		class loader_base_t
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
			*	\brief 返回 模塊 路徑\n
			*
			*	\param lib	當前搜索的 庫 根目錄
			*	\param pkg	要加載的 模塊 名
			*	\return		模塊路徑
			*/
			virtual const char* module_path(const char* lib,const char* pkg) = 0;
			
			/**
			*	\brief 加載模塊\n
			*
			*/
			virtual bool module_impot(const char* path) = 0;
			
		};
	};
	};
	};
};
#endif // KG_SCRIPTS_EXTRAS_MODULES_LOADER_HEADER_HPP
