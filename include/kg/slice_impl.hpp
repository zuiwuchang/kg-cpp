#ifndef KG_SLICE_IMPL_HEADER_HPP

#include "allocator.hpp"
#include <boost/noncopyable.hpp>
#include <boost/smart_ptr.hpp>
namespace kg
{
	template<typename T,typename Alloc>
	class slice_impl:boost::noncopyable
	{
	protected:
		class array_t
		{
		public:
			Alloc _alloc;
			T* _p;
			array_t(const std::size_t size)
			{
				_p = _alloc.create_array(size);
			}
			~array_t()
			{
				if(_p)
				{
					_alloc.destory_array(_p);
				}
			}
		};
		typedef T type;

	public:
		boost::shared_ptr<array_t> _array;
		std::size_t _capacity;
		std::size_t _size;
		std::size_t _pos;
	public:
		slice_impl(const std::size_t size=0,std::size_t capacity=0)
			:_pos(0)
		{
			if(capacity < size)
			{
				capacity = size;
			}
			_size = size;
			_capacity = capacity;

			if(capacity)
			{
				_array = boost::make_shared<array_t>(capacity);
			}
		}
		~slice_impl()
		{

		}

		inline std::size_t size()const
		{
			return _size;
		}
		inline std::size_t capacity()const
		{
			return _capacity;
		}

		inline T& operator[](const std::size_t i)
		{
			return (_array->_p)[i + _pos];
		}
		T& at(std::size_t i)
		{
			if(i >= _size)
			{
				throw std::out_of_range("kg::slice_impl.at index >= this->size()");
			}
			i += _pos;
			return (_array->_p)[i];
		}
		inline const T& operator[](const std::size_t i)const
		{
			return (_array->_p)[i + _pos];
		}
		const T& at(std::size_t i)const
		{
			if(i >= _size)
			{
				throw std::out_of_range("kg::slice_impl.at const index >= this->size()");
			}
			i += _pos;
			return (_array->_p)[i];
		}

		T* begin()
		{
			if(_size)
			{
				return (_array->_p) + _pos;
			}
			return 0;
		}
		T* end()
		{
			if(_size)
			{
				return (_array->_p) + _pos + _size;
			}
			return 0;
		}
		const T* begin()const
		{
			if(_size)
			{
				return (_array->_p) + _pos;
			}
			return 0;
		}
		const T* end()const
		{
			if(_size)
			{
				return (_array->_p) + _pos + _size;
			}
			return 0;
		}

		class slice_t;
		friend class slice_t;
	};
};
#endif // KG_SLICE_IMPL_HEADER_HPP
