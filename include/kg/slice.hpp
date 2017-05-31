#ifndef KG_SLICE_HEADER_HPP

#include "slice_impl.hpp"

#include <boost/smart_ptr.hpp>
namespace kg
{
	/**
	*	\brief	如同 go 的 slice
	*
	*	\param	T	切片保存的數據 型別
	*	\param	Alloc	定義了如何 向os 申請釋放內存
	*/
	template<typename T,typename Alloc = allocator_t<T>>
	class slice_t
	{
	protected:
		typedef slice_impl<T,Alloc> type_t;
		typedef boost::shared_ptr<type_t> type_spt;

		type_spt _impl;
	protected:
		slice_t(type_spt impl)
		{
			_impl = impl;
		}
	public:
		/**
		*   \brief  構造 一個 切片
		*
		*	\exception	std::bad_alloc
		*
		*	\param	size	切片大小
		*	\param	capacity	切片容量
		*
		*/
		slice_t(const std::size_t size=0,std::size_t capacity=0)
		{
			_impl = boost::make_shared<type_t>(size,capacity);
		}
		~slice_t()
		{
		}

		/**
		*   \brief  返回 切片是否 相等
		*
		*/
		bool operator==(const slice_t& compare)const
		{
			type_spt m1 = this->_impl;
			type_spt m2 = compare._impl;

			return m1->_array == m2->_array &&
				//m1->_capacity == m2->_capacity &&
				m1->_size == m2->_size &&
				m1->_pos == m2->_pos
				;
		}
		inline bool operator!=(const slice_t& compare)const
		{
			return !((*this) == compare);
		}

		/**
		*   \brief  訪問切片 元素
		*
		*	\attention	如 標註庫一樣 不會 檢查 越界
		*
		*/
		inline T& operator[](std::size_t i)
		{
			return (*_impl)[i];
		}
		/**
		*   \brief  訪問切片 元素
		*
		*	\exception std::out_of_range
		*
		*/
		inline T& at(std::size_t i)
		{
			return _impl->at(i);
		}

		/**
		*   \brief  訪問切片 元素
		*
		*	\attention	如 標註庫一樣 不會 檢查 越界
		*
		*/
		inline const T& operator[](std::size_t i)const
		{
			return (*_impl)[i];
		}
		/**
		*   \brief  訪問切片 元素
		*
		*	\exception std::out_of_range
		*
		*/
		inline const T& at(std::size_t i)const
		{
			return _impl->at(i);
		}

		/**
		*   \brief  返回切片 大小
		*
		*/
		inline std::size_t size()const
		{
			return _impl->size();
		}

		/**
		*   \brief  返回切片 容量
		*
		*/
		inline std::size_t capacity()const
		{
			return _impl->capacity();
		}

		/**
		*   \brief  切取 切片
		*
		*	\exception	std::bad_alloc
		*	\exception	std::out_of_range
		*
		*	\param	begin 切取位置 必須是 [0,size]
		*
		*	\return	子切片
		*/
		slice_t range(std::size_t begin)const
		{
			std::size_t size = _impl->_size;
			if(begin > size)
			{
				throw std::out_of_range("kg::slice_t.range begin > this->size()");
			}
			size -= begin;

			std::size_t pos = _impl->_pos;
			pos += begin;
			std::size_t capacity = _impl->_capacity;
			capacity -= begin;

			type_spt impl = boost::make_shared<type_t>();
			impl->_array = _impl->_array;
			impl->_pos = pos;
			impl->_size = size;
			impl->_capacity = capacity;
			return slice_t(impl);
		}
		/**
		*   \brief  切取 切片
		*
		*	\exception	std::bad_alloc
		*	\exception	std::out_of_range
		*
		*	\param	begin 切取位置 必須是 [0,capacity]
		*	\param	end 切取位置 必須是 [0,capacity]
		*
		*	\return	子切片
		*
		*	\attention	在傳入 end 時 允許 begin 取 (size,capacity] 之間的 值
		*	\attention	如果 end <= begin 子切片 的size 爲0
		*
		*/
		slice_t range(std::size_t begin,std::size_t end)const
		{
			std::size_t capacity = _impl->_capacity;
			if(begin > capacity)
			{
				throw std::out_of_range("kg::slice_t.range begin > this->capacity()");
			}
			if(end > capacity)
			{
				end = capacity;
			}
			capacity -= begin;
			std::size_t pos = _impl->_pos;
			pos += begin;

			std::size_t size = 0;
			if(begin < end)
			{
				size = end - begin;
			}

			type_spt impl = boost::make_shared<type_t>();
			impl->_array = _impl->_array;
			impl->_pos = pos;
			impl->_size = size;
			impl->_capacity = capacity;
			return slice_t(impl);
		}
		/**
		*   \brief  返回 正方迭代器 begin
		*
		*/
        inline T* begin()
        {
        	return _impl->begin();
        }
        /**
		*   \brief  返回 正方迭代器 end
		*
		*/
        inline T* end()
        {
        	return _impl->end();
        }
        /**
		*   \brief  返回 正方 const 迭代器 begin
		*
		*/
        inline const T* begin()const
        {
        	return _impl->begin();
        }
		/**
		*   \brief  返回 正方 const 迭代器 end
		*
		*/
        inline const T* end()const
        {
        	return _impl->end();
        }
	};
};
#endif // KG_SLICE_HEADER_HPP
