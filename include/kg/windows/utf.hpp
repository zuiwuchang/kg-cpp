#ifndef KG_WINDOWS_UTF_HEADER_HPP
#define KG_WINDOWS_UTF_HEADER_HPP

#include <boost/smart_ptr.hpp>
namespace kg
{
	namespace windows
	{
		/**
		*	\brief 字符串轉換
		*
		*/
		class utf
		{
		public:
			static std::wstring char_to_wchar(const std::string& str)
			{
				int size	=	MultiByteToWideChar(CP_ACP,0,str.c_str(),-1,0,0);
				boost::scoped_array<wchar_t> wcs(new wchar_t[size]);
			
				MultiByteToWideChar(CP_ACP,0,str.c_str(),-1,wcs.get(),size);

				return std::wstring(wcs.get());
			}
			static std::string wchar_to_char(const std::wstring& wcs)
			{
				int size	=	WideCharToMultiByte(CP_ACP,0,wcs.c_str(),-1,0,0,0,0);
				boost::scoped_array<char> str(new char[size]);

				WideCharToMultiByte(CP_ACP,0,wcs.c_str(),-1,str.get(),size,0,0);
			
				return std::string(str.get());
			}

			static std::wstring to_utf16(const std::string& utf8)
			{
				int size	=	MultiByteToWideChar(CP_UTF8,0,utf8.c_str(),-1,0,0);
				boost::scoped_array<wchar_t> utf16(new wchar_t[size]);
			
				MultiByteToWideChar(CP_UTF8,0,utf8.c_str(),-1,utf16.get(),size);

				return std::wstring(utf16.get());
			}
			static std::string to_utf8(const std::wstring& utf16)
			{
				int size	=	WideCharToMultiByte(CP_UTF8,0,utf16.c_str(),-1,0,0,0,0);
				boost::scoped_array<char> utf8(new char[size]);

				WideCharToMultiByte(CP_UTF8,0,utf16.c_str(),-1,utf8.get(),size,0,0);
			
				return std::string(utf8.get());
			}
		};
	};
};
#endif