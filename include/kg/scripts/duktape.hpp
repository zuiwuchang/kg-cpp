#ifndef KG_SCRIPTS_DUKTAPE_HEADER_HPP
#include "duktape/duktape.h"

#include <boost/smart_ptr.hpp>

namespace kg
{
	namespace scripts
	{
		/**
		*	\brief duktape 的封裝
		*
		*	封裝了 duktape 環境 的智能指針\n
		*	api 基本同 duktape\n
		*	http://duktape.org/api.html
		*
		*	\code 使用示例
		*	...
		*	\endcode
		*/
		class duktape_t
		{
		private:
			/**
			*	\brief 保存duk 運行環境
			*/
			boost::shared_ptr<duk_context> _ctx;
		public:
			/*		duk 環境		*/
			/**
			*	\brief duk 環境 銷毀函數
			*/
			typedef void (*destroy_ft)(duk_context* ctx);
			/**
			*	\brief duk 默認的 銷毀函數
			*/
			static void destroy_ctx(duk_context* ctx)
			{
				duk_destroy_heap(ctx);
			}
			/**
			*	\brief	爲空的duk銷毀 函數
			*
			*	\note	實現爲空 以便 在擴展 duk函數時 可以將 duk 環境包裝成 duktape_t
			*/
			static void destroy_none(duk_context* ctx)
			{
			}


			/**
			*	\brief 創建一個默認的 duk 運行環境
			*	\exception std::bad_alloc
			*/
			duktape_t()
			{
				duk_context* ctx = duk_create_heap_default();
				if(!ctx)
				{
					throw std::bad_alloc();
				}

				_ctx = boost::shared_ptr<duk_context>(ctx,destroy_ctx);
			}
			/**
			*	\brief	將 duk 指針 包裝 成智能指針
			*	\exception	std::bad_alloc
			*	\param ctx	duk 運行環境
			*	\param func	定義了 如何銷毀 duk 的回調
			*/
			duktape_t(duk_context* ctx,destroy_ft func = destroy_none)
			{
				_ctx = boost::shared_ptr<duk_context>(ctx,destroy_ctx);
			}

			/**
			*	\brief copy 構造
			*/
			duktape_t(const duktape_t& copy)
			{
				_ctx = copy._ctx;
			}
			/**
			*	\brief copy
			*/
			duktape_t& operator=(const duktape_t& copy)
			{
				_ctx = copy._ctx;
				return *this;
			}

			/**
			*	\brief 返回 duk 環境 原始指針
			*/
			inline duk_context* get()const
			{
				return _ctx.get();
			}

			/**
			*	\brief 返回 duk 環境 原始指針
			*/
			inline operator duk_context*()const
			{
				return _ctx.get();
			}

			/*		duk 調試		*/
			/**
			*	\brief 打印當前 棧 到 stdcout
			*/
			void dump_context_stdout()
			{
				duk_context* ctx = _ctx.get();
				duk_push_context_dump(ctx);
				fprintf(stdout, "%s\n", duk_safe_to_string(ctx, -1));
				duk_pop(ctx);
			}
			/**
			*	\brief 打印當前 棧 到 stderr
			*/
			void dump_context_stderr()
			{
				duk_context* ctx = _ctx.get();
				duk_push_context_dump(ctx);
				fprintf(stderr, "%s\n", duk_safe_to_string(ctx, -1));
				duk_pop(ctx);
			}

			/*		棧操作		*/
			/**
			*	\brief 彈出棧頂元素
			*/
			inline void pop()
			{
				duk_pop(_ctx.get());
			}
			/**
			*	\brief 從棧頂 彈出 2 個 元素
			*/
			inline void pop_2()
			{
				duk_pop_2(_ctx.get());
			}
			/**
			*	\brief 從棧頂 彈出 3 個 元素
			*/
			inline void pop_3()
			{
				duk_pop_3(_ctx.get());
			}
			/**
			*	\brief 從棧頂 彈出 n 個 元素
			*/
			inline void pop_n(std::size_t n)
			{
				duk_pop_n(_ctx.get(),n);
			}

			/**
			*	\brief 入棧一個 double
			*/
			inline void push(double n)
			{
				duk_push_number(_ctx.get(),(duk_double_t)n);
			}
			/**
			*	\brief 入棧一個 float
			*/
			inline void push(float n)
			{
				duk_push_number(_ctx.get(),(duk_double_t)n);
			}
			/**
			*	\brief 入棧一個 int
			*/
			inline void push(int n)
			{
				duk_push_number(_ctx.get(),(duk_double_t)n);
			}
			/**
			*	\brief 入棧一個 std::size_t
			*/
			inline void push(std::size_t n)
			{
				duk_push_number(_ctx.get(),(duk_double_t)n);
			}

			/**
			*	\brief		入棧一個 bool
			*/
			inline void push_bool(bool ok)
			{
				if(ok)
				{
					duk_push_true(_ctx.get());
				}
				else
				{
					duk_push_false(_ctx.get());
				}
			}

