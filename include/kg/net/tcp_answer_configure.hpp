#ifndef KG_NET_TCP_ANSWER_CONFIGURE_HEADER_HPP
#define KG_NET_TCP_ANSWER_CONFIGURE_HEADER_HPP


#include "types.hpp"
#include "../slice.hpp"

namespace kg
{
namespace net
{
	/**
	*	\brief tcp_answer_t 的配置
	*
	*	\param T	一個支持copy語義的 自定義 對象 可以用來存儲自定義的 session 數據
	*/
	template<typename T>
	class tcp_answer_configure_t
		:boost::noncopyable
	{
	public:
		/**
		*	\brief session 型別
		*
		*/
		typedef T session_t;

		typedef tcp_answer_configure_t type_t;
		typedef boost::shared_ptr<type_t> type_spt;
	private:
		/**
		*	\brief tcp 超時 時間
		*
		*/
		boost::posix_time::time_duration _timeout;

		/**
		*	\brief recv 緩衝區大小
		*
		*/
		std::size_t _buffer_size;

		/**
		*	\brief 消息頭 長度
		*
		*/
		kg::uint32_t _header_size;
		/**
		*	\brief 將tcp分配到不同 cpu時 忽略的大小 compare 最小取值爲 1
		*
		*/
		std::size_t _compare;
	public:
		/**
		*	\brief 初始化一個默認的 服務器 配置
		*
		*	\param headerSize	消息頭長度
		*/
		tcp_answer_configure_t(kg::uint32_t headerSize)
			:_timeout(boost::posix_time::hours(1))
			,_header_size(headerSize)
			,_compare(50)
			,_buffer_size(1024)
		{

		}

		/**
		*	\brief 返回 recv 緩衝區大小
		*
		*/
		inline std::size_t buffer_size()const
		{
			return _buffer_size;
		}
		/**
		*	\brief 設置 recv 緩衝區大小
		*
		*/
		inline void buffer_size(std::size_t size)
		{
			_buffer_size = size;
		}

		/**
		*	\brief 返回消息頭 長度
		*
		*/
		inline kg::uint32_t header_size()const
		{
			return _header_size;
		}
		/**
		*	\brief 設置消息頭 長度
		*
		*/
		inline void header_size(kg::uint32_t n)
		{
			_header_size = n;
		}

		/**
		*	\brief 返回消息頭 長度
		*
		*/
		inline boost::posix_time::time_duration timeout()const
		{
			return _timeout;
		}
		/**
		*	\brief 設置消息頭 長度
		*
		*/
		inline void timeout(boost::posix_time::time_duration duration)
		{
			_timeout = duration;
		}

		/**
		*	\brief 將tcp分配到不同 cpu時 忽略的大小 compare 最小取值爲 1
		*
		*/
		inline std::size_t compare()const
		{
			return _compare;
		}
		/**
		*	\brief 將tcp分配到不同 cpu時 忽略的大小 compare 最小取值爲 1
		*
		*/
		inline void compare(std::size_t n)
		{
			if(n < 1)
			{
				return;
			}
			_compare = n;
		}


	public:
		/**
		*	\brief 如何創建 session 默認 session_t()
		*
		*/
		typedef boost::function<session_t(socket_spt)> create_session_bft;

		/**
		*	\brief 如何銷毀 session
		*
		*/
		typedef boost::function<void(socket_spt,session_t)> destroy_session_bft;

		/**
		*	\brief 傳入 header 返回 header + body 長度 錯誤返回 一個小於 headerSize 的值 通常是 0
		*
		*/
		typedef boost::function<kg::uint32_t(kg::byte_t*,std::size_t)> reader_header_bft;

		typedef kg::slice_t<kg::byte_t> slice_t;
		/**
		*	\brief 當收到消息時 回調 返回 false 將斷開 連接
		*
		*/
		typedef boost::function<bool(socket_spt,session_t,slice_t)> message_bft;
	private:
		create_session_bft _create_session;
		destroy_session_bft _destroy_session;
		reader_header_bft _reader_header;
		message_bft _message;
	public:
		/**
		*	\brief 設置 創建 session 回調 在新連接創建成功時 回調此函數 默認 T()
		*
		*/
		inline void create_session(create_session_bft bf)
		{
			_create_session = bf;
		}
		/**
		*	\brief 返回 創建 session 回調 在新連接創建成功時 回調此函數 默認 T()
		*
		*/
		inline create_session_bft create_session()const
		{
			return _create_session;
		}

		/**
		*	\brief 設置 銷毀 session 回調 在連接調開前 回調此函數
		*
		*/
		inline void destroy_session(destroy_session_bft bf)
		{
			_destroy_session = bf;
		}
		/**
		*	\brief 返回 銷毀 session 回調 在連接調開前 回調此函數
		*
		*/
		inline destroy_session_bft destroy_session()const
		{
			return _destroy_session;
		}

		/**
		*	\brief 設置 銷毀 session 回調 在連接調開前 回調此函數
		*
		*/
		inline void reader_header(reader_header_bft bf)
		{
			_reader_header = bf;
		}
		/**
		*	\brief 返回 銷毀 session 回調 在連接調開前 回調此函數
		*
		*/
		inline reader_header_bft reader_header()const
		{
			return _reader_header;
		}

		/**
		*	\brief 設置 收到消息時 回調 返回 false 將斷開 連接
		*
		*/
		inline void message(message_bft bf)
		{
			_message = bf;
		}

		/**
		*	\brief 返回 當收到消息時 回調 返回 false 將斷開 連接
		*
		*/
		inline message_bft message()const
		{
			return _message;
		}
	};

};
};
#endif	//KG_NET_TCP_ANSWER_CONFIGURE_HEADER_HPP
