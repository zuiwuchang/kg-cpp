#ifndef KG_UTILS_SINGLETON_HEADER_HPP
#define KG_UTILS_SINGLETON_HEADER_HPP

#include <boost/noncopyable.hpp>
namespace kg
{
namespace utils
{
	template<typename T>
	class singleton
		: boost::noncopyable
	{
	private:
		static T& get()
		{
			static T instance;
			return instance;
		}
	public:
		static const T & get_const_instance()
		{
			return get();
		}
		static T & get_mutable_instance()
		{
			return get();
		}
	};
};
};
#endif // KG_UTILS_SINGLETON_HEADER_HPP