			/**
			*	\brief 入棧一個 utf8 字符串
			*	\param str 要入棧的 字符串 可以包含 0
			*/
			inline void push(const std::string& str)
			{
				duk_push_lstring(_ctx.get(),str.data(),str.size());
			}
			/**
			*	\brief		入棧一個 utf8 字符串
			*
			*	\param str	要入棧的 字符串 可以包含 0
			*	\param n	字符串長度
			*
			*	\attention	如果 str 爲 nullptr 將忽略 n 直接入棧 空字符串
			*/
			inline void push(const char* str,std::size_t n)
			{
				duk_push_lstring(_ctx.get(),str,n);
			}

			/**
			*	\brief		入棧一個 utf8 的 c 字符串
			*
			*	\param str	要入棧的 字符串 以 0 結尾
			*
			*	\attention	如果 str 爲 "" 將 push 空字符串 \n
			*				如果 str 爲 nullptr 將 push null
			*/
			inline void push_cstring(const char* str)
			{
				duk_push_string(_ctx.get(),str);
			}

			/**
			*	\brief		入棧一個 NaN
			*
			*/
			inline void push_nan()
			{
				duk_push_nan(_ctx.get());
			}
			/**
			*	\brief		入棧一個 null
			*
			*/
			inline void push_null()
			{
				duk_push_null(_ctx.get());
			}
			/**
			*	\brief		入棧一個 undefined
			*
			*/
			inline void push_undefined()
			{
				duk_push_undefined(_ctx.get());
			}

			/**
			*	\brief 將全局變量入棧
			*/
			inline void push_global_object()
			{
				duk_push_global_object(_ctx.get());
			}

			/**
			*	\brief	將當前激活的 棧 格式化爲字符串 入棧
			*
			*	\note   這對調試相當有用 比如 dump_context_stdout 便是依此實現的
			*	\warning	不要在生產環境調用此 函數
			*/
			inline void push_context_dump()
			{
				duk_push_context_dump(_ctx.get());
			}

			/**
			*	\brief	入棧一個 c 函數 以供 js 調用
			*
			*	\param func		函數地址
			*	\param nargs	函數期待的 參數 數量\n
			*					nargs >= 0 多餘的參數 忽略 少的 使用 undefined
			*					nargs == DUK_VARARGS 參數數量 由實際調用時 決定
			*	\return			返回函數在 棧中的 非負數 索引
			*
			*/
			inline duk_idx_t push_c_function(duk_c_function func, duk_idx_t nargs)
			{
				return duk_push_c_function(_ctx.get(),func,nargs);
			}

			/**
			*	\brief	創建一個空的 數組 並入棧
			*
			*	\return	返回數組在 棧中的 非負數 索引
			*/
			inline duk_idx_t push_array()
			{
				return duk_push_array(_ctx.get());
			}

			/**
			*	\brief	創建一個空的 object 並入棧
			*
			*	\return	返回object在 棧中的 非負數 索引
			*/
			inline duk_idx_t push_object()
			{
				return duk_push_object(_ctx.get());
			}

			/**
			*	\brief	返回當前 棧 中元素 數量
			*/
			inline duk_idx_t get_top()
			{
				return duk_get_top(_ctx.get());
			}
			/**
			*	\brief	返回當前 棧頂元素 索引 或 DUK_INVALID_INDEX
			*/
			inline duk_idx_t get_top_index()
			{
				return duk_get_top_index(_ctx.get());
			}

			/**
			*	\brief	返回當前 棧 是否能容納 額外的 extra 個元素
			*/
			inline bool check_stack(duk_idx_t extra)
			{
				return duk_check_stack(_ctx.get(),extra) != 0;
			}

			/**
			*	\brief	類似 check_stack 但如果 棧不夠 將 自動擴容 並返回 擴容是否成功
			*/
			inline bool check_stack_top(duk_idx_t extra)
			{
				return duk_check_stack_top(_ctx.get(),extra) != 0;
			}

