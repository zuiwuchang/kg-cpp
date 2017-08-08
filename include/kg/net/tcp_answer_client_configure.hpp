#ifndef KG_NET_TCP_ANSWER_CLIENT_CONFIGURE_HEADER_HPP
#define KG_NET_TCP_ANSWER_CLIENT_CONFIGURE_HEADER_HPP


#include "types.hpp"
#include "../slice.hpp"

namespace kg
{
namespace net
{
	/**
	*	\brief tcp_answer_client_t 的配置
	*
	*	\param T	一個支持copy語義的 自定義 對象 可以用來存儲自定義的 session 數據
	*/
	template<typename T>
	class tcp_answer_client_configure_t
		:boost::noncopyable
	{
	public:
		/**
		*	\brief session 型別
		*
		*/
		typedef T session_t;

		typedef tcp_answer_client_configure_t type_t;
		typedef boost::shared_ptr<type_t> type_spt;
	private:
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
	public:
		/**
		*	\brief 初始化一個默認的 服務器 配置
		*
		*	\param headerSize	消息頭長度
		*/
		tcp_answer_client_configure_t(kg::uint32_t headerSize)
			:_header_size(headerSize)
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
	private:
		create_session_bft _create_session;
		destroy_session_bft _destroy_session;
		reader_header_bft _reader_header;
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
	};

};
};
#endif	//KG_NET_TCP_ANSWER_CLIENT_CONFIGURE_HEADER_HPP
