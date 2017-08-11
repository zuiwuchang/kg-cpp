#ifndef KG_NET_ECHO_SERVER_HEADER_HPP
#define KG_NET_ECHO_SERVER_HEADER_HPP
#include "basic_server.hpp"
#include "../bytes/buffer.hpp"
namespace kg
{
namespace net
{
/**
*	\brief 使用 basic_server_t 實現的 tcp 服務器 可以自動解析 read消息
*
*	\param T session 型別 需要支持 copy 語義
*/
template<typename T>
class echo_server_t
    : boost::noncopyable
{
public:
	/**
	*	\brief type_t type_spt
	*
	*/
    KG_TYPEDEF_TT(echo_server_t);
    /**
	*	\brief type_t type_spt
	*
	*/
	typedef T session_t;
private:
	class basic_session_t
	{
	public:
		session_t session;

		kg::bytes::buffer_t<> buffer;
		int size;
		basic_session_t():size(-1)
		{

		}
	};
	typedef boost::shared_ptr<basic_session_t> basic_session_spt;
	basic_server_t<basic_session_spt> _s;

	int _headerSize;
public:
	/**
	*	\brief 初始化 服務器
	*
	*	\exception boost::system::system_error
	*	\param laddr	服務器監聽地址
	*	\param poll		連接分配cpu 輪詢計算 (每個cpu 會被分配 poll個 連接之後 才會將 連接分配到下個 cpu)
	*	\param timeout	客戶端 未活動 斷開 超時時間(單位 秒)
	*	\param headerSize	消息頭 長度
	*
	*/
    echo_server_t(const std::string& laddr,std::size_t poll,std::size_t timeout,int headerSize)
    	:_s(laddr,poll,timeout)
	{
		if(headerSize > -1)
		{
			_headerSize = headerSize;
		}
		else
		{
			_headerSize = 0;
		}
		//轉發 basic_server 回調
		_s.connected(boost::bind(&type_t::forward_connected,this,_1,_2,_3));
		_s.closed(boost::bind(&type_t::forward_closed,this,_1,_2,_3));
		_s.readed(boost::bind(&type_t::forward_readed,this,_1,_2,_3,_4,_5));
	}
	/**
	*	\brief 初始化 服務器
	*
	*	\param laddr	服務器監聽地址
	*	\param poll		連接分配cpu 輪詢計算 (每個cpu 會被分配 poll個 連接之後 才會將 連接分配到下個 cpu)
	*	\param timeout	客戶端 未活動 斷開 超時時間(單位 秒)
	*	\param headerSize	消息頭 長度
	*
	*/
    echo_server_t(const std::string& laddr,std::size_t poll,std::size_t timeout,int headerSize,boost::system::error_code& ec)
    	:_s(laddr,poll,timeout,ec)
	{
		if(headerSize > -1)
		{
			_headerSize = headerSize;
		}
		else
		{
			_headerSize = 0;
		}

		//轉發 basic_server 回調
		_s.connected(boost::bind(&type_t::forward_connected,this,_1,_2,_3));
		_s.closed(boost::bind(&type_t::forward_closed,this,_1,_2,_3));
		_s.readed(boost::bind(&type_t::forward_readed,this,_1,_2,_3,_4,_5));
	}
	~echo_server_t()
	{
		stop();
	}
	/**
	*	\brief 停止 服務器 釋放所有資源
	*
	*/
    inline void stop()
    {
    	_s.stop();
    }
	/**
	*	\brief 運行 服務器
	*
	*/
    inline void run()
    {
    	_s.run();
    }

public:
	/**
	*	\brief 定義 連接建立後 回調
	*
	*	\return 返回 false 將 自動斷開 連接  並調用 closed 回調
	*/
	typedef boost::function<bool(socket_spt,session_t&,boost::asio::yield_context)> connected_bft;
	/**
	*	\brief 定義 連接斷開後 回調
	*
	*	\return 返回 false 將 自動斷開 連接  並調用 closed 回調
	*/
	typedef boost::function<void(socket_spt,session_t&,boost::asio::yield_context)> closed_bft;
	/**
	*	\brief 定義 讀取到數據後 回調
	*
	*/
	typedef boost::function<bool(socket_spt,session_t&,kg::byte_t*,std::size_t n,boost::asio::yield_context)> readed_bft;

	/**
	*	\brief 解析消息
	*
	*	\param byte_t* 消息頭
	*	\param size_t	消息頭 長度
	*	\return 消息長度 如果<0 或 <headerSize 將 斷開 連接
	*/
	typedef boost::function<int(session_t&,kg::byte_t*,std::size_t,boost::asio::yield_context)> reader_bft;
private:
	//轉發 basic_server 回調
	bool forward_connected(kg::net::socket_spt s,basic_session_spt& basic_session,boost::asio::yield_context ctx)
	{
		try
		{
			basic_session = boost::make_shared<basic_session_t>();
		}
		catch(const std::bad_alloc&)
		{
			return false;
		}

		session_t session = session_t();
		if(_connected && ! _connected(s,session,ctx))
		{
			return false;
		}
		basic_session->session = session;
		return true;
	}
	void forward_closed(kg::net::socket_spt s,basic_session_spt& basic_session,boost::asio::yield_context ctx)
	{
		if(_closed)
		{
			_closed(s,basic_session->session,ctx);
		}
		basic_session.reset();
	}
	bool forward_readed(kg::net::socket_spt s,basic_session_spt& basic_session,kg::byte_t*b,std::size_t n,boost::asio::yield_context ctx)
	{
		//不處理 數據包
		if(!_readed)
		{
			return true;
		}

		//未設置 解包
		basic_session_t& basic = *basic_session;
		if(!_reader)
		{
			return _readed(s,basic.session,b,n,ctx);
		}

		try
		{
			kg::bytes::buffer_t<>& buffer = basic.buffer;
			if(n != buffer.write(b,n))
			{
				return false;
			}

			//開始 解包
			if(basic.size == -1)
			{
				//讀取包頭
				if(_headerSize == 0)
				{
					basic.size = _reader(basic.session,NULL,0,ctx);
				}
				else
				{
					if(buffer.size()<_headerSize)
					{
						//等待 包頭
						return true;
					}
					//解析包頭
					boost::shared_array<kg::byte_t> header(new kg::byte_t[_headerSize]);
					buffer.copy_to(header.get(),_headerSize);
					basic.size = _reader(basic.session,header.get(),_headerSize,ctx);
				}

				//解包錯誤
				if(basic.size < _headerSize || basic.size < 1)
				{
					return false;
				}
			}

			if(buffer.size() < basic.size)
			{
				//等待 body
				return true;
			}
			boost::shared_array<kg::byte_t> msg(new kg::byte_t[basic.size]);
			buffer.read(msg.get(),basic.size);
			//通知 回調
			if(!_readed(s,basic.session,msg.get(),basic.size,ctx))
			{
				return false;
			}

			//重置 消息解析 狀態
			basic.size = -1;
		}
		catch(const std::bad_alloc&)
		{
			return false;
		}
		return true;
	}

	connected_bft _connected;
	closed_bft _closed;
	readed_bft _readed;
	reader_bft _reader;
public:
	/**
	*	\brief 設置 連接建立後 回調
	*
	*/
	inline void connected(connected_bft func)
	{
		_connected = func;
	}
	/**
	*	\brief 設置 連接斷開後 回調
	*
	*/
	inline void closed(closed_bft func)
	{
		_closed = func;
	}
	/**
	*	\brief 設置 讀取到 完整 數據包後 回調
	*
	*/
	inline void readed(readed_bft func)
	{
		_readed = func;
	}
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
#endif // KG_NET_ECHO_SERVER_HEADER_HPP
