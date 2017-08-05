#ifndef KG_NET_ADDRESS_HEADER_HPP
#define KG_NET_ADDRESS_HEADER_HPP

#include <boost/asio.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/thread.hpp>

namespace kg
{
namespace net
{
	/**
	*	\brief 無效的 ip 地址
	*/
	class bad_address_t:public std::exception
	{
	public:
		virtual const char* what() const noexcept
		{
			return "address is invalid";
		}
	};
};
};
#endif	//KG_NET_ADDRESS_HEADER_HPP