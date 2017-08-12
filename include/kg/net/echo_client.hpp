#ifndef KG_NET_ECHO_CLIENT_HEADER_HPP
#define KG_NET_ECHO_CLIENT_HEADER_HPP

#include <boost/typeof/typeof.hpp>
#include <boost/lexical_cast.hpp>

#include "types.hpp"
#include "../bytes/buffer.hpp"
#include "../slice.hpp"

namespace kg
{
namespace net
{

#define KG_NET_ECHO_CLIENT_CODE_BAD_ALLOC		1
#define KG_NET_ECHO_CLIENT_CODE_BAD_ADDR		100
#define KG_NET_ECHO_CLIENT_CODE_BAD_MSG_HEADER	200
/**
*	\brief echo_client_t 異常定義
*
*/
class echo_client_category :
    public boost::system::error_category
{
public:
    virtual const char *name() const BOOST_SYSTEM_NOEXCEPT
    {
        return "kg::net::echo_client : ";
    }
    virtual std::string message(int ev) const
    {
    	std::string msg("kg::net::echo_client : ");
        switch(ev)
        {
        case 0:
            return msg + "success";
		case KG_NET_ECHO_CLIENT_CODE_BAD_ALLOC:
            return msg + "bad alloc";
        case KG_NET_ECHO_CLIENT_CODE_BAD_ADDR:
            return msg + "bad listen address";
		case KG_NET_ECHO_CLIENT_CODE_BAD_MSG_HEADER:
			return msg + "bad msg header";
        }
        return msg + "unknow";
    }
    static echo_client_category& get()
    {
    	static echo_client_category instance;
    	return instance;
    }
};

/**
*	\brief 使用 可以自動解析 read消息的 同步 tcp client
*
*/
class echo_client_t
    : boost::noncopyable
{
public:
	/**
	*	\brief type_t type_spt
	*
	*/
    KG_TYPEDEF_TT(echo_client_t);

	typedef kg::slice_t<kg::byte_t> bytes_t;
private:
	io_service_t _service;
	socket_t _socket;

	int _headerSize;
	kg::bytes::buffer_t<> _buffer;
	int _size;
	kg::byte_t _data[1024];

	bytes_t _empty_bytes;
public:
	/**
	*	\brief 初始化 服務器
	*
	*	\exception std::bad_alloc
	*	\param headerSize	消息頭 長度
	*
	*/
    echo_client_t(int headerSize)
    	:_socket(_service),_size(-1)
	{
		if(headerSize > -1)
		{
			_headerSize = headerSize;
		}
		else
		{
			_headerSize = 0;
		}

	}

	~echo_client_t()
	{
		//stop();
		boost::system::error_code ec;
		close(ec);
	}
	/**
	*	\brief 返回原始 socket
	*
	*/
	inline socket_t& get()
	{
		return _socket;
	}

	/**
	*	\brief 連接服務器
	*
	*	\exception boost::system::system_error
	*/
	void connect(const std::string& addr)
	{
		//解析地址
		BOOST_AUTO(find,addr.find_last_of(':'));
		if(find == std::string::npos)
		{
			BOOST_THROW_EXCEPTION(boost::system::system_error(KG_NET_ECHO_CLIENT_CODE_BAD_ADDR,echo_client_category::get()));
		}
		std::string tmp = addr.substr(find+1);
		unsigned short port = 0;
		try
		{
			port = boost::lexical_cast<unsigned short>(tmp);
		}
		catch(const boost::bad_lexical_cast&)
		{
		}
		if(port == 0)
		{
			BOOST_THROW_EXCEPTION(boost::system::system_error(KG_NET_ECHO_CLIENT_CODE_BAD_ADDR,echo_client_category::get()));
		}
		tmp = addr.substr(0,find);

		//connect
		_socket.connect(boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string(tmp),port));
	}
	/**
	*	\brief 連接服務器
	*
	*/
	void connect(const std::string& addr,boost::system::error_code& ec)
	{
		try
		{
			connect(addr);
		}
		catch(const boost::system::system_error& e)
		{
			ec = e.code();
		}
	}
	/**
	*	\brief 關閉 socket
	*
	*	\exception boost::system::system_error
	*/
	void close()
	{
		_socket.shutdown(socket_t::shutdown_both);
        _socket.close();
	}
	/**
	*	\brief 關閉 socket
	*
	*/
	void close(boost::system::error_code& ec)
	{
		try
		{
			close();
		}
		catch(const boost::system::system_error& e)
		{
			ec = e.code();
		}
	}