			/*		is		*/
			/**
			*	\brief	返回棧 中指定元素 是否是 number
			*/
			inline bool is_number(duk_idx_t idx)
			{
				return duk_is_number(_ctx.get(),idx) != 0;
			}
			/**
			*	\brief	返回棧 中指定元素 是否是 bool
			*/
			inline bool is_bool(duk_idx_t idx)
			{
				return duk_is_boolean(_ctx.get(),idx) != 0;
			}
			/**
			*	\brief	返回棧 中指定元素 是否是 string
			*/
			inline bool is_string(duk_idx_t idx)
			{
				return duk_is_string(_ctx.get(),idx) != 0;
			}
			/**
			*	\brief	返回棧 中指定元素 是否是 NaN
			*/
			inline bool is_nan(duk_idx_t idx)
			{
				return duk_is_nan(_ctx.get(),idx) != 0;
			}
			/**
			*	\brief	返回棧 中指定元素 是否是 null
			*/
			inline bool is_null(duk_idx_t idx)
			{
				return duk_is_null(_ctx.get(),idx) != 0;
			}
			/**
			*	\brief	返回棧 中指定元素 是否是 undefined
			*/
			inline bool is_undefined(duk_idx_t idx)
			{
				return duk_is_undefined(_ctx.get(),idx) != 0;
			}
			/**
			*	\brief	返回棧 中指定元素 是否是 null or undefined
			*/
			inline bool is_null_or_undefined(duk_idx_t idx)
			{
				return duk_is_null_or_undefined(_ctx.get(),idx) != 0;
			}
			/**
			*	\brief	返回棧 中指定元素 是否是 array
			*/
			inline bool is_array(duk_idx_t idx)
			{
				return duk_is_array(_ctx.get(),idx) != 0;
			}
			/**
			*	\brief	返回棧 中指定元素 是否是 object
			*
			*	\attention	對於下列 數據 is_object 都會返回 true\n
			*	Ecmascript object\n
			*	Ecmascript array\n
			*	Ecmascript function\n
			*	Duktape thread (coroutine)\n
			*	Duktape internal objects\n
			*/
			inline bool is_object(duk_idx_t idx)
			{
				return duk_is_object(_ctx.get(),idx) != 0;
			}
			/**
			*	\brief	返回棧 中指定元素 是否是 c_function
			*/
			inline bool is_c_function(duk_idx_t idx)
			{
				return duk_is_c_function(_ctx.get(),idx) != 0;
			}

			/*		to		*/
			/**
			*	\brief	to int
			*/
			inline duk_int_t to_int(duk_idx_t idx)
			{
				return duk_to_int(_ctx.get(),idx);
			}
			/**
			*	\brief	to int32
			*/
			inline duk_int32_t to_int32(duk_idx_t idx)
			{
				return duk_to_int32(_ctx.get(),idx);
			}
			/**
			*	\brief	to double
			*/
			inline duk_double_t to_number(duk_idx_t idx)
			{
				return duk_to_number(_ctx.get(),idx);
			}
			/**
			*	\brief	to uint16
			*/
			inline duk_uint16_t to_uint16(duk_idx_t idx)
			{
				return duk_to_uint16(_ctx.get(),idx);
			}
			/**
			*	\brief	to uint32
			*/
			inline duk_uint32_t to_uint32(duk_idx_t idx)
			{
				return duk_to_uint32(_ctx.get(),idx);
			}
			/**
			*	\brief	to bool
			*/
			inline bool to_bool(duk_idx_t idx)
			{
				return duk_to_boolean(_ctx.get(),idx) !=0;
			}
			/**
			*	\brief	返回 以 0 結尾的 c utf8字符串
			*
			*/
			inline const char * to_c_string(duk_idx_t idx)
			{
				return duk_to_string(_ctx.get(),idx);
			}
			/**
			*	\brief	返回 utf8 字符串
			*
			*/
			inline const char * to_string(duk_idx_t idx,duk_size_t *out_len)
			{
				return duk_to_lstring(_ctx.get(),idx,out_len);
			}
			/**
			*	\brief	返回 utf8 字符串
			*
			*/
			inline const std::string to_string(duk_idx_t idx)
			{
				duk_size_t len;
				const char* str = duk_to_lstring(_ctx.get(),idx,&len);
				return std::string(str,len);
			}
			/**
			*	\brief	返回 utf8 字符串
			*
			*/
			inline void to_string(duk_idx_t idx,std::string& out)
			{
				duk_size_t len;
				const char* str = duk_to_lstring(_ctx.get(),idx,&len);
				out = std::string(str,len);
			}

			/**
			*	\brief	返回 以 0 結尾的 c utf8字符串
			*
			*/
			inline const char * safe_to_c_string(duk_idx_t idx)
			{
				return duk_safe_to_string(_ctx.get(),idx);
			}
			/**
			*	\brief	返回 utf8 字符串
			*
			*/
			inline const char * safe_to_string(duk_idx_t idx,duk_size_t *out_len)
			{
				return duk_safe_to_lstring(_ctx.get(),idx,out_len);
			}
			/**
			*	\brief	返回 utf8 字符串
			*
			*/
			inline const std::string safe_to_string(duk_idx_t idx)
			{
				duk_size_t len;
				const char* str = duk_safe_to_lstring(_ctx.get(),idx,&len);
				return std::string(str,len);
			}
			/**
			*	\brief	返回 utf8 字符串
			*
			*/
			inline void safe_to_string(duk_idx_t idx,std::string& out)
			{
				duk_size_t len;
				const char* str = duk_safe_to_lstring(_ctx.get(),idx,&len);
				out = std::string(str,len);
			}


			/*		array		*/

			/*		object		*/

			/*		call		*/

			/*		eval		*/
		};
	};
};
#endif // KG_SCRIPTS_DUKTAPE_HEADER_HPP
