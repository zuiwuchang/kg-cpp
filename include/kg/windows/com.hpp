#ifndef KG_WINDOWS_COM_HEADER_HPP
#define KG_WINDOWS_COM_HEADER_HPP

#include <windows.h>
#include <rpcsal.h>
#include <crtdbg.h>
#include <netfw.h>
#include <objbase.h>
#include <oleauto.h>

#pragma comment( lib, "ole32.lib" )
#pragma comment( lib, "oleaut32.lib" )

#include <boost/system/system_error.hpp>
#include <boost/exception/all.hpp>
namespace kg
{
	namespace windows
	{
#define KG_SAFE_RELEASE(x) if((x)!=NULL){(x)->Release();(x)=NULL;}

	};
};
#endif // KG_WINDOWS_COM_HEADER_HPP
