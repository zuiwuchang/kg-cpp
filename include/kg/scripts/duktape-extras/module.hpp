#ifndef KG_SCRIPTS_DUKTAPE_EXTRAS_MODULE_HEADER_HPP
#define KG_SCRIPTS_DUKTAPE_EXTRAS_MODULE_HEADER_HPP
#include "../duktape/duktape.h"
#include "modules/loader.hpp"
#include "modules/js.hpp"

#include <boost/algorithm/string.hpp>
#include <boost/xpressive/xpressive_dynamic.hpp>

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
		*	module 爲 duk 增加了 kg_import 函數
		*	function kg_import(pkg,type)
		*
		*	kg_import 用於 加載模塊 並返回 模塊 返回的 obj
		*		pkg 模塊名
		*		type 是要加載的模塊類型 如果 type 不是一個合法的 字符串 將 依次嘗試使用 所有的 loader 進行加載
		*
		*
		*	module 同時增加了一個全局 變量 _g_kg_modules 用於保存已經加載的 模塊
		*		不要手動 操作 _g_kg_modules 此處 只是提醒使用者 不要 覆蓋了 全局的 _g_kg_modules
		*
		*	模塊
		*		模塊是 一個 返回值是 object 的 function
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
				{
					duk_push_c_function(ctx,native_destory,0);
					duk_set_finalizer(ctx,-2);
				}
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

				//加載默認 模塊 加載器
				register_loader(ctx,modules::js_loader_t::create(),modules::js_loader_t::destory);

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
				duk_push_global_object(ctx);
				duk_get_prop_string(ctx,-1,"_g_kg_modules");

				const char* key = loader->module_type();
				if(duk_get_prop_string(ctx,-1,key))
				{
					//已經存在 刪除
					duk_get_prop_string(ctx,-1,"destory");
					free_loader_ft f = (free_loader_ft)(duk_get_pointer(ctx,-1));
					duk_pop(ctx);
					if(f)
					{
						duk_get_prop_string(ctx,-1,"loader");
						modules::loader_i* loader = (modules::loader_i*)(duk_get_pointer(ctx,-1));
						duk_pop(ctx);
						f(loader);
					}
				}
				duk_pop(ctx);

				duk_push_string(ctx,key);
				duk_push_object(ctx);
				{
					duk_push_string(ctx,"loader");
					duk_push_pointer(ctx,loader);
					duk_put_prop(ctx,-3);

					duk_push_string(ctx,"destory");
					void* p = (void*)fft;
					duk_push_pointer(ctx,p);
					duk_put_prop(ctx,-3);

					duk_push_string(ctx,"libs");
					duk_push_object(ctx);
					duk_put_prop(ctx,-3);
				}
				duk_put_prop(ctx,-3);

				duk_pop_2(ctx);
				return true;
			}
		private:
			static duk_ret_t native_destory(duk_context *ctx)
			{
				duk_push_global_object(ctx);

				duk_get_prop_string(ctx,-1,"_g_kg_modules");
				duk_enum(ctx,-1,DUK_ENUM_OWN_PROPERTIES_ONLY);
				while(duk_next(ctx, -1, 1))
				{
					duk_get_prop_string(ctx,-1,"destory");
					free_loader_ft f = (free_loader_ft)(duk_get_pointer(ctx,-1));
					duk_pop(ctx);
					if(f)
					{
						duk_get_prop_string(ctx,-1,"loader");
						modules::loader_i* loader = (modules::loader_i*)(duk_get_pointer(ctx,-1));
						duk_pop(ctx);
						f(loader);
					}

					duk_pop_2(ctx);
				}
				duk_pop_2(ctx);

				duk_del_prop_string(ctx,-1,"_g_kg_modules");
				duk_pop(ctx);
				return 0;
			}
			static duk_ret_t native_package(duk_context *ctx)
			{
				/*
				duk_push_context_dump(ctx);
				fprintf(stdout, "%s\n", duk_safe_to_string(ctx, -1));
				duk_pop(ctx);
				*/
				return 0;
			}
			static duk_ret_t native_import(duk_context *ctx)
			{
				if(!duk_is_string(ctx,0))
				{
					return 0;
				}

				//module_type
				const char* key = NULL;
				if(duk_is_string(ctx,1))
				{
					key = duk_get_string(ctx,1);
				}

				//package
				const char* pkg = duk_get_string(ctx,0);
				duk_pop_2(ctx);

				if(!duk_get_global_string(ctx,"_g_kg_modules"))
				{
					return 0;
				}
				if(key)
				{
					//js loader obj
					if(!duk_get_prop_string(ctx,-1,key))
					{
						return 0;
					}

					//libs
					if(!duk_get_prop_string(ctx,-1,"libs"))
					{
						return 0;
					}
					if(duk_get_prop_string(ctx,-1,pkg))
					{
						//庫已經 被加載 直接返回
						return 1;
					}
					duk_pop(ctx);

					//loader 指針
					if(!duk_get_prop_string(ctx,-2,"loader"))
					{
						return 0;
					}
					modules::loader_i* loader = (modules::loader_i*)duk_get_pointer(ctx,-1);
					if(!loader)
					{
						return 0;
					}
					duk_pop(ctx);

					return import_pkg(ctx,loader,pkg);
				}
				else
				{
					duk_enum(ctx,-1,DUK_ENUM_OWN_PROPERTIES_ONLY);

					while (duk_next(ctx, -1 , 1 ))
					{

						//libs
						if(!duk_get_prop_string(ctx,-1,"libs"))
						{
							duk_pop_3(ctx);
							continue;
						}

						if(duk_get_prop_string(ctx,-1,pkg))
						{
							//庫已經 被加載 直接返回
							return 1;
						}
						duk_pop(ctx);

						//loader 指針
						if(!duk_get_prop_string(ctx,-2,"loader"))
						{
							duk_pop_n(ctx,4);
							continue;
						}

						modules::loader_i* loader = (modules::loader_i*)duk_get_pointer(ctx,-1);
						if(!loader)
						{
							duk_pop_n(ctx,4);
							continue;
						}
						duk_pop(ctx);

						if(import_pkg(ctx,loader,pkg))
						{
							return 1;
						}

						duk_pop_n(ctx,4);
					}
					duk_pop(ctx);
				}
				duk_pop(ctx);
				return 0;
			}

			/**
			*	... loader libs -> ... loader libs
			*/
			static duk_ret_t import_pkg(duk_context *ctx,modules::loader_i* loader,const std::string pkg)
			{
				std::string env = getenv(loader->module_env());
				boost::trim(env);
#ifdef WIN32
				const char* flag = ";";
#else
				const char* flag = ":";
#endif // WIN32
				boost::xpressive::sregex reg = boost::xpressive::sregex::compile(flag);
				boost::xpressive::sregex_token_iterator iterator(env.begin(),env.end(),reg,-1);
				boost::xpressive::sregex_token_iterator end;
				while(iterator != end)
				{
					std::string lib = *iterator;
					boost::trim(lib);
					if(!lib.empty())
					{
						std::string path = loader->module_path(lib,pkg);
						if(loader->module_import(ctx,path))
						{
							//創建 模塊副本
							duk_dup_top(ctx);

							//緩存 模塊
							duk_put_prop_lstring(ctx,-3,pkg.data(),pkg.size());

							//返回 模塊
							return 1;
						}

						if(!duk_is_undefined(ctx,-1))
						{
							return 1;
						}
						duk_pop(ctx);
					}
					++iterator;
				}

				return 0;
			}
		};
	};
	};
};
#endif // KG_SCRIPTS_DUKTAPE_EXTRAS_MODULE_HEADER_HPP