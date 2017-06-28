#ifndef KG_SCRIPTS_EXTRAS_MODULE_HEADER_HPP
#include "../duktape/duktape.h"

namespace kg
{
	namespace scripts
	{
	namespace duktape
	{
		/**
		*	\brief 爲 duktape 提供了一種 模塊 框架
		*
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
			inline static bool init(duk_context* ctx)
			{
				
				return true;
			}
		};
	};
	};
};
#endif // KG_SCRIPTS_EXTRAS_MODULE_HEADER_HPP
