#ifndef KG_DEFER_HEADER_HPP
#define KG_DEFER_HEADER_HPP

#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
namespace kg
{
	/**
	*	\brief 如同 go 的 defer
	*
	*   創建一個對象實例 存儲一個 boost::function<void()>\n
	*   當對象 析構時 自動調用此 function 用於 進行清理工作\n
	*
	*   \code
	#include <iostream>
	#include <kg/defer.hpp>

	int main()
	{
		int x = 0;
		{
			kg::defer_t([&]()
			{
				std::cout<<"step 0 x="<<x++<<"\n";
			});

			kg::defer_t([&]()
			{
				std::cout<<"step 1 x="<<x++<<"\n";
			});
		}

		kg::defer_t([&]()
		{
			std::cout<<"step 2 x="<<x++<<"\n";
		});

		return 0;
	}
	*   \endcode
	*   step 0 x=0\n
	*   step 1 x=1\n
	*   step 2 x=2\n
	*/
	class defer_t:boost::noncopyable
	{
	private:
		boost::function<void()> _func;
	public:
		/**
		*   \brief  構造 defer_t
		*/
		defer_t(boost::function<void()> func):_func(func)
		{

		}
		~defer_t()
		{
			if(_func)
			{
				_func();
			}
		}
	};
};
#endif // KG_DEFER_HEADER_HPP
