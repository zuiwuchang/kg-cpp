#ifndef KG_SCRIPTS_DUKTAPE_EXTRAS_HEADER_HPP
#include "duktape/duktape.h"
#include "duktape-extras/print_alert.hpp"
#include "duktape-extras/module.hpp"

namespace kg
{
	namespace scripts
	{
		/**
		*	\brief 對 duktape 進行了 擴展
		*
		*/
		class duktape_extras
		{
		private:
			duktape_extras(){}
			~duktape_extras(){}
		public:
			/**
			*	\brief 初始化 所有 默認 擴展
			*
			*	\note	print_alert
			*/
			inline static bool init(duk_context* ctx)
			{
				return duktape::print_alert::init(ctx) &&
					duktape::module::init(ctx)
					;
			}
		};
	};
};
#endif // KG_SCRIPTS_DUKTAPE_EXTRAS_HEADER_HPP
