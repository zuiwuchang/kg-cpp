#ifndef KG_SLICE_IMPL_HEADER_HPP

#include "define.hpp"
#include "allocator.hpp"
#include <boost/noncopyable.hpp>
#include <boost/smart_ptr.hpp>
namespace kg
{
	/**
	*	\brief	[private] slice_t 的實現代碼
	*
	*	\param	T	切片保存的數據 型別
	*	\param	Alloc	定義了如何 向os 申請釋放內存
	*
	*	\warning	請使用 slice_t 不要直接使用 slice_impl
	*/
	template<typename T,typename Alloc>
	class slice_impl:boost::noncopyable
	{
	public:
		//type_t type_spt
		KG_TYPEDEF_TT(slice_impl)
	private:
		/**
		*	\brief	slice_impl 數據模型
		*
		*/
		class array_t
		{
		public:
			/**
			*	\brief	內存分片器
			*
			*/
			Alloc _alloc;
			/**
			*	\brief	內存地址 指針
			*
			*/
			T* _p;

			/**
			*	\brief	內存數組 大小
			*
			*/
			std::size_t _size;

			array_t(const std::size_t size):_size(size)
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

		/**
		*	\brief	slice_impl 數據模型
		*
		*/
		boost::shared_ptr<array_t> _array;
		/**
		*	\brief	容量
		*
		*/
		std::size_t _capacity;
		/**
		*	\brief	大小
		*
		*/
		std::size_t _size;
		/**
		*	\brief	偏移
		*
		*/
		std::size_t _pos;
	public:
		/**
		*	\brief	構造 slice_impl
		*
		*	\exception	std::bad_alloc
		*
		*	\param	size	切片大小
		*	\param	capacity	切片容量
		*
		*/
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

		/**
		*   \brief  返回 數組地址 或 nullptr
		*
		*/
		inline T* get()const
		{
			if(_size)
			{
				return _array->_p + _pos;
			}
			return nullptr;
		}

		/**
		*   \brief  返回切片 大小
		*
		*/
		inline std::size_t size()const
		{
			return _size;
		}

		/**
		*   \brief  返回切片 容量
		*
		*/
		inline std::size_t capacity()const
		{
			return _capacity;
		}

		/**
		*   \brief  返回 兩個 切片是否 一樣
		*
		*	\param	l	待比較的切片
		*	\param	r	待比較的切片
		*
		*	\return	如果一樣 返回 true
		*/
		static bool equal(const slice_impl& l,const slice_impl& r)
		{
			return l._array == r._array &&
				//m1->_capacity == m2->_capacity &&
				l._size == r._size &&
				l._pos == r._pos
				;
		}

		/**
		*   \brief  切取 切片
		*
		*	\exception	std::bad_alloc
		*	\exception	std::out_of_range
		*
		*	\param	slice 被切取 的原切片
		*	\param	begin 切取位置 必須是 [0,size]
		*
		*	\return	子切片的 impl 智能指針
		*/
		static type_spt range(const slice_impl& slice,std::size_t begin)
		{
			std::size_t size = slice._size;
			if(begin > size)
			{
				throw std::out_of_range("kg::slice_t.range begin > this->size()");
			}

			type_spt impl = boost::make_shared<type_t>();
			impl->_array = slice._array;
			impl->_pos = slice._pos + begin;
			impl->_size = size - begin;
			impl->_capacity = slice._capacity - begin;

			return impl;
		}
		/**
		*   \brief  切取 切片
		*
		*	\exception	std::bad_alloc
		*	\exception	std::out_of_range
		*
		*	\param	slice 被切取 的原切片
		*	\param	begin 切取位置 必須是 [0,capacity]
		*	\param	end 切取位置 必須是 [0,capacity]
		*
		*	\return	子切片的 impl 智能指針
		*
		*	\attention	在傳入 end 時 允許 begin 取 (size,capacity] 之間的 值
		*	\attention	如果 end <= begin 子切片 的size 爲0
		*
		*/
		static type_spt range(const slice_impl& slice,std::size_t begin,std::size_t end)
		{
			std::size_t capacity = slice._capacity;
			if(begin > capacity)
			{
				throw std::out_of_range("kg::slice_t.range begin > this->capacity()");
			}
			if(end > capacity)
			{
				end = capacity;
			}

			std::size_t size = 0;
			if(begin < end)
			{
				size = end - begin;
			}

			type_spt impl = boost::make_shared<type_t>();
			impl->_array = slice._array;
			impl->_pos = slice._pos + begin;
			impl->_size = size;
			impl->_capacity = capacity - begin;
			return impl;
		}

