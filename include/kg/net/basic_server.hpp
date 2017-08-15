#ifndef KG_NET_BASIC_SERVER_HEADER_HPP
#define KG_NET_BASIC_SERVER_HEADER_HPP

#include <boost/asio/spawn.hpp>
#include <boost/typeof/typeof.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/unordered_set.hpp>


#include "types.hpp"
//#include "../debug.hpp"


namespace kg
{
namespace net
{
#ifndef KG_NET_BASIC_SERVER_BUFFER_SIZE
#define KG_NET_BASIC_SERVER_BUFFER_SIZE	1024
#endif // KG_NET_BASIC_SERVER_BUFFER_SIZE

#define KG_NET_BASIC_SERVER_CODE_BAD_ALLOC	1
#define KG_NET_BASIC_SERVER_CODE_BAD_ADDR	100
/**
*	\brief basic_server_t 異常定義
*
*/
class basic_server_category :
    public boost::system::error_category
{
public:
    virtual const char *name() const BOOST_SYSTEM_NOEXCEPT
    {
        return "kg::net::basic_server : ";
    }
    virtual std::string message(int ev) const
    {
    	std::string msg("kg::net::basic_server : ");
        switch(ev)
        {
        case 0:
            return msg + "success";
		case KG_NET_BASIC_SERVER_CODE_BAD_ALLOC:
            return msg + "bad alloc";
        case KG_NET_BASIC_SERVER_CODE_BAD_ADDR:
            return msg + "bad listen address";
        }
        return msg + "unknow";
    }
    static basic_server_category& get()
    {
    	static basic_server_category instance;
    	return instance;
    }
};

/**
*	\brief 使用 boost::asio 協程 實現的 tcp 服務器
*
*	\param T	用於關聯到 連接 的 自定義 session 型別 需要支持 copy 語義
*/
template<typename T>
class basic_server_t
    : boost::noncopyable
{
public:
	/**
	*	\brief type_t type_spt
	*
	*/
    KG_TYPEDEF_TT(basic_server_t);
    /**
	*	\brief type_t type_spt
	*
	*/
	typedef T session_t;
private:
    //listen
    io_service_spt _service;
    acceptor_spt _acceptor;

    //read
    class service_t: boost::noncopyable
    {
	private:
		//連接 客戶端
		boost::unordered_set<socket_spt> _clients;
		boost::mutex _mutex;
		void clear()
		{
			boost::system::error_code ec;
			boost::mutex::scoped_lock(_mutex);
			BOOST_FOREACH(const socket_spt& sock,_clients)
			{
				sock->shutdown(socket_t::shutdown_both,ec);
        		sock->close(ec);
			}
			_clients.clear();
		}
	public:
		//asio 服務
		io_service_t service;
		//asio work
		work_spt work;
		//已連接客戶數
		std::size_t clients;

		//工作線程
		thread_spt thread;

		service_t()
		{
			clients = 0;
		}
		~service_t()
		{
			stop();
		}
		void stop()
		{
			//結束 asio work
			if(work)
			{
				work.reset();
			}
			//關閉 socket
			clear();

			//等待 線程退出
			if(thread)
			{
				thread->join();
				thread.reset();
			}
		}
		void insert(const socket_spt& sock)
		{
			boost::mutex::scoped_lock(_mutex);
			_clients.insert(sock);
		}
		void erase(const socket_spt& sock)
		{
			boost::mutex::scoped_lock(_mutex);
			BOOST_AUTO(find,_clients.find(sock));
			if(find != _clients.end())
			{
				_clients.erase(find);
			}
		}
    };
    typedef boost::shared_ptr<service_t> service_spt;
    std::vector<service_spt> _services;

    //當前響應 服務器
    std::size_t _flag;
    std::size_t _pos;
    std::size_t _poll;

    //客戶端 超時 時間 如果爲0 永不超時
    std::size_t _timeout;

	void init(const std::string& laddr)
	{
		//解析地址
		BOOST_AUTO(find,laddr.find_last_of(':'));
		if(find == std::string::npos)
		{
			BOOST_THROW_EXCEPTION(boost::system::system_error(KG_NET_BASIC_SERVER_CODE_BAD_ADDR,basic_server_category::get()));
		}
		std::string tmp = laddr.substr(find+1);
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
			BOOST_THROW_EXCEPTION(boost::system::system_error(KG_NET_BASIC_SERVER_CODE_BAD_ADDR,basic_server_category::get()));
		}
		tmp = laddr.substr(0,find);

		//創建 asio 服務
		try
		{
			//創建 asio
			_service = boost::make_shared<io_service_t>();

			//創建 監聽器
			_acceptor = boost::make_shared<acceptor_t>(*_service,boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string(tmp),port));
			
			//創建 響應 服務器
			std::size_t n = boost::thread::hardware_concurrency();
			for(std::size_t i=0; i<n; ++i)
			{
				service_spt service = boost::make_shared<service_t>();
				service->work = boost::make_shared<work_t>(service->service);
				service->thread = boost::make_shared<thread_t>(boost::bind(type_t::works_thread,&(service->service)));

				_services.push_back(service);
			}
		}
		catch(const std::bad_alloc&)
		{
			BOOST_THROW_EXCEPTION(boost::system::system_error(KG_NET_BASIC_SERVER_CODE_BAD_ALLOC,basic_server_category::get()));
		}
	}
	static void works_thread(io_service_t* service)
	{
		//KG_TRACE("start read service "<<service)
		service->run();
		//KG_TRACE("exit read service "<<service)
	}
public:
	/**
	*	\brief 初始化 服務器
	*
	*	\exception boost::system::system_error
	*	\param laddr	服務器監聽地址
	*	\param poll		連接分配cpu 輪詢計算 (每個cpu 會被分配 poll個 連接之後 才會將 連接分配到下個 cpu)
	*	\param timeout	客戶端 未活動 斷開 超時時間(單位 秒)
	*
	*/
    basic_server_t(const std::string& laddr,std::size_t poll,std::size_t timeout)
    	:_pos(0),_timeout(timeout),_run(false)
    {
    	//設置輪詢 響應服務器 差值
    	if(poll < 1)
		{
			poll = 10;
		}
		_poll = _flag = poll;

    	init(laddr);
    }
    /**
	*	\brief 初始化 服務器
	*
	*	\param laddr	服務器監聽地址
	*	\param poll		連接分配cpu 輪詢計算 (每個cpu 會被分配 poll個 連接之後 才會將 連接分配到下個 cpu)
	*	\param timeout	客戶端 未活動 斷開 超時時間(單位 秒)
	*
	*/
    basic_server_t(const std::string& laddr,std::size_t poll,std::size_t timeout,boost::system::error_code& ec)
    	:_pos(0),_timeout(timeout),_run(false)
    {
    	try
    	{
    		//設置輪詢 響應服務器 差值
    		if(poll < 1)
			{
				poll = 10;
			}
			_poll = _flag = poll;

    		init(laddr);
    	}
    	catch(const boost::system::system_error& e)
    	{
    		ec = e.code();
    	}
    }
    ~basic_server_t()
    {
    	stop();
    }
public:
    /**
	*	\brief 停止 服務器 釋放所有資源
	*
	*/
    void stop()
    {
		boost::mutex::scoped_lock(_mutex);
		if(!_run)
		{
			return;
		}
		_run = false;
		//停止 監聽 服務器    	
    	if(_acceptor)
		{
			boost::system::error_code ec;
			_acceptor->cancel(ec);
			_acceptor.reset();
		}

		if(_service)
		{
			_service->stop();
			_service.reset();
		}

    	//停止 響應服務器
    	BOOST_FOREACH(service_spt& service,_services)
		{
			service->stop();
		}
		_services.clear();

		if(_thread)
		{
			_thread->join();
			_thread.reset();
		}
    }
    /**
	*	\brief 運行 服務器
	*	\exception boost::system::system_error
	*
	*/
    void run()
    {
		try
		{
			boost::mutex::scoped_lock(_mutex);
			if(_run)
			{
				return;
			}
			post_accept();
			_run = true;
			_thread = boost::make_shared<thread_t>(boost::bind(&basic_server_t::run_thread,this,_service));
		}
		catch(const std::bad_alloc&)
		{
			BOOST_THROW_EXCEPTION(boost::system::system_error(KG_NET_BASIC_SERVER_CODE_BAD_ALLOC,basic_server_category::get()));
		}
    }
	/**
	*	\brief 運行 服務器
	*
	*/
    void run(boost::system::error_code& ec)
	{
		try
		{
			run();
		}
		catch(const boost::system::system_error& e)
		{
			ec = e.code();
		}
	}
	/**
	*	\brief 等待服務器停止
	*
	*/
	void join()
	{
		_mutex.lock();
		thread_spt thread = _thread;
		_mutex.unlock();
		if(thread)
		{
			thread->join();
		}
	}
private:
	bool _run;
	boost::mutex _mutex;
	thread_spt _thread;
	void run_thread(io_service_spt service)
	{
        service->run();
	}
private:
    void post_accept()
    {
    	try
    	{
    		service_spt service = get_service();

			socket_spt sock = boost::make_shared<socket_t>(service->service);
			_acceptor->async_accept(*sock,
								   boost::bind(&type_t::post_accept_handler,
											   this,
											   boost::asio::placeholders::error,
											   sock,
											   service)
								  );
    	}
    	catch(const std::exception&)
    	{
    		//出錯 重新 投遞 accept
    		post_accept();
    	}
    }

