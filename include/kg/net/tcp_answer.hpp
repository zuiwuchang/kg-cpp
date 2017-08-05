#ifndef KG_NET_TCP_ANSWER_HEADER_HPP
#define KG_NET_TCP_ANSWER_HEADER_HPP

#include <vector>

#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/typeof/typeof.hpp>
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/unordered_map.hpp>

#include "../types.hpp"
#include "../bytes/buffer.hpp"
#include "types.hpp"
#include "bad_address.hpp"
#include "tcp_answer_configure.hpp"
#include "tcp_answer_service_info.hpp"


namespace kg
{
namespace net
{
/**
*	\brief 一個tcp 應答服務器
*
*	孤 發現 很多時候 需要 實現一個 tcp 應答服務器 類似 rpc/http 服務器\n
*	(rpc 客戶端 服務器 實現起來都 比較 繁瑣 且 不同客戶端 語言 也遠是個麻煩 rpc 也沒有 session)\n
*	(應答服務器 接收一個 客戶請求 之後 返回響應 基本和 http 工作模式 一樣 只是 現在 保存了 狀態 並且 是長連接的)\n
*
*	\param T	一個支持copy語義的 自定義 對象 可以用來存儲自定義的 session 數據
*
*/
template<typename T>
class tcp_answer_t
    :boost::noncopyable
{
public:
	/**
    *	\brief class 型別
    *
    */
    typedef tcp_answer_t type_t;

    /**
    *	\brief session 型別
    *
    */
    typedef T session_t;

    typedef	tcp_answer_configure_t<session_t> 	configure_t;
    typedef	boost::shared_ptr<configure_t> configure_spt;
private:
    configure_spt _cnf;

    typedef answer_service_info_t<session_t> service_info_t;
	typedef boost::shared_ptr<service_info_t> service_info_spt;
	typedef std::vector<service_info_spt> services_info_t;


private:

public:
    /**
    *	\brief 啓動服務器 並監聽 指定地址
    *
    *	\exception class std::bad_alloc
    *	\exception class boost::system::system_error
    *	\exception class bad_listen_address_t
    *
    *	\param addr		形如 :port XXX:port 的監聽地址
    *	\param cnf		服務器配置
    *
    */
    tcp_answer_t(std::string addr,configure_spt cnf)
        :_timer(_service)
        ,_cnf(cnf)
        ,_flag(0),_pos(0)
    {
        //分解 服務器 地址
        boost::algorithm::trim(addr);
        BOOST_AUTO(find,addr.find(":"));
        if(find == addr.npos)
        {
            throw bad_address_t();
        }
        std::string ip = addr.substr(0,find);
        addr = addr.substr(find+1);
        unsigned short port;
        try
        {
            port = boost::lexical_cast<unsigned short>(addr);
            if(port == 0)
            {
                throw bad_address_t();
            }
        }
        catch(const boost::bad_lexical_cast&)
        {
            throw bad_address_t();
        }

        //監聽 端口
        if(ip.empty())
        {
            //監聽
            boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address_v4::any(),port);
            _acceptor = boost::make_shared<acceptor_t>(_service,endpoint);
        }
        else
        {
            //監聽
            boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address::from_string(ip),port);
            _acceptor = boost::make_shared<acceptor_t>(_service,endpoint);
        }


        //創建 響應 服務 線程
        std::size_t n = boost::thread::hardware_concurrency();
        for(std::size_t i=0; i<n; ++i)
        {
            //創建 服務
            io_service_spt service = boost::make_shared<io_service_t>();
            work_spt work = boost::make_shared<work_t>(*service);
            service_info_spt info = boost::make_shared<service_info_t>(service,work,_cnf);

            _services.push_back(info);


            //啓動 工作線程
            _threads.create_thread(boost::bind(works,service));
        }
    }
    ~tcp_answer_t()
    {

    }