	/**
	*	\brief 解析出一個 消息
	*
	*	\exception boost::system::system_error
	*	\param b	在進行解析前 寫入到解析器的 數據
	*	\param n	在進行解析前 寫入到解析器的 數據 長度
	*/
	bytes_t reader_message(const kg::byte_t* b = NULL,std::size_t n = 0)
	{
		try
		{
			if(b)
			{
				if(n != _buffer.write(_data,n))
				{
					BOOST_THROW_EXCEPTION(boost::system::system_error(KG_NET_ECHO_CLIENT_CODE_BAD_ALLOC,echo_client_category::get()));
				}
			}

			/***	解消息	***/
			if(_size == -1)
			{
				//讀取包頭
				if(_headerSize == 0)
				{
					_size = _reader(NULL,0);
				}
				else
				{
					if(_buffer.size()<_headerSize)
					{
						//等待 包頭
						return _empty_bytes;
					}
					//解析包頭
					boost::shared_array<kg::byte_t> header(new kg::byte_t[_headerSize]);
					_buffer.copy_to(header.get(),_headerSize);
					_size = _reader(header.get(),_headerSize);
				}

				//解包錯誤
				if(_size < _headerSize || _size < 1)
				{
					BOOST_THROW_EXCEPTION(boost::system::system_error(KG_NET_ECHO_CLIENT_CODE_BAD_MSG_HEADER,echo_client_category::get()));
				}
			}

			if(_buffer.size() < _size)
			{
				//等待 body
				return _empty_bytes;
			}

			//返回消息
			bytes_t msg(_size);
			_buffer.read(msg.get(),_size);
			_size = -1;
			return msg;
		}
		catch(const std::bad_alloc&)
		{

		}
		return _empty_bytes;
	}
	/**
	*	\brief 解析出一個 消息
	*
	*	\param b	在進行解析前 寫入到解析器的 數據
	*	\param n	在進行解析前 寫入到解析器的 數據 長度
	*/
	bytes_t reader_message(const kg::byte_t* b,std::size_t n,boost::system::error_code& ec)
	{
		try
		{
			return reader_message(b,n);
		}
		catch(const boost::system::system_error&e)
		{
			ec = e.code();
		}
		return _empty_bytes;
	}

	/**
	*	\brief 讀取一個消息
	*
	*	\exception boost::system::system_error
	*/
	bytes_t read()
	{
		bytes_t msg = reader_message();
		if(msg.size())
		{
			return msg;
		}

		while(true)
		{
			std::size_t	n = _socket.read_some(boost::asio::buffer(_data,1024));
			//沒有 解包 函數 直接返回 數據
			if(!_reader)
			{
				bytes_t bytes(n);
				return bytes;
			}

			//write
			if(n != _buffer.write(_data,n))
			{
				BOOST_THROW_EXCEPTION(boost::system::system_error(KG_NET_ECHO_CLIENT_CODE_BAD_ALLOC,echo_client_category::get()));
			}

			msg = reader_message();
			if(msg.size())
			{
				return msg;
			}
		}
		return _empty_bytes;
	}


	/**
	*	\brief 讀取一個消息
	*
	*	\exception boost::system::system_error
	*/
	bytes_t read(boost::system::error_code& ec)
	{
		try
		{
			return read();
		}
		catch(const boost::system::system_error& e)
		{
			ec = e.code();
		}
		return _empty_bytes;
	}
	/**
	*	\brief 解析消息
	*
	*	\param byte_t* 消息頭
	*	\param size_t	消息頭 長度
	*	\return 消息長度 如果<0 或 <headerSize 將 斷開 連接
	*/
	typedef boost::function<int(kg::byte_t*,std::size_t)> reader_bft;
private:
	reader_bft _reader;
public:
	/**
	*	\brief 設置 解包 回調
	*
	*/
	inline void reader(reader_bft func)
	{
		_reader = func;
	}
};

};
};
#endif // KG_NET_ECHO_CLIENT_HEADER_HPP
