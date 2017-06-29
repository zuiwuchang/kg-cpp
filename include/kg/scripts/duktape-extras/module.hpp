#ifndef KG_SCRIPTS_DUKTAPE_EXTRAS_MODULE_HEADER_HPP
#define KG_SCRIPTS_DUKTAPE_EXTRAS_MODULE_HEADER_HPP
#include "../duktape/duktape.h"
#include "modules/loader.hpp"
#include "modules/js.hpp"

namespace kg
{
	namespace scripts
	{
	namespace duktape
	{
		/**
		*	\brief 爲 duktape 提供了一種 模塊 框架
		*
		* <pre>
		*	module 爲 duk 增加了 兩個 js 函數
		*	function kg_package(func)
		*	function kg_import(pkg,type)
		*
		*	kg_package 用於註冊 模塊
		*		func 是一個 返回 object 的 js 函數 負責初始化 模塊 並將模塊功能 設置到 obj 並返回
		*	kg_import 用於 加載模塊 並返回 模塊 返回的 obj
		*		pkg 模塊名
		*		type 是要加載的模塊類型 如果 type 不是一個合法的 字符串 將 依次嘗試使用 所有的 loader 進行加載
		*
		*
		*
		*	module 同時增加了一個全局 變量 _g_kg_modules 用於保存已經加載的 模塊
		*		不要手動 操作 _g_kg_modules 此處 只是提醒使用者 不要 覆蓋了 全局的 _g_kg_modules
		* </pre>
		*/
		class module
		{
		private:
			module(){}
			~module(){}

		public:
			/**
			*	\brief 初始化 擴展
			*
			*/
			static bool init(duk_context* ctx)
			{
				duk_push_global_object(ctx);

				//創建 _g_kg_modules
				duk_push_string(ctx,"_g_kg_modules");
				duk_push_object(ctx);
				duk_put_prop(ctx,-3);

				//創建 package 函數
				duk_push_string(ctx,"kg_package");
				duk_push_c_function(ctx,native_package,1);
				duk_put_prop(ctx,-3);

				//創建 import 函數
				duk_push_string(ctx,"kg_import");
				duk_push_c_function(ctx,native_import,2);
				duk_put_prop(ctx,-3);


				duk_pop(ctx);

				//加載默認模塊 加載器
				//register_loader(ctx,modules::js_loader_t::create(),modules::js_loader_t::destory);

				return true;
			}

			/**
			*	\brief 模塊 加載器 釋放函數定義\n
			*
			*/
			typedef void (*free_loader_ft)(void*);
			/**
			*	\brief 註冊 模塊 加載器 必須在 init 之後 調用\n
			*
			*	\param ctx		duk運行環境
			*	\param loader	模塊加載器
			*	\param fft		加載器 釋放函數
			*
			*	\attention	如果已經存在一個 此類型的 loader 將 覆蓋 並且 不會釋放 原loader的指針\n
			*				合理的 做法是 在 register_loader 前 調用 get_loader 獲取 原 loader
			*/
			static bool register_loader(duk_context* ctx,modules::loader_i* loader,free_loader_ft fft)
			{
				return true;
			}
		private:
			static duk_ret_t native_package(duk_context *ctx)
			{
				return 0;
			}
			static duk_ret_t native_import(duk_context *ctx)
			{
				return 0;
			}
		};
	};
	};
};
#endif // KG_SCRIPTS_DUKTAPE_EXTRAS_MODULE_HEADER_HPP
