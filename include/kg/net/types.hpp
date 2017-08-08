#ifndef KG_NET_TYPES_HEADER_HPP
#define KG_NET_TYPES_HEADER_HPP

#include <boost/asio.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/thread.hpp>

namespace kg
{
namespace net
{
	/*	typedef boost	*/
	typedef boost::asio::io_service io_service_t;
	typedef boost::asio::io_service::work work_t;
	typedef boost::asio::ip::tcp::acceptor acceptor_t;
	typedef boost::asio::ip::tcp::socket socket_t;
	typedef boost::asio::ip::tcp::endpoint endpoint_t;
	typedef boost::shared_ptr<io_service_t> io_service_spt;
	typedef boost::shared_ptr<work_t> work_spt;
	typedef boost::shared_ptr<acceptor_t> acceptor_spt;
	typedef boost::shared_ptr<socket_t> socket_spt;
	typedef boost::shared_ptr<endpoint_t> endpoint_spt;

	typedef boost::thread thread_t;
	typedef boost::shared_ptr<thread_t> thread_spt;

	typedef boost::asio::deadline_timer deadline_timer_t;
};
};
#endif	//KG_NET_TYPES_HEADER_HPP
