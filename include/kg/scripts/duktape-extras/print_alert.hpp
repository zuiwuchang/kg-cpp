#ifndef KG_SCRIPTS_EXTRAS_PRINT_ALERT_HEADER_HPP
#include "../duktape/duktape.h"

namespace kg
{
	namespace scripts
	{
	namespace duktape
	{
		/**
		*	\brief 爲 duktape 的js 增加了 全局的 alert print 函數
		*
		*/
		class print_alert
		{
		private:
			print_alert(){}
			~print_alert(){}
		public:
			/**
			*	\brief 初始化 擴展
			*
			*/
			inline static bool init(duk_context* ctx)
			{
				duk_push_global_object(ctx);

				if(!duk_has_prop_string(ctx,-1,"alert"))
				{
					duk_push_c_function(ctx, native_alert,DUK_VARARGS);
					duk_put_prop_string(ctx, -2, "alert");
				}

				if(!duk_has_prop_string(ctx,-1,"print"))
				{
					duk_push_c_function(ctx, native_alert,DUK_VARARGS);
					duk_put_prop_string(ctx, -2, "print");
				}

				duk_pop(ctx);
				return true;
			}
		private:
			static duk_ret_t native_alert(duk_context *ctx)
			{
				duk_idx_t n = duk_get_top(ctx);
				for(duk_idx_t i=0;i<n;++i)
				{
					duk_dup(ctx,i);
					if(i)
					{
						std::cout<<" ";
					}
					std::cout<<duk_safe_to_string(ctx,-1);
					duk_pop(ctx);
				}
				std::cout<<std::endl;
				return 0;
			}
		};
	};
	};
};
#endif // KG_SCRIPTS_EXTRAS_PRINT_ALERT_HEADER_HPP
