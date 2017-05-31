#ifndef KG_ALLOCATOR_HEADER_HPP

namespace kg
{
	/**
	*	\brief 內存分配模式
	*
	*	將 new delte 進行了包裝\n
	*	以便 在某些組件的使用時 用戶可以 提供自己的 內存分配策略 以替代默認 分配方案
	*
	*	\param	T	內存中的數據型別
	*
	*/
	template<typename T>
	class allocator_t
	{
	public:
		/**
		*	\brief 動態分配內存 通常是一個 new
		*/
		inline T* create()const
		{
			return new T();
		}
		/**
		*	\brief 釋放動態分配的內存 通常是一個 delete
		*/
		inline void destory(T* p)const
		{
			delete p;
		}

		/**
		*	\brief 動態分配數組 通常是一個 new []
		*/
		inline T* create_array(std::size_t n)const
		{
			return new T[n];
		}
		/**
		*	\brief 釋放動態分配的數組 通常是一個 delete []
		*/
		inline void destory_array(T* p)const
		{
			delete[] p;
		}
	};
};
#endif // KG_ALLOCATOR_HEADER_HPP