    /**
    *	\brief 運行服務器
    *
    *	\exception class std::bad_alloc
    *
    *	\warning	一旦 stop 之後 不能再次 run
    */
    void run()
    {
        if(!_thread)
        {
            _thread = boost::make_shared<thread_t>(&tcp_answer_t::work,this);
        }
    }
    /**
    *	\brief 停止服務器 並等待 服務結束
    *
    *	\warning	一旦 stop 之後 不能再次 run
    */
    void stop()
    {
        //停止 監聽服務器
        if(_acceptor)
        {
            boost::system::error_code ec;

            //stop
            _service.stop();
            _acceptor->cancel(ec);

            //等待線程 結束
            if(_thread)
            {
                _thread->join();
                _thread.reset();
            }

            //釋放資源
            _acceptor.reset();
        }

        //停止響應服務器
        if(!_services.empty())
        {
            //stop
            BOOST_FOREACH(service_info_spt& info,_services)
            {
                info->stop();
            }

            //等待線程 結束
            _threads.join_all();

            //釋放資源
            _services.clear();
        }
    }

    /**
    *	\brief 等待服務器 停止
    *
    */
    inline void join()
    {
        _threads.join_all();
    }
private:
    /**
    *	\brief 接收連接的 asio 服務
    *
    */
    io_service_t _service;
    /**
    *	\brief 與 _service 關聯的 定時器
    *
    */
    deadline_timer_t _timer;
    /**
    *	\brief asio 接受器
    *
    */
    acceptor_spt _acceptor;
    /**
    *	\brief 監聽線程
    *
    */
    thread_spt _thread;
    void work()
    {
        _service.dispatch(boost::bind(&type_t::dispatch_accept,this));
        _service.run();
    }
    void dispatch_accept()
    {
        post_accept();
    }


    /**
    *	\brief 響應請求的 asio 服務 數組
    *
    */
    services_info_t _services;


    /**
    *	\brief 響應線程
    *
    */
    boost::thread_group _threads;
    static void works(io_service_spt s)
    {
        s->run();
    }

private:
    std::size_t _flag;
    std::size_t _pos;
    /**
    *	\brief 返回一個 響應服務器
    *
    */
    service_info_spt get_service()
    {
        if(_flag)
        {
            --_flag;
            return _services[_pos];
        }

        std::size_t min = _services[0]->num();
        _pos = 0;
        for(std::size_t i=1; i<_services.size(); ++i)
        {
            std::size_t num = _services[0]->num();
            if(num < min)
            {
                min = num;
                _pos = i;
            }
        }
        _flag = _cnf->compare() - 1;
        return _services[_pos];
    }
    /**
    *	\brief 投遞一個 接收連接
    *
    */
    void post_accept(service_info_spt info=service_info_spt())
    {
        if(!info)
        {
            info = get_service();
        }

        try
        {

            socket_spt sock = boost::make_shared<socket_t>(info->get_io_service());
            _acceptor->async_accept(*sock,
                                    boost::bind(&tcp_answer_t::handler_accept,
                                                this,
                                                boost::asio::placeholders::error,
                                                info,
                                                sock
                                               )
                                   );

        }
        catch(const std::bad_alloc&)
        {
            //內存不足 等待一秒後 重新 post
            std::cerr<<"tcp_answer_t::post_accept bad_alloc";

            _timer.expires_from_now(boost::posix_time::seconds(1));
            _timer.async_wait(boost::bind(&tcp_answer_t::handler_timer_accept,this,_1,info));
        }
    }
    /**
    *	\brief 重新投遞 accept
    *
    */
    void handler_timer_accept(const boost::system::error_code& e,service_info_spt info)
    {
        if(e)
        {
            return;
        }

        post_accept(info);
    }

    /**
    *	\brief handler accept
    *
    */
    void handler_accept(const boost::system::error_code& ec,service_info_spt info,socket_spt sock)
    {
        post_accept();

        if(ec)
        {
            return;
        }

        //通知新連接
        session_t session = session_t();
        BOOST_AUTO(create_session,_cnf->create_session());
        if(create_session)
        {
            session = create_session(sock);
        }

        //post read
        info->dispatch_read(sock,session);
    }
};

};
};
#endif	//KG_NET_TCP_ANSWER_HEADER_HPP
