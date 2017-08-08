#ifndef KG_NET_TCP_ANSWER_CLIENT_HEADER_HPP
#define KG_NET_TCP_ANSWER_CLIENT_HEADER_HPP

#include <vector>

#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/typeof/typeof.hpp>
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include "../types.hpp"
#include "../bytes/buffer.hpp"
#include "types.hpp"
#include "bad_address.hpp"
#include "tcp_answer_client_configure.hpp"


namespace kg
{
namespace net
{
/**
*	\brief tcp_answer_t 的一個 配套 同步 客戶端 實現
*
*	\param T	一個支持copy語義的 自定義 對象 可以用來存儲自定義的 session 數據
*
*/
template<typename T>
class tcp_answer_client_t
    :boost::noncopyable
{
public:
	/**
    *	\brief class 型別
    *
    */
    typedef tcp_answer_client_t type_t;

    /**
    *	\brief session 型別
    *
    */
    typedef T session_t;

    typedef	tcp_answer_client_configure_t<session_t> 	configure_t;
    typedef	boost::shared_ptr<configure_t> configure_spt;
private:
    configure_spt _cnf;

    io_service_t _service;
	socket_spt _sock;
	endpoint_spt _endpoint;
	session_t _session;
public:
	/**
    *	\brief 啓動服務器 並監聽 指定地址
    *
    *	\exception class std::bad_alloc
    *	\exception class bad_address_t
    *
    *	\param addr		形如 :port XXX:port 的連接地址
    *	\param cnf		客戶端配置
    *
    */
	tcp_answer_client_t(std::string addr,configure_spt cnf)
		:_cnf(cnf)
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

        _endpoint = boost::make_shared<endpoint_t>(boost::asio::ip::address::from_string(ip),port);
		_sock = boost::make_shared<socket_t>(_service);
    }
    ~tcp_answer_client_t()
    {
    	close_socket();
    }
    /**
    *	\brief 返回 asio socket
    *
    *
    */
    socket_spt socket()
    {
    	return _sock;
    }

	/**
    *	\brief 同步服務器 客戶端
    *
    */
  	void connect(boost::system::error_code& ec)
    {
    	_sock->connect(*_endpoint,ec);
    	if(ec)
		{
			return;
		}
		BOOST_AUTO(create_session,_cnf->create_session());
		if(create_session)
		{
			_session = create_session(_sock);
		}
		else
		{
			_session = session_t();
		}
    }

    /**
    *	\brief 同步服務器 客戶端
    *
    *	\exception class boost::system::system_error
    */
	void connect()
    {
    	_sock->connect(*_endpoint);
		BOOST_AUTO(create_session,_cnf->create_session());
		if(create_session)
		{
			_session = create_session(_sock);
		}
		else
		{
			_session = session_t();
		}
    }

private:
	void close_socket()
	{
		if(!_sock)
		{
			return;
		}

		boost::system::error_code ec;
		_sock->shutdown(boost::asio::ip::tcp::socket::shutdown_both,ec);
		BOOST_AUTO(destroy_session,_cnf->destroy_session());
		if(destroy_session)
		{
			destroy_session(_sock,_session);
		}
		_sock->close(ec);
		_sock.reset();
	}
};

};
};
#endif	//KG_NET_TCP_ANSWER_CLIENT_HEADER_HPP
