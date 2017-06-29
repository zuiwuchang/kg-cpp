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

			/*		global		*/
			/**
			*	\brief 將全局變量入棧\n
			*	... -> ... global\n
			*/
			inline void push_global()
			{
				duk_push_global_object(_ctx.get());
			}

			/**
			*	\brief 將 global[key] 入棧\n
			*	... -> ... val	(if key exists)\n
			*	... -> ... undefined	(if key doesn't exist)\n
			*/
			duk_bool_t get_global(const char *key)
			{
				return 0 != duk_get_global_string(_ctx.get(),key);
			}
			/**
			*	\brief 將 global[key] 入棧\n
			*	... -> ... val	(if key exists)\n
			*	... -> ... undefined	(if key doesn't exist)\n
			*/
			duk_bool_t get_global(const char *key,duk_size_t len)
			{
				return 0 != duk_get_global_lstring(_ctx.get(),key,len);
			}
			/**
			*	\brief 將 global[key] 入棧\n
			*	... -> ... val	(if key exists)\n
			*	... -> ... undefined	(if key doesn't exist)\n
			*/
			duk_bool_t get_global(const std::string& key)
			{
				return 0 != duk_get_global_lstring(_ctx.get(),key.data(),key.size());
			}

			/**
			*	\brief 將 棧頂出棧 並且 設置到 global[key]\n
			*	... val -> ...\n
			*/
			duk_bool_t put_global(const char *key)
			{
				return 0 != duk_put_global_string(_ctx.get(),key);
			}
			/**
			*	\brief 將 棧頂出棧 並且 設置到 global[key]\n
			*	... val -> ...\n
			*/
			duk_bool_t put_global(const char *key,duk_size_t len)
			{
				return 0 != duk_put_global_lstring(_ctx.get(),key,len);
			}
			/**
			*	\brief 將 棧頂出棧 並且 設置到 global[key]\n
			*	... val -> ...\n
			*/
			duk_bool_t put_global(const std::string& key)
			{
				return 0 != duk_put_global_lstring(_ctx.get(),key.data(),key.size());
			}

			/*		棧操作		*/
			/**
			*	\brief 彈出棧頂元素\n
			*	... val -> ...\n
			*/
			inline void pop()
			{
				duk_pop(_ctx.get());
			}
			/**
			*	\brief 從棧頂 彈出 2 個 元素\n
			*	... val1 -> ...\n
			*/
			inline void pop_2()
			{
				duk_pop_2(_ctx.get());
			}
			/**
			*	\brief 從棧頂 彈出 3 個 元素\n
			*	... val1 val2 val3 -> ...\n
			*/
			inline void pop_3()
			{
				duk_pop_3(_ctx.get());
			}
			/**
			*	\brief 從棧頂 彈出 n 個 元素\n
			*	... val1 ... valN -> ...\n
			*/
			inline void pop_n(std::size_t n)
			{
				duk_pop_n(_ctx.get(),n);
			}

			/**
			*	\brief 爲棧中指定數據 創建一個副本 並入棧\n
			*	... val ... -> ... val ... val\n
			*
			*	\attention	對於 obj array 等引用 創建的時 一個引用 同 c++ copy 語義
			*/
			inline void dup(duk_idx_t idx)
			{
				duk_dup(_ctx.get(),idx);
			}
			/**
			*	\brief 爲棧頂元素 創建一個副本 並入棧\n
			*	... val ... -> ... val ... val\n
			*
			*	\attention	對於 obj array 等引用 創建的時 一個引用 同 c++ copy 語義
			*/
			inline void dup_top()
			{
				duk_dup_top(_ctx.get());
			}

			/**
			*	\brief 入棧一個 double\n
			*	... -> ... val\n
			*/
			inline void push(double n)
			{
				duk_push_number(_ctx.get(),(duk_double_t)n);
			}
			/**
			*	\brief 入棧一個 float\n
			*	... -> ... val\n
			*/
			inline void push(float n)
			{
				duk_push_number(_ctx.get(),(duk_double_t)n);
			}
			/**
			*	\brief 入棧一個 int\n
			*	... -> ... val\n
			*/
			inline void push(int n)
			{
				duk_push_number(_ctx.get(),(duk_double_t)n);
			}
			/**
			*	\brief 入棧一個 std::size_t\n
			*	... -> ... val\n
			*/
			inline void push(std::size_t n)
			{
				duk_push_number(_ctx.get(),(duk_double_t)n);
			}

			/**
			*	\brief		入棧一個 bool\n
			*	... -> ... bool\n
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
			*	\brief 入棧一個 utf8 字符串\n
			*	... -> ... str\n
			*
			*	\param str 要入棧的 字符串 可以包含 0
			*/
			inline void push(const std::string& str)
			{
				duk_push_lstring(_ctx.get(),str.data(),str.size());
			}
			/**
			*	\brief		入棧一個 utf8 字符串\n
			*	... -> ... str\n
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
			*	\brief		入棧一個 utf8 的 c 字符串\n
			*	... -> ... str (if str != NULL)\n
			*	... -> ... null (if str == NULL)\n
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
			*	\brief		入棧一個 NaN\n
			*	... -> ... NaN\n
			*
			*/
			inline void push_nan()
			{
				duk_push_nan(_ctx.get());
			}
			/**
			*	\brief		入棧一個 null\n
			*	... -> ... null\n
			*
			*/
			inline void push_null()
			{
				duk_push_null(_ctx.get());
			}
			/**
			*	\brief		入棧一個 undefined\n
			*	... -> ... undefined\n
			*
			*/
			inline void push_undefined()
			{
				duk_push_undefined(_ctx.get());
			}

			/**
			*	\brief		入棧一個 c 指針\n
			*	... -> ... ptr\n
			*
			*/
			inline void push_pointer(void *p)
			{
				duk_push_pointer(_ctx.get(),p);
			}

			/**
			*	\brief	將當前激活的 棧 格式化爲字符串 入棧\n
			*	... -> ... str\n
			*
			*	\note   這對調試相當有用 比如 dump_context_stdout 便是依此實現的
			*	\warning	不要在生產環境調用此 函數
			*/
			inline void push_context_dump()
			{
				duk_push_context_dump(_ctx.get());
			}

			/**
			*	\brief	入棧一個 c 函數 以供 js 調用\n
			*	... -> ... func\n
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
			*	\brief	創建一個空的 數組 並入棧\n
			*	... -> ... arr\n
			*
			*	\return	返回數組在 棧中的 非負數 索引
			*/
			inline duk_idx_t push_array()
			{
				return duk_push_array(_ctx.get());
			}

			/**
			*	\brief	創建一個空的 object 並入棧\n
			*	... -> ... obj\n
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
			*	\brief	返回棧 中指定元素 是否是 number\n
			*	... val ...\n
			*/
			inline bool is_number(duk_idx_t idx)
			{
				return duk_is_number(_ctx.get(),idx) != 0;
			}
			/**
			*	\brief	返回棧 中指定元素 是否是 bool\n
			*	... val ...\n
			*/
			inline bool is_bool(duk_idx_t idx)
			{
				return duk_is_boolean(_ctx.get(),idx) != 0;
			}
			/**
			*	\brief	返回棧 中指定元素 是否是 string\n
			*	... val ...\n
			*/
			inline bool is_string(duk_idx_t idx)
			{
				return duk_is_string(_ctx.get(),idx) != 0;
			}
			/**
			*	\brief	返回棧 中指定元素 是否是 NaN\n
			*	... val ...\n
			*/
			inline bool is_nan(duk_idx_t idx)
			{
				return duk_is_nan(_ctx.get(),idx) != 0;
			}
			/**
			*	\brief	返回棧 中指定元素 是否是 null\n
			*	... val ...\n
			*/
			inline bool is_null(duk_idx_t idx)
			{
				return duk_is_null(_ctx.get(),idx) != 0;
			}
			/**
			*	\brief	返回棧 中指定元素 是否是 undefined\n
			*	... val ...\n
			*/
			inline bool is_undefined(duk_idx_t idx)
			{
				return duk_is_undefined(_ctx.get(),idx) != 0;
			}
			/**
			*	\brief	返回棧 中指定元素 是否是 null or undefined\n
			*	... val ...\n
			*/
			inline bool is_null_or_undefined(duk_idx_t idx)
			{
				return duk_is_null_or_undefined(_ctx.get(),idx) != 0;
			}
			/**
			*	\brief	返回棧 中指定元素 是否是 c 指針\n
			*	... val ...\n
			*/
			inline bool is_pointer(duk_idx_t idx)
			{
				return duk_is_pointer(_ctx.get(),idx) != 0;
			}
			/**
			*	\brief	返回棧 中指定元素 是否是 array\n
			*	... val ...\n
			*/
			inline bool is_array(duk_idx_t idx)
			{
				return duk_is_array(_ctx.get(),idx) != 0;
			}
			/**
			*	\brief	返回棧 中指定元素 是否是 object\n
			*	... val ...\n
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
			*	\brief	返回棧 中指定元素 是否是 c_function\n
			*	... val ...\n
			*/
			inline bool is_c_function(duk_idx_t idx)
			{
				return duk_is_c_function(_ctx.get(),idx) != 0;
			}

			/*		get		*/
			/**
			*	\brief	如果 數據 是 number 返回 number值 否則返回 0\n
			*	... val ...\n
			*/
			inline duk_int_t get_int(duk_idx_t idx)
			{
				return duk_get_int(_ctx.get(),idx);
			}
			/**
			*	\brief	如果 數據 是 number/NaN 返回 number值/0 否則 def\n
			*	... val ...\n
			*/
			inline duk_int_t get_int(duk_idx_t idx,duk_int_t def)
			{
				return duk_get_int_default(_ctx.get(),idx,def);
			}

			/**
			*	\brief	如果 數據 是 number 返回 number值 否則返回 0\n
			*	... val ...\n
			*/
			inline duk_uint_t get_uint(duk_idx_t idx)
			{
				return duk_get_uint(_ctx.get(),idx);
			}
			/**
			*	\brief	如果 數據 是 number/NaN 返回 number值/0 否則返回 def\n
			*	... val ...\n
			*/
			inline duk_uint_t get_uint(duk_idx_t idx,duk_uint_t def)
			{
				return duk_get_uint_default(_ctx.get(),idx,def);
			}
						/**
			*	\brief	如果 數據 是 number 返回 number值 否則返回 nan\n
			*	... val ...\n
			*/
			inline duk_double_t get_number(duk_idx_t idx)
			{
				return duk_get_number(_ctx.get(),idx);
			}
			/**
			*	\brief	如果 數據 是 number/NaN 返回 number值/nan 否則返回 def\n
			*	... val ...\n
			*/
			inline duk_double_t get_number(duk_idx_t idx,duk_double_t def)
			{
				return duk_get_number_default(_ctx.get(),idx,def);
			}

			/**
			*	\brief	如果 數據 是 string 返回 string值 否則返回 nullptr \n
			*	... val ...\n
			*/
			inline const char * get_string(duk_idx_t idx)
			{
				return duk_get_string(_ctx.get(),idx);
			}
			/**
			*	\brief	如果 數據 是 string 返回 string值 否則返回 def \n
			*	... val ...\n
			*/
			inline const char * get_string(duk_idx_t idx,const char *def)
			{
				return duk_get_string_default(_ctx.get(),idx,def);
			}

			/**
			*	\brief	如果 數據 是 string 返回 string值 否則返回 nullptr \n
			*	... val ...\n
			*
			*	\param idx	數據在棧中位置
			*	\param len	返回 字符串長\n
			*				如果 out_len == nullptr 返回 nullptr
			*	\return		只讀的 字符串指針
			*/
			inline const char * get_lstring(duk_idx_t idx, duk_size_t *len)
			{
				return duk_get_lstring(_ctx.get(),idx,len);
			}

			/**
			*	\brief	如果 數據 是 c 指針 返回 指針 否則返回 nullptr \n
			*	... val ...\n
			*
			*/
			inline void * get_pointer(duk_idx_t idx)
			{
				return duk_get_pointer(_ctx.get(),idx);
			}

			/**
			*	\brief	如果 數據 是 c 指針 返回 指針 否則返回 def \n
			*	... val ...\n
			*
			*/
			inline void * get_pointer(duk_idx_t idx,void* def)
			{
				return duk_get_pointer_default(_ctx.get(),idx,def);
			}


			/**
			*	\brief	如果 數據 是 string 返回 string值 否則返回 def_ptr def_len \n
			*	... val ...\n
			*
			*	\param idx		數據在棧中位置
			*	\param out_len	返回 字符串長\n
			*					如果 out_len == nullptr 返回 nullptr
			*	\param	def_ptr	默認的 字符串返回值
			*	\param	def_len	默認的 長度返回值
			*	\return		只讀的 字符串指針
			*/
			inline const char * get_lstring(duk_idx_t idx, duk_size_t *out_len,const char *def_ptr, duk_size_t def_len)
			{
				return duk_get_lstring_default(_ctx.get(),idx,out_len,def_ptr,def_len);
			}
			/*		to		*/
			/**
			*	\brief	to int\n
			*	... val ... -> ... ToNumber(val) ...\n
			*/
			inline duk_int_t to_int(duk_idx_t idx)
			{
				return duk_to_int(_ctx.get(),idx);
			}
			/**
			*	\brief	to int32\n
			*	... val ... -> ... ToInt32(val) ...\n
			*/
			inline duk_int32_t to_int32(duk_idx_t idx)
			{
				return duk_to_int32(_ctx.get(),idx);
			}
			/**
			*	\brief	to double\n
			*	... val ... -> ... ToNumber(val) ...\n
			*/
			inline duk_double_t to_number(duk_idx_t idx)
			{
				return duk_to_number(_ctx.get(),idx);
			}
			/**
			*	\brief	to uint\n
			*	... val ... -> ... ToNumber(val) ...\n
			*/
			inline duk_uint_t to_uint(duk_idx_t idx)
			{
				return duk_to_uint(_ctx.get(),idx);
			}
			/**
			*	\brief	to uint16\n
			*	... val ... -> ... ToUint16(val) ...\n
			*/
			inline duk_uint16_t to_uint16(duk_idx_t idx)
			{
				return duk_to_uint16(_ctx.get(),idx);
			}
			/**
			*	\brief	to uint32\n
			*	... val ... -> ... ToUint32(val) ...\n
			*/
			inline duk_uint32_t to_uint32(duk_idx_t idx)
			{
				return duk_to_uint32(_ctx.get(),idx);
			}
			/**
			*	\brief	to bool\n
			*	... val ... -> ... ToBoolean(val) ...\n
			*/
			inline bool to_bool(duk_idx_t idx)
			{
				return duk_to_boolean(_ctx.get(),idx) !=0;
			}
			/**
			*	\brief	返回 以 0 結尾的 c utf8字符串\n
			*	... val ... -> ... ToString(val) ...\n
			*
			*/
			inline const char * to_c_string(duk_idx_t idx)
			{
				return duk_to_string(_ctx.get(),idx);
			}
			/**
			*	\brief	返回 utf8 字符串\n
			*	... val ... -> ... ToString(val) ...\n
			*
			*/
			inline const char * to_string(duk_idx_t idx,duk_size_t *out_len)
			{
				return duk_to_lstring(_ctx.get(),idx,out_len);
			}
			/**
			*	\brief	返回 utf8 字符串\n
			*	... val ... -> ... ToString(val) ...\n
			*
			*/
			inline const std::string to_string(duk_idx_t idx)
			{
				duk_size_t len;
				const char* str = duk_to_lstring(_ctx.get(),idx,&len);
				return std::string(str,len);
			}
			/**
			*	\brief	返回 utf8 字符串\n
			*	... val ... -> ... ToString(val) ...\n
			*
			*/
			inline void to_string(duk_idx_t idx,std::string& out)
			{
				duk_size_t len;
				const char* str = duk_to_lstring(_ctx.get(),idx,&len);
				out = std::string(str,len);
			}

			/**
			*	\brief	返回 以 0 結尾的 c utf8字符串\n
			*	... val ... -> ... ToString(val) ...\n
			*
			*/
			inline const char * safe_to_c_string(duk_idx_t idx)
			{
				return duk_safe_to_string(_ctx.get(),idx);
			}
			/**
			*	\brief	返回 utf8 字符串\n
			*	... val ... -> ... ToString(val) ...\n
			*
			*/
			inline const char * safe_to_string(duk_idx_t idx,duk_size_t *out_len)
			{
				return duk_safe_to_lstring(_ctx.get(),idx,out_len);
			}
			/**
			*	\brief	返回 utf8 字符串\n
			*	... val ... -> ... ToString(val) ...\n
			*
			*/
			inline std::string safe_to_string(duk_idx_t idx)
			{
				duk_size_t len;
				const char* str = duk_safe_to_lstring(_ctx.get(),idx,&len);
				return std::string(str,len);
			}
			/**
			*	\brief	返回 utf8 字符串\n
			*	... val ... -> ... ToString(val) ...\n
			*
			*/
			inline void safe_to_string(duk_idx_t idx,std::string& out)
			{
				duk_size_t len;
				const char* str = duk_safe_to_lstring(_ctx.get(),idx,&len);
				out = std::string(str,len);
			}

			/**
			*	\brief	to c 指針\n
			*	... val ... -> ... pointer(val) ...\n
			*/
			inline void* to_pointer(duk_idx_t idx)
			{
				return duk_to_pointer(_ctx.get(),idx);
			}

			/*		array		*/
			/**
			*	\brief	返回 數組長度\n
			*	... val ...\n
			*
			*/
			inline duk_size_t get_length(duk_idx_t idx)
			{
				return duk_get_length(_ctx.get(),idx);
			}
			/**
			*	\brief	返回 數組 是否存在指定 索引\n
			*	... obj ... -> ... obj ...\n
			*
			*/
			inline bool has_prop_index(duk_idx_t idx,duk_uarridx_t arr_idx)
			{
				return duk_has_prop_index(_ctx.get(),idx,arr_idx) != 0;
			}

			/**
			*	\brief	將棧頂元素 設置到 數組指定 索引 並出棧\n
			*	... obj ... val -> ... obj ...\n
			*
			*	\param idx	數組在 棧中 位置
			*	\param i	arr[i] =
			*
			*	\attention	此操作等同於 set_prop_string( idx , to_string(i) )
			*
			*/
			inline bool put_prop_index(duk_idx_t idx, duk_uarridx_t i)
			{
				return duk_put_prop_index(_ctx.get(),idx,i) != 0;
			}

			/**
			*	\brief	將 arr[i] 入棧\n
			*	... obj ... -> ... obj ... val (if key exists)\n
			*	... obj ... -> ... obj ... undefined (if key doesn't exist)\n
			*
			*	\param idx	數組在 棧中 位置
			*	\param i	arr[i] =
			*
			*	\attention	此操作等同於 get_prop_string( idx , to_string(i) )
			*/
			inline bool get_prop_index(duk_idx_t idx, duk_uarridx_t i)
			{
				return duk_get_prop_index(_ctx.get(),idx,i) != 0;
			}

			/**
			*	\brief	將 arr[i] 刪除\n
			*	... obj ... -> ... obj ...\n
			*
			*	\param idx	數組在 棧中 位置
			*	\param i	arr[i]
			*
			*	\attention	此操作等同於 del_prop_string( idx , to_string(i) )
			*/
			inline bool del_prop_index(duk_idx_t idx, duk_uarridx_t i)
			{
				return duk_del_prop_index(_ctx.get(),idx,i) != 0;
			}

			/**
			*	\brief	調整數組 長度\n
			*	... val ...\n
			*
			*	\param idx	數組在 棧中 位置
			*	\param len	數組長度
			*
			*/
			inline void set_length(duk_idx_t idx, duk_size_t len)
			{
				duk_set_length(_ctx.get(),idx,len);
			}

			/*		object		*/
			/**
			*	\brief	爲 obj 設置一個 finalizer 函數 在其被釋放時自動調用\n
			*	... obj ... finalizer -> ... obj ...\n
			*
			*/
			inline void set_finalizer(duk_idx_t idx)
			{
				duk_set_finalizer(_ctx.get(),idx);
			}
			/**
			*	\brief	將 obj 的 finalizer 函數 入棧\n
			*	... obj ... -> ... obj ... finalizer\n
			*	... obj ... -> ... obj ... undefined	(if not obj or not exits finalizer)\n
			*
			*/
			inline void get_finalizer(duk_idx_t idx)
			{
				duk_get_finalizer(_ctx.get(),idx);
			}

			/**
			*	\brief	返回 是否存在 obj[key]\n
			*	... obj ... -> ... obj ...\n
			*
			*	\param	idx	obj 在棧中位置
			*	\param	key	要查詢的 key
			*/
			inline bool has_prop_string(duk_idx_t idx,const char *key)
			{
				return duk_has_prop_string(_ctx.get(),idx,key) != 0;
			}
			/**
			*	\brief	返回 是否存在 obj[key]\n
			*	... obj ... -> ... obj ...\n
			*
			*	\param	idx	obj 在棧中位置
			*	\param	key	要查詢的 key
			*	\param	len	key長度
			*/
			inline bool has_prop_string(duk_idx_t idx,const char *key,duk_size_t len)
			{
				return duk_has_prop_lstring(_ctx.get(),idx,key,len) != 0;
			}
			/**
			*	\brief	返回 是否存在 obj[key]\n
			*	... obj ... -> ... obj ...\n
			*
			*	\param	idx	obj 在棧中位置
			*	\param	key	要查詢的 key
			*/
			inline bool has_prop_string(duk_idx_t idx,const std::string& key)
			{
				return duk_has_prop_lstring(_ctx.get(),idx,key.data(),key.size()) != 0;
			}

			/**
			*	\brief	將棧頂元素 設置爲 obj[key] 並出棧\n
			*	... obj ... val -> ... obj ...\n
			*
			*	\param	idx	obj 在棧中位置
			*	\param	key	要設置的 key
			*/
			inline bool put_prop_string(duk_idx_t idx,const char* key)
			{
				return duk_put_prop_string(_ctx.get(),idx,key) != 0;
			}

			/**
			*	\brief	將棧頂元素 設置爲 obj[key] 並出棧\n
			*	... obj ... val -> ... obj ...\n
			*
			*	\param	idx	obj 在棧中位置
			*	\param	key	要設置的 key
			*	\param	len	key長度
			*/
			inline bool put_prop_string(duk_idx_t idx,const char* key,duk_size_t len)
			{
				return duk_put_prop_lstring(_ctx.get(),idx,key,len) != 0;
			}

			/**
			*	\brief	將棧頂元素 設置爲 obj[key] 並出棧\n
			*	... obj ... val -> ... obj ...\n
			*
			*	\param	idx	obj 在棧中位置
			*	\param	key	要設置的 key
			*/
			inline bool put_prop_string(duk_idx_t idx,const std::string& key)
			{
				return duk_put_prop_lstring(_ctx.get(),idx,key.data(),key.size()) != 0;
			}

			/**
			*	\brief	將 obj[key] 入棧\n
			*	... obj ... -> ... obj ... val (if key exists)\n
			*	... obj ... -> ... obj ... undefined (if key doesn't exist)\n
			*
			*	\param	idx	obj 在棧中位置
			*	\param	key	要查詢的 key
			*/
			inline bool get_prop_string(duk_idx_t idx,const char* key)
			{
				return duk_get_prop_string(_ctx.get(),idx,key) != 0;
			}

			/**
			*	\brief	將 obj[key] 入棧\n
			*	... obj ... -> ... obj ... val (if key exists)\n
			*	... obj ... -> ... obj ... undefined (if key doesn't exist)\n
			*
			*	\param	idx	obj 在棧中位置
			*	\param	key	要查詢的 key
			*	\param	len	key長度
			*/
			inline bool get_prop_string(duk_idx_t idx,const char* key,duk_size_t len)
			{
				return duk_get_prop_lstring(_ctx.get(),idx,key,len) != 0;
			}

			/**
			*	\brief	將 obj[key] 入棧\n
			*	... obj ... -> ... obj ... val (if key exists)\n
			*	... obj ... -> ... obj ... undefined (if key doesn't exist)\n
			*
			*	\param	idx	obj 在棧中位置
			*	\param	key	要查詢的 key
			*/
			inline bool get_prop_string(duk_idx_t idx,const std::string& key)
			{
				return duk_get_prop_lstring(_ctx.get(),idx,key.data(),key.size()) != 0;
			}

			/**
			*	\brief	將 obj[key] 刪除\n
			*	... obj ... -> ... obj ...\n
			*
			*	\param	idx	obj 在棧中位置
			*	\param	key	要刪除的 key
			*/
			inline bool del_prop_string(duk_idx_t idx,const char* key)
			{
				return duk_del_prop_string(_ctx.get(),idx,key) != 0;
			}
			/**
			*	\brief	將 obj[key] 刪除\n
			*	... obj ... -> ... obj ...\n
			*
			*	\param	idx	obj 在棧中位置
			*	\param	key	要刪除的 key
			*	\param	len	key長度
			*/
			inline bool del_prop_string(duk_idx_t idx,const char* key,duk_size_t len)
			{
				return duk_del_prop_lstring(_ctx.get(),idx,key,len) != 0;
			}
			/**
			*	\brief	將 obj[key] 刪除\n
			*	... obj ... -> ... obj ...\n
			*
			*	\param	idx	obj 在棧中位置
			*	\param	key	要刪除的 key
			*/
			inline bool del_prop_string(duk_idx_t idx,const std::string& key)
			{
				return duk_del_prop_lstring(_ctx.get(),idx,key.data(),key.size()) != 0;
			}
			/*		enum		*/
			/**
			*	\brief	爲 對象 創建一個 枚舉 並入棧\n
			*	... obj ... -> ... obj ... enum\n
			*
			*	\param	idx		obj 在棧中位置
			*	\param	flags	如何枚舉
			*
			*	\note	DUK_ENUM_INCLUDE_NONENUMERABLE	\n
			*			DUK_ENUM_INCLUDE_HIDDEN			\n
			*			DUK_ENUM_INCLUDE_SYMBOLS		\n
			*			DUK_ENUM_EXCLUDE_STRINGS		\n
			*			DUK_ENUM_OWN_PROPERTIES_ONLY	僅枚舉 自己的屬性值 (包括數值屬性)(包括繼承而來的)	\n
			*			DUK_ENUM_ARRAY_INDICES_ONLY		僅枚舉 數值屬性	\n
			*			DUK_ENUM_SORT_ARRAY_INDICES		排序 和 DUK_ENUM_ARRAY_INDICES_ONLY一起使用 相當緩慢	\n
			*			DUK_ENUM_NO_PROXY_BEHAVIOR		\n
			*/
			inline void push_enum(duk_idx_t idx, duk_uint_t flags = DUK_ENUM_OWN_PROPERTIES_ONLY)
			{
				duk_enum(_ctx.get(),idx,flags);
			}
			/**
			*	\brief	將枚舉對象的當前 棧入棧 並移動到下個值\n
			*	... enum ... -> ... enum ... (if enum empty)\n
			*	... enum ... -> ... enum ... key (if enum not empty and get_value=false)\n
			*	... enum ... -> ... enum ... key value (if enum not empty and get_value=true)\n
			*
			*	\param	idx 枚舉對象 在棧中 位置
			*	\param	get_value	是否在入棧 key 後 入棧 value
			*	\return	如果存在枚舉項   在入棧數據後 返回 true\n
			*			如果布存在枚舉項 不對棧操作 直接返回 false
			*/
			inline bool next(duk_idx_t idx, bool get_value = false)
			{
				if(get_value)
				{
					return duk_next(_ctx.get(),idx,1) != 0;
				}
				return duk_next(_ctx.get(),idx,0) != 0;
			}

			/*	prop	*/
			/**
			*	\brief	將棧頂 作爲 key 出棧 並且 返回 是否存在 obj[key]\n
			*	... obj ... -> ... obj ...\n
			*
			*	\param idx	obj 在棧中位置
			*/
			inline bool has_prop(duk_idx_t idx)
			{
				return duk_has_prop(_ctx.get(),idx) != 0;
			}

			/**
			*	\brief	將棧頂 作爲 key 出棧 並且 將 obj[key] 入棧\n
			*	... obj ... key -> ... obj ... val (if key exists)\n
			*	... obj ... key -> ... obj ... undefined (if key doesn't exist)\n
			*
			*	\param idx	obj 在棧中位置
			*/
			inline bool get_prop(duk_idx_t idx)
			{
				return duk_get_prop(_ctx.get(),idx) != 0;
			}

			/**
			*	\brief	將 -2 作爲 key -1 作爲 value 出棧 並且 obj[key]=value\n
			*	... obj ... key val -> ... obj ...\n
			*
			*	\param idx	obj 在棧中位置
			*/
			inline bool put_prop(duk_idx_t idx)
			{
				return duk_put_prop(_ctx.get(),idx) != 0;
			}

			/**
			*	\brief	將棧頂 作爲 key 出棧 並且 刪除 obj[key]\n
			*	... obj ... key -> ... obj ...\n
			*
			*	\param idx	obj 在棧中位置
			*/
			inline bool del_prop(duk_idx_t idx)
			{
				return duk_del_prop(_ctx.get(),idx) != 0;
			}
			/*		eval		*/
			/**
			*	\brief	將棧頂 作爲腳本 出棧 執行 eval 操作 並且 將 eval 返回值 入棧\n
			*	... source -> ... result\n
			*
			*/
			inline void eval()
			{
				duk_eval(_ctx.get());
			}
			/**
			*	\brief	將字符串 執行 eval 操作 並且 將 eval 返回值 入棧\n
			*	... -> ... result\n
			*
			*/
			inline void eval(const char* str)
			{
				duk_eval_string(_ctx.get(),str);
			}
			/**
			*	\brief	將字符串 執行 eval 操作 並且 將 eval 返回值 入棧\n
			*	... -> ... result\n
			*
			*/
			inline void eval(const char* str,duk_size_t len)
			{
				duk_eval_lstring(_ctx.get(),str,len);
			}
			/**
			*	\brief	將字符串 執行 eval 操作 並且 將 eval 返回值 入棧\n
			*	... -> ... result\n
			*
			*/
			inline void eval(const std::string& str)
			{
				duk_eval_lstring(_ctx.get(),str.data(),str.size());
			}


			/**
			*	\brief	將棧頂 作爲腳本 出棧 執行 eval 操作\n
			*	... source -> ...\n
			*
			*/
			inline void eval_noresult()
			{
				duk_eval_noresult(_ctx.get());
			}
			/**
			*	\brief	將字符串 執行 eval 操作\n
			*	... -> ...\n
			*
			*/
			inline void eval_noresult(const char* str)
			{
				duk_eval_string_noresult(_ctx.get(),str);
			}
			/**
			*	\brief	將字符串 執行 eval 操作\n
			*	... -> ...\n
			*
			*/
			inline void eval_noresult(const char* str,duk_size_t len)
			{
				duk_eval_lstring_noresult(_ctx.get(),str,len);
			}
			/**
			*	\brief	將字符串 執行 eval 操作\n
			*	... -> ...\n
			*
			*/
			inline void eval_noresult(const std::string& str)
			{
				duk_eval_lstring_noresult(_ctx.get(),str.data(),str.size());
			}


			/**
			*	\brief	在 安全模式下 將棧頂 作爲腳本 出棧 執行 eval 操作 並且 將 eval 返回值/錯誤描述 入棧\n
			*	... source -> ... result	(if success, return true)\n
			*	... source -> ... error	(if failure, return false)\n
			*
			*/
			inline bool peval()
			{
				return 0 == duk_peval(_ctx.get());
			}
			/**
			*	\brief	在 安全模式下 將字符串 執行 eval 操作 並且 將 eval 返回值/錯誤描述 入棧\n
			*	... -> ... result	(if success, return true)\n
			*	... -> ... error	(if failure, return false)\n
			*
			*/
			inline bool peval(const char* str)
			{
				return 0 == duk_peval_string(_ctx.get(),str);
			}
			/**
			*	\brief	在 安全模式下 將字符串 執行 eval 操作 並且 將 eval 返回值/錯誤描述 入棧\n
			*	... -> ... result	(if success, return true)\n
			*	... -> ... error	(if failure, return false)\n
			*
			*/
			inline bool peval(const char* str,duk_size_t len)
			{
				return 0 == duk_peval_lstring(_ctx.get(),str,len);
			}
			/**
			*	\brief	在 安全模式下 將字符串 執行 eval 操作 並且 將 eval 返回值/錯誤描述 入棧\n
			*	... -> ... result	(if success, return true)\n
			*	... -> ... error	(if failure, return false)\n
			*
			*/
			inline bool peval(const std::string& str)
			{
				return 0 == duk_peval_lstring(_ctx.get(),str.data(),str.size());
			}


			/**
			*	\brief	在 安全模式下 將棧頂 作爲腳本 出棧 執行 eval 操作\n
			*	... source -> ...\n
			*
			*/
			inline bool peval_noresult()
			{
				return 0 == duk_peval_noresult(_ctx.get());
			}
			/**
			*	\brief	在 安全模式下 將字符串 執行 eval 操作\n
			*	... -> ...\n
			*
			*/
			inline bool peval_noresult(const char* str)
			{
				return 0 == duk_peval_string_noresult(_ctx.get(),str);
			}
			/**
			*	\brief	在 安全模式下 將字符串 執行 eval 操作\n
			*	... -> ...\n
			*
			*/
			inline bool peval_noresult(const char* str,duk_size_t len)
			{
				return 0 == duk_peval_lstring_noresult(_ctx.get(),str,len);
			}
			/**
			*	\brief	在 安全模式下 將字符串 執行 eval 操作\n
			*	... -> ...\n
			*
			*/
			inline bool peval_noresult(const std::string& str)
			{
				return 0 == duk_peval_lstring_noresult(_ctx.get(),str.data(),str.size());
			}


			/*		call		*/
			/**
			*	\brief	調用 函數\n
			*	... func arg1 ... argN -> ... retval\n
			*
			*	\param nargs	參數數量
			*/
			inline void call(duk_idx_t nargs)
			{
				duk_call(_ctx.get(),nargs);
			}

			/**
			*	\brief	調用 函數 (bind this)\n
			*	... func this arg1 ... argN -> ... retval\n
			*
			*	\param nargs	參數數量(不包含 this)
			*/
			inline void call_method(duk_idx_t nargs)
			{
				duk_call_method(_ctx.get(),nargs);
			}

			/**
			*	\brief	調用 obj.func (bind this = obj)\n
			*	... obj ... key arg1 ... argN -> ... obj ... retval\n
			*
			*	\param nargs	參數數量
			*	\param idx	obj 在棧中位置
			*/
			inline void call_prop(duk_idx_t idx,duk_idx_t nargs)
			{
				duk_call_prop(_ctx.get(),idx,nargs);
			}

			/**
			*	\brief	在安全模式下 在安全模式下 調用 函數\n
			*	... func arg1 ... argN -> ... retval	(if success, return true)\n
			*	... func arg1 ... argN -> ... error		(if failure, return false)\n
			*
			*	\param nargs	參數數量
			*/
			inline bool pcall(duk_idx_t nargs)
			{
				return 0 == duk_pcall(_ctx.get(),nargs);
			}

			/**
			*	\brief	在安全模式下 調用 函數 (bind this)\n
			*	... func this arg1 ... argN -> ... retval	(if success, return true)\n
			*	... func this arg1 ... argN -> ... error	(if failure, return false)\n
			*
			*	\param nargs	參數數量(不包含 this)
			*/
			inline bool pcall_method(duk_idx_t nargs)
			{
				return 0 == duk_pcall_method(_ctx.get(),nargs);
			}

			/**
			*	\brief	在安全模式下 調用 obj.func (bind this = obj)\n
			*	... obj ... key arg1 ... argN -> ... obj ... retval	(if success, return true)\n
			*	... obj ... key arg1 ... argN -> ... obj ... error	(if failure, return false)\n
			*
			*	\param nargs	參數數量
			*	\param idx	obj 在棧中位置
			*/
			inline bool pcall_prop(duk_idx_t idx,duk_idx_t nargs)
			{
				return 0 == duk_pcall_prop(_ctx.get(),idx,nargs);
			}
		};
	};
};
#endif // KG_SCRIPTS_DUKTAPE_HEADER_HPP