    service_spt get_service()
    {
        if(_flag)
        {
            --_flag;
            return _services[_pos];
        }

        std::size_t min = _services[_pos]->clients;
        std::size_t n;
        _pos = 0;
        for(std::size_t i=1; i<_services.size(); ++i)
        {
        	n = _services[i]->clients;
            if(n < min)
            {
                min = n;
                _pos = i;
            }
        }
        _flag = _poll - 1;
        return _services[_pos];
    }

    void post_accept_handler(const boost::system::error_code& ec,socket_spt sock,service_spt service)
    {
		if(!_run)
		{
			return;
		}

        post_accept();
        if(ec)
        {
            return;
        }

        //爲 socket 啓動 通信 coroutine
        boost::asio::spawn(service->service,boost::bind(&type_t::coroutine_read,this,sock,service,_timeout,_1));
    }

	void coroutine_read(socket_spt sp,service_spt service,std::size_t timeout,boost::asio::yield_context ctx)
    {
    	service->insert(sp);

    	std::size_t& clients = service->clients;
    	++clients;

    	boost::system::error_code ec;
    	socket_t& s = *sp;
    	//KG_TRACE("one in");
    	session_t session = session_t();
    	if(!_connected || _connected(sp,session,ctx))
		{
			deadline_timer_t timer(service->service);
			try
			{
				//讀取消息
				byte_t buffer[KG_NET_BASIC_SERVER_BUFFER_SIZE];
				while(true)
				{
					//超時 斷開
					if(timeout)
					{
						timer.expires_from_now(boost::posix_time::seconds(timeout));
						timer.async_wait(boost::bind(handler_timer,sp,_1));
					}

					//接收消息 yield
					std::size_t n = s.async_read_some(boost::asio::buffer(buffer,KG_NET_BASIC_SERVER_BUFFER_SIZE),ctx);
					//取消 超時 斷開
					if(timeout)
					{
						timer.cancel(ec);
					}

					//通知 響應
					if(_readed &&
						!_readed(sp,session,buffer,n,ctx)
						)
					{
						break;
					}
				}
			}
			catch(const boost::system::system_error&)
			{
			}
			if(timeout)
			{
				timer.cancel(ec);
			}
		}

        s.shutdown(socket_t::shutdown_both,ec);
        s.close(ec);

		service->erase(sp);
        --clients;

        //KG_TRACE("one out");
		if(_closed)
		{
			_closed(sp,session,ctx);
		}
    }
    static void handler_timer(socket_spt sock,const boost::system::error_code& e)
    {
		if(e)
		{
			return;
		}
		boost::system::error_code ec;
		sock->shutdown(socket_t::shutdown_both,ec);
        sock->close(ec);
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

private:
	connected_bft _connected;
	closed_bft _closed;
	readed_bft _readed;
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
	*	\brief 設置 讀取到數據後 回調
	*
	*/
	inline void readed(readed_bft func)
	{
		_readed = func;
	}
};
};
};
#endif	//KG_NET_BASIC_SERVER_HEADER_HPP
