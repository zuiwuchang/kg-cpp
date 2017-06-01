#ifndef KG_SLICE_HEADER_HPP

#include "slice_impl.hpp"

#include <boost/smart_ptr.hpp>
namespace kg
{
	/**
	*	\brief	如同 go 的 slice
	*
	*	\attention	slice 保存的元素 必須符合 copy 語義 這就和 標準stl 容器一樣
	*
	*	\param	T	切片保存的數據 型別
	*	\param	Alloc	定義了如何 向os 申請釋放內存
	*/
	template<typename T,typename Alloc = allocator_t<T>>
	class slice_t
	{
	public:
		//type_t type_spt
		KG_TYPEDEF_TT(slice_t)
	private:
		typedef slice_impl<T,Alloc> impl_t;
		typedef typename impl_t::type_spt impl_spt;
		impl_spt _impl;

		slice_t(impl_spt impl)
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
			_impl = boost::make_shared<impl_t>(size,capacity);
		}
		~slice_t()
		{
		}

		/**
		*   \brief  返回 切片是否 一樣
		*
		*/
		inline bool operator==(const slice_t& compare)const
		{
			return impl_t::equal(*_impl,*(compare._impl));
		}

		/**
		*   \brief  返回 切片是否 不一樣
		*
		*/
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
		inline slice_t range(std::size_t begin)const
		{
			return slice_t(impl_t::range(*_impl,begin));
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
		inline slice_t range(std::size_t begin,std::size_t end)const
		{
			return slice_t(impl_t::range(*_impl,begin,end));
		}
		/**
		*   \brief  返回 正向迭代器 begin
		*
		*/

		/**
		*   \brief  如同go的 append
		*
		*	在 slice 尾添加 數據 返回添加成功後的 新slice
		*
		*	\exception	std::bad_alloc
		*
		*	\param	val	要添加的 新元素
		*	\return	新切片
		*
		*	\note	如果需要重新申請內存 slice_t會自動完成 此時 返回的 新slice 和原 slice 的內存模型 將指向不同的 地址
		*/
		inline slice_t append(const T& val)
		{
			return slice_t(impl_t::append(*_impl,val));
		}
		/**
		*   \brief  如同go的 append
		*
		*	在 slice 尾添加 數據 返回添加成功後的 新slice
		*
		*	\exception	std::bad_alloc
		*
		*	\param	val	要添加的 新元素
		*	\return	新切片
		*
		*	\note	如果需要重新申請內存 slice_t會自動完成 此時 返回的 新slice 和原 slice 的內存模型 將指向不同的 地址
		*/
		/*slice_t append(const T& val)
		{
			return slice_t;
		}*/

        inline T* begin()
        {
        	return _impl->begin();
        }
        /**
		*   \brief  返回 正向迭代器 end
		*
		*/
        inline T* end()
        {
        	return _impl->end();
        }
        /**
		*   \brief  返回 正向 const 迭代器 begin
		*
		*/
        inline const T* begin()const
        {
        	return _impl->begin();
        }
		/**
		*   \brief  返回 正向 const 迭代器 end
		*
		*/
        inline const T* end()const
        {
        	return _impl->end();
        }
	};
};
#endif // KG_SLICE_HEADER_HPP