		/**
		*   \brief  如同go的 append
		*
		*	在 slice 尾添加 數據 返回添加成功後的 新slice
		*
		*	\exception	std::bad_alloc
		*
		*	\param	slice 被添加 的原切片
		*	\param	val	要添加的 新元素
		*	\return	新切片的 impl 智能指針
		*
		*	\note	如果需要重新申請內存 slice_t會自動完成 此時 返回的 新slice 和原 slice 的內存模型 將指向不同的 地址
		*/
		static type_spt append(const slice_impl& slice,const T& val)
		{
			type_spt impl = boost::make_shared<type_t>();
			std::size_t size = slice._size;
			if(slice._capacity > size)
			{
				//容量充足 直接 添加
				impl->_array = slice._array;
				impl->_pos = slice._pos;
				impl->_size = size + 1;
				impl->_capacity = slice._capacity;

				T* dist = impl->_array->_p;
				dist[size] = val;
			}
			else
			{
				//重新申請內存

				//新內存 容量
				std::size_t capacity = slice._capacity? slice._capacity * 2:1;

				//新 內存
				impl->_array = boost::make_shared<array_t>(capacity);
				impl->_pos = 0;
				impl->_size = size + 1;
				impl->_capacity = capacity;

				//copy 原數據
				T* dist = impl->_array->_p;
				if(slice._array)
				{
					T* src = slice.get();
					std::copy(src,src + slice._size,dist);
				}
				dist[size] = val;
			}
			return impl;
		}

		/**
		*   \brief  如同go的 append
		*
		*	在 slice 尾添加 數據 返回添加成功後的 新slice
		*
		*	\exception	std::bad_alloc
		*
		*	\param	slice 被添加 的原切片
		*	\param	arrs	要添加的 數組
		*	\param	size	數組 大小
		*	\return	新切片的 impl 智能指針
		*
		*	\note	如果需要重新申請內存 slice_t會自動完成 此時 返回的 新slice 和原 slice 的內存模型 將指向不同的 地址
		*/
		static type_spt append(const slice_impl& slice,const T* arrs,const std::size_t size)
		{
			type_spt impl = boost::make_shared<type_t>();
			std::size_t new_size = slice._size + size;
			if(slice._capacity >= new_size)
			{
				//容量充足 直接 添加
				impl->_array = slice._array;
				impl->_pos = slice._pos;
				impl->_size = new_size;
				impl->_capacity = slice._capacity;
				if(size)
				{
					T* dist = impl->_array->_p;
					std::copy(arrs,arrs+size,dist + slice._pos + slice._size);
				}
			}
			else
			{

				//重新申請內存

				//新內存 容量
				std::size_t capacity = slice._capacity? slice._capacity * 2:1;
				if(capacity < new_size)
				{
					capacity = new_size;
				}

				//新 內存
				impl->_array = boost::make_shared<array_t>(capacity);
				impl->_pos = 0;
				impl->_size = new_size;
				impl->_capacity = capacity;

				//copy 原數據
				T* dist = impl->_array->_p;
				if(slice._array)
				{
					T* src = slice.get();
					std::copy(src,src + slice._size,dist);
				}
				std::copy(arrs,arrs+size,dist + slice._size);
			}
			return impl;
		}
		/**
		*   \brief  如同go的 append
		*
		*	在 slice 尾添加 數據 返回添加成功後的 新slice
		*
		*	\exception	std::bad_alloc
		*
		*	\param	slice 被添加 的原切片
		*	\param	slice1	要添加的 數組
		*	\return	新切片的 impl 智能指針
		*
		*	\note	如果需要重新申請內存 slice_t會自動完成 此時 返回的 新slice 和原 slice 的內存模型 將指向不同的 地址
		*/
		static type_spt append(const slice_impl& slice,const slice_impl& slice1)
		{
			if(slice1._size)
			{
				return append(slice,slice1.get(),slice1._size);
			}
			return append(slice,nullptr,0);
		}

		/**
		*   \brief  如同go的 copy
		*
		*	\param	源數組	地址
		*	\param	size	數組 大小
		*	\return	拷貝數據大小
		*
		*	\note   如果 size 小於 當前切片 長度 當前切片長度不會縮小 僅僅copy數組罷\n
		*	如果 size 大於 當前切片長度 不會 copy 溢出的 數據
		*/
		std::size_t copy_from(const T* src,const std::size_t size)
		{
			if(!_size || !size)
			{
				return 0;
			}
			std::size_t min = std::min(_size,size);
			if(min)
			{
				std::copy(src,src + min,get());
				return min;
			}
			return 0;
		}

		/**
		*   \brief  如同go的 copy
		*
		*	\param	源切片
		*	\return	拷貝數據大小
		*
		*	\note   如果 size 小於 當前切片 長度 當前切片長度不會縮小 僅僅copy數組罷\n
		*	如果 size 大於 當前切片長度 不會 copy 溢出的 數據
		*/
		inline std::size_t copy_from(const slice_impl& slice)
		{
			if(_size && slice._size)
			{
				return copy_from(slice.get(),slice._size);
			}
			return 0;
		}

		/**
		*   \brief  訪問切片 元素
		*
		*	\attention	如 標註庫一樣 不會 檢查 越界
		*
		*/
		inline T& operator[](const std::size_t i)
		{
			return (_array->_p)[i + _pos];
		}

		/**
		*   \brief  訪問切片 元素
		*
		*	\exception std::out_of_range
		*
		*/
		T& at(std::size_t i)
		{
			if(i >= _size)
			{
				throw std::out_of_range("kg::slice_impl.at index >= this->size()");
			}
			i += _pos;
			return (_array->_p)[i];
		}

		/**
		*   \brief  訪問切片 元素
		*
		*	\attention	如 標註庫一樣 不會 檢查 越界
		*
		*/
		inline const T& operator[](const std::size_t i)const
		{
			return (_array->_p)[i + _pos];
		}

		/**
		*   \brief  訪問切片 元素
		*
		*	\exception std::out_of_range
		*
		*/
		const T& at(std::size_t i)const
		{
			if(i >= _size)
			{
				throw std::out_of_range("kg::slice_impl.at const index >= this->size()");
			}
			i += _pos;
			return (_array->_p)[i];
		}

		/**
		*   \brief  返回 正向迭代器 begin
		*
		*/
		T* begin()
		{
			if(_size)
			{
				return (_array->_p) + _pos;
			}
			return 0;
		}

		/**
		*   \brief  返回 正向迭代器 end
		*
		*/
		T* end()
		{
			if(_size)
			{
				return (_array->_p) + _pos + _size;
			}
			return 0;
		}

		/**
		*   \brief  返回 正向 const 迭代器 begin
		*
		*/
		const T* begin()const
		{
			if(_size)
			{
				return (_array->_p) + _pos;
			}
			return 0;
		}

		/**
		*   \brief  返回 正向 const 迭代器 end
		*
		*/
		const T* end()const
		{
			if(_size)
			{
				return (_array->_p) + _pos + _size;
			}
			return 0;
		}
	};
};
#endif // KG_SLICE_IMPL_HEADER_HPP
