//需要用到的 型別 定義
#ifndef KG_BYTES_FRAGMENTATION_HEADER_HPP
#define KG_BYTES_FRAGMENTATION_HEADER_HPP

#include "../types.hpp"
#include "../allocator.hpp"

namespace kg
{

/**
*	\brief 字節相關
*/
namespace bytes
{
	typedef kg::byte_t byte_t;

	/**
	*	\brief 數據 分片緩衝區
	*
	*	bytes_t 的分片數據\n
	*	爲建立 bytes_t 緩存 提供了 方便操作的 函數
	*/
	template<typename Alloc = kg::allocator_t<byte_t>>
	class fragmentation_t
	{
	private:
		Alloc _alloc;

		/**
		*	\brief 內部保存的 數組指針
		*
		*/
		byte_t* _array;

		/**
		*	\brief 內部數組 容量
		*/
		std::size_t _capacity;
		/**
		*	\brief 內部數組 偏移
		*/
		std::size_t _offset;
		/**
		*	\brief 內部數組 大小
		*/
		std::size_t _size;
	public:
		/**
		*	\brief	構造一個指定容量的 分片數據
		*/
		explicit fragmentation_t(const std::size_t size):
			_capacity(0),_offset(0),_size(0),_array(NULL)
		{
			if(!size)
			{
				return;
			}
			try
			{
				_array = _alloc.create_array(size);
				_capacity = size;
			}
			catch(const std::bad_alloc&)
			{
			}
		}
		virtual ~fragmentation_t()
		{
			reset();
		}
	private:
		fragmentation_t(const fragmentation_t& copy);
		fragmentation_t& operator=(const fragmentation_t& copy);
	public:
		/**
		*	\brief 返回 分片 是否不為空
		*/
		inline operator bool()const
		{
			return _capacity != 0;
		}
		/**
		*	\brief 返回 分片 是否為空
		*/
		inline bool empty()const
		{
			return _capacity == 0;
		}

		/**
		*	\brief 重置 分片
		*
		*	此後分片 大小 偏移 爲0  容量不變
		*/
		inline void reinit()
		{
			_offset = _size = 0;
		}
		/**
		*	\brief 釋放 分片
		*
		*	此後分片 大小 偏移 容量 爲0\n
		*	並且釋放數組
		*/
		inline void reset()
		{
			_capacity = _offset = _size = 0;
			if(_array)
			{
				_alloc.destroy_array(_array);
			}
		}

		/**
		*	\brief 返回 容量
		*/
		inline std::size_t capacity() const
		{
			return _capacity;
		}
		/**
		*	\brief 返回 有效數據 實際大小
		*/
		inline std::size_t size() const
		{
			return _size;
		}
		/**
		*	\brief 返回 空閒 容量
		*/
		inline std::size_t get_free()
		{
			return _capacity - _offset - _size;
		}

		/**
		*	\brief 在分片尾寫入數據
		*
		*	\param bytes 待寫入數據指針
		*	\param n 待寫入數據大小
		*
		*	\return	實際寫入大小
		*/
		std::size_t write(const byte_t* bytes,const std::size_t n)
		{
			std::size_t free = get_free();
			std::size_t need = n;
			if(need > free)
			{
				need = free;
			}
			memcpy(_array + _offset + _size,bytes,need * sizeof(byte_t));
			_size += need;

			return need;
		}

		/**
		*	\brief 讀取 數據
		*
		*	從分片頭讀取數據\n
		*	被讀取的 數據 將被 移除 緩衝區
		*
		*	\param bytes 待讀取數據指針
		*	\param n 待讀取數據大小
		*
		*	\return	實際讀取大小
		*/
		std::size_t read(byte_t* bytes,const std::size_t n)
		{
			std::size_t need = n;
			if(need > _size)
			{
				need = _size;
			}

			memcpy(bytes,_array + _offset,need * sizeof(byte_t));
			_size -= need;
			_offset += need;

			return need;
		}

		/**
		*	\brief 拷貝 數據
		*
		*	同 fragmentation_t::read \n
		*	被讀取的 數據 不會 從緩衝區 移除
		*
		*	\param bytes 待讀取數據指針
		*	\param n 待讀取數據大小
		*
		*	\return	實際讀取大小
		*/
		std::size_t copy_to(byte_t* bytes,const std::size_t n)const
		{
			std::size_t need = n;
			if (n > _size)
			{
				need = _size;
			}
			memcpy(bytes,_array + _offset,need * sizeof(byte_t));

			return need;
		}

		/**
		*	\brief 拷貝 數據
		*
		*	同 fragmentation_t::copy \n
		*	但 跳過 緩衝區 前 skip 個 字節
		*
		*	\param skip 要跳過的字節數
		*	\param bytes 待讀取數據指針
		*	\param n 待讀取數據大小
		*
		*	\return	實際讀取大小
		*
		*/
		std::size_t copy_to(const std::size_t skip,byte_t* bytes,const std::size_t n)const
		{
			if(skip >= _size)
			{
				return 0;
			}
			std::size_t offset = _offset + skip;
			std::size_t size = _size - skip;

			std::size_t need = n;
			if (need > size)
			{
				need = size;
			}
			memcpy(bytes,_array + offset,need * sizeof(byte_t));

			return need;
		}

	};

};
};

#endif // KG_BYTES_FRAGMENTATION_HEADER_HPP
