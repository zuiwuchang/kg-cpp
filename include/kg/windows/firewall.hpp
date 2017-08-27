#ifndef KG_WINDOWS_FIREWALL_HEADER_HPP
#define KG_WINDOWS_FIREWALL_HEADER_HPP


#include "com.hpp"
#include "utf.hpp"

#include <rpcsal.h>
#include <crtdbg.h>
#include <netfw.h>
#include <objbase.h>
#include <oleauto.h>

#pragma comment( lib, "ole32.lib" )
#pragma comment( lib, "oleaut32.lib" )

#include <boost/system/system_error.hpp>
#include <boost/exception/all.hpp>
#include <boost/noncopyable.hpp>
namespace kg
{
	namespace windows
	{
#define KG_WINDOWS_FIREWALL_CATEGORY_CoCreateInstance					1
#define KG_WINDOWS_FIREWALL_CATEGORY_get_LocalPolicy						2
#define KG_WINDOWS_FIREWALL_CATEGORY_get_CurrentProfile					3
#define KG_WINDOWS_FIREWALL_CATEGORY_get_FirewallEnabled				4
#define KG_WINDOWS_FIREWALL_CATEGORY_put_FirewallEnabled				5
#define KG_WINDOWS_FIREWALL_CATEGORY_get_AuthorizedApplications	6
#define KG_WINDOWS_FIREWALL_CATEGORY_SysAllocString						7
#define KG_WINDOWS_FIREWALL_CATEGORY_get_Enabled							8
#define KG_WINDOWS_FIREWALL_CATEGORY_put_Enabled							9
#define KG_WINDOWS_FIREWALL_CATEGORY_put_ProcessImageFileName	10
#define KG_WINDOWS_FIREWALL_CATEGORY_put_Name								11
#define KG_WINDOWS_FIREWALL_CATEGORY_AddRule									12
#define KG_WINDOWS_FIREWALL_CATEGORY_RemoveRule							13
#define KG_WINDOWS_FIREWALL_CATEGORY_get_GloballyOpenPorts			14
#define KG_WINDOWS_FIREWALL_CATEGORY_put_Port									15
#define KG_WINDOWS_FIREWALL_CATEGORY_put_Protocol							16
		/**
		*	\brief firewall_t 異常 定義
		*
		*/
		class firewall_category :
			public boost::system::error_category
		{
		public:
			virtual const char *name() const BOOST_SYSTEM_NOEXCEPT
			{
				return "firewall";
			}
			virtual std::string message(int ev) const
			{
    			std::string msg("firewall : ");
				switch(ev)
				{
				case 0:
					return msg + "success";
				case KG_WINDOWS_FIREWALL_CATEGORY_CoCreateInstance:
					return msg + "CoCreateInstance failed";
				case KG_WINDOWS_FIREWALL_CATEGORY_get_LocalPolicy:
					return msg + "get_LocalPolicy failed";
				case KG_WINDOWS_FIREWALL_CATEGORY_get_CurrentProfile:
					return msg + "get_CurrentProfile failed";
				case KG_WINDOWS_FIREWALL_CATEGORY_get_FirewallEnabled:
					return msg + "get_FirewallEnabled failed";
				case KG_WINDOWS_FIREWALL_CATEGORY_put_FirewallEnabled:
					return msg + "put_FirewallEnabled failed";
				case KG_WINDOWS_FIREWALL_CATEGORY_get_AuthorizedApplications:
					return msg + "get_AuthorizedApplications failed";
				case KG_WINDOWS_FIREWALL_CATEGORY_SysAllocString:
					return msg + "SysAllocString failed";
				case KG_WINDOWS_FIREWALL_CATEGORY_get_Enabled:
					return msg + "get_Enabled failed";
				case KG_WINDOWS_FIREWALL_CATEGORY_put_Enabled:
					return msg + "put_Enabled failed";
				case KG_WINDOWS_FIREWALL_CATEGORY_put_ProcessImageFileName:
					return msg + "put_ProcessImageFileName failed";
				case KG_WINDOWS_FIREWALL_CATEGORY_put_Name:
					return msg + "put_Name failed";
				case KG_WINDOWS_FIREWALL_CATEGORY_AddRule:
					return msg + "AddRule failed";
				case KG_WINDOWS_FIREWALL_CATEGORY_RemoveRule:
					return msg + "RemoveRule failed";
				case KG_WINDOWS_FIREWALL_CATEGORY_get_GloballyOpenPorts:
					return msg + "get_GloballyOpenPorts failed";
				case KG_WINDOWS_FIREWALL_CATEGORY_put_Port:
					return msg + "put_Port failed";
				case KG_WINDOWS_FIREWALL_CATEGORY_put_Protocol:
					return msg + "put_Protocol failed";
				}
				return msg + "unknow";
			}
			static firewall_category& get()
			{
    			static firewall_category instance;
    			return instance;
			}
		};
		/**
		*	\brief 封裝了對 window 防火牆操作 的封裝
		*
		*/
		class firewall_t:boost::noncopyable
		{
		public:
			/**
			*	\brief 創建一個 防火牆操作 組建
			*
			*	\exception boost::system::system_error
			*/
			firewall_t()
                :_profile(NULL)
			{
				HRESULT hr = S_OK;  
				INetFwMgr* fwMgr = NULL;  
				INetFwPolicy* fwPolicy = NULL; 

				hr = CoCreateInstance(  
					__uuidof(NetFwMgr),  
					NULL,  
					CLSCTX_INPROC_SERVER,  
					__uuidof(INetFwMgr),  
					(void **)&fwMgr);  
  
				if (FAILED(hr))  
				{
					KG_SAFE_RELEASE(fwMgr);
					KG_SAFE_RELEASE(fwPolicy);
					BOOST_THROW_EXCEPTION(boost::system::system_error(KG_WINDOWS_FIREWALL_CATEGORY_CoCreateInstance,firewall_category::get()));
				}
				
  
				hr = fwMgr->get_LocalPolicy(&fwPolicy);  
				if (FAILED(hr))  
				{
					KG_SAFE_RELEASE(fwMgr);
					KG_SAFE_RELEASE(fwPolicy);
					BOOST_THROW_EXCEPTION(boost::system::system_error(KG_WINDOWS_FIREWALL_CATEGORY_get_LocalPolicy,firewall_category::get()));
				}  
  
				hr = fwPolicy->get_CurrentProfile(&_profile);  
				if (FAILED(hr))  
				{
					KG_SAFE_RELEASE(fwMgr);
					KG_SAFE_RELEASE(fwPolicy);
					BOOST_THROW_EXCEPTION(boost::system::system_error(KG_WINDOWS_FIREWALL_CATEGORY_get_CurrentProfile,firewall_category::get()));
				}

				KG_SAFE_RELEASE(fwMgr);
				KG_SAFE_RELEASE(fwPolicy);
			}
			~firewall_t()
			{
				KG_SAFE_RELEASE(_profile);
			}
        private:
            INetFwProfile* _profile;

		public:
			/**
			*	\brief 返回 防火牆 是否處於打開狀態
			*
			*	\exception boost::system::system_error
			*/
			bool is_on()
			{
				HRESULT hr = S_OK;  
				VARIANT_BOOL fwEnabled;  
 
				hr = _profile->get_FirewallEnabled(&fwEnabled);  
				if (FAILED(hr))  
				{
					BOOST_THROW_EXCEPTION(boost::system::system_error(KG_WINDOWS_FIREWALL_CATEGORY_get_FirewallEnabled,firewall_category::get()));
				}
				
				return fwEnabled != VARIANT_FALSE;
			}

			/**
			*	\brief 關閉防火牆
			*
			*	\exception boost::system::system_error
			*/
			void turn_off()
			{
				if(!is_on())
				{
					return;
				}

				HRESULT hr = _profile->put_FirewallEnabled(VARIANT_FALSE);  
				if (FAILED(hr))  
				{
					BOOST_THROW_EXCEPTION(boost::system::system_error(KG_WINDOWS_FIREWALL_CATEGORY_put_FirewallEnabled,firewall_category::get()));
				}
			}

			/**
			*	\brief 打開防火牆
			*
			*	\exception boost::system::system_error
			*/
			void turn_on()
			{
				if(is_on())
				{
					return;
				}

				HRESULT hr = _profile->put_FirewallEnabled(VARIANT_TRUE);  
				if (FAILED(hr))  
				{
					BOOST_THROW_EXCEPTION(boost::system::system_error(KG_WINDOWS_FIREWALL_CATEGORY_put_FirewallEnabled,firewall_category::get()));
				}
			}
			/**
			*	\brief 返回 一個 程式 是否被 防火牆 放行
			*
			*	\exception boost::system::system_error
			*	\param path 程式全路徑 utf8 字符串
			*/
			inline bool app_rule(const std::string path)
			{
				return app_status(utf::to_utf16(path));
			}

			/**
			*	\brief 返回 一個 程式 是否被 防火牆 放行
			*
			*	\exception boost::system::system_error
			*	\param path 程式全路徑 utf8 字符串
			*/
			inline bool app_status(const std::string path)
			{
				return app_status(utf::to_utf16(path));
			}
			/**
			*	\brief 返回 一個 程式 是否被 防火牆 放行
			*
			*	\exception boost::system::system_error
			*	\param path 程式全路徑
			*/
			bool app_status(const std::wstring path)
			{
				HRESULT hr = S_OK;  
				BSTR fwBstrProcessImageFileName = NULL;  
				VARIANT_BOOL fwEnabled;  
				INetFwAuthorizedApplication* fwApp = NULL;  
				INetFwAuthorizedApplications* fwApps = NULL;  
  
				//獲取授權程式集
				hr = _profile->get_AuthorizedApplications(&fwApps);  
				if (FAILED(hr))  
				{
					BOOST_THROW_EXCEPTION(boost::system::system_error(KG_WINDOWS_FIREWALL_CATEGORY_get_AuthorizedApplications,firewall_category::get()));
				}

				fwBstrProcessImageFileName = SysAllocString(path.c_str());  
				if (fwBstrProcessImageFileName == NULL)  
				{
					KG_SAFE_RELEASE(fwApps);
					BOOST_THROW_EXCEPTION(boost::system::system_error(KG_WINDOWS_FIREWALL_CATEGORY_SysAllocString,firewall_category::get()));
				}  

				bool yes;
				hr = fwApps->Item(fwBstrProcessImageFileName, &fwApp);  
				if (SUCCEEDED(hr))  
				{  
					hr = fwApp->get_Enabled(&fwEnabled);  
					if (FAILED(hr))  
					{
						SysFreeString(fwBstrProcessImageFileName);
						KG_SAFE_RELEASE(fwApp);
						KG_SAFE_RELEASE(fwApps);
						BOOST_THROW_EXCEPTION(boost::system::system_error(KG_WINDOWS_FIREWALL_CATEGORY_get_Enabled,firewall_category::get()));	
					}
					yes = fwEnabled != VARIANT_FALSE;
				}
				else
				{
					yes = false;
				}
  
				SysFreeString(fwBstrProcessImageFileName);
				KG_SAFE_RELEASE(fwApp);
				KG_SAFE_RELEASE(fwApps);
				return yes;
			}
		
			/**
			*	\brief 添加規則 允許一個 程式 通過 防火牆
			*
			*	\exception boost::system::system_error
			*
			*	\param name 規則名稱
			*	\param path 程式全路徑
			*/
			void app_allow(const std::string& name,const std::string& path)
			{
				app_allow(utf::to_utf16(name),utf::to_utf16(path));
			}
			/**
			*	\brief 添加規則 允許一個 程式 通過 防火牆
			*
			*	\exception boost::system::system_error
			*
			*	\param name 規則名稱
			*	\param path 程式全路徑
			*/
			void app_allow(const std::wstring& name,const std::wstring& path)
			{
				if(app_status(path))
				{
					return;
				}

				INetFwAuthorizedApplication* fwApp = NULL;
				INetFwAuthorizedApplications* fwApps = NULL;
				//獲取授權程式集
				HRESULT  hr = _profile->get_AuthorizedApplications(&fwApps);
				if (FAILED(hr))
				{
					BOOST_THROW_EXCEPTION(boost::system::system_error(KG_WINDOWS_FIREWALL_CATEGORY_get_AuthorizedApplications,firewall_category::get()));
				}
		
				// Create an instance of an authorized application.
				hr = CoCreateInstance(
					__uuidof(NetFwAuthorizedApplication),
					NULL,
					CLSCTX_INPROC_SERVER,
					__uuidof(INetFwAuthorizedApplication),
					(void**)&fwApp
					);
				if (FAILED(hr))
				{
					KG_SAFE_RELEASE(fwApps);
					BOOST_THROW_EXCEPTION(boost::system::system_error(KG_WINDOWS_FIREWALL_CATEGORY_CoCreateInstance,firewall_category::get()));
				}
		
				// Allocate a BSTR for the process image file name.
				BSTR fwBstrProcessImageFileName = SysAllocString(path.c_str());
				if (fwBstrProcessImageFileName == NULL)
				{
					KG_SAFE_RELEASE(fwApp);
					KG_SAFE_RELEASE(fwApps);
					BOOST_THROW_EXCEPTION(boost::system::system_error(KG_WINDOWS_FIREWALL_CATEGORY_SysAllocString,firewall_category::get()));
				}
		
				// Set the process image file name.
				hr = fwApp->put_ProcessImageFileName(fwBstrProcessImageFileName);
				if (FAILED(hr))
				{
					KG_SAFE_RELEASE(fwApp);
					KG_SAFE_RELEASE(fwApps);
					SysFreeString(fwBstrProcessImageFileName);
					BOOST_THROW_EXCEPTION(boost::system::system_error(KG_WINDOWS_FIREWALL_CATEGORY_put_ProcessImageFileName,firewall_category::get()));
				}
		
				// Allocate a BSTR for the application friendly name.
				BSTR fwBstrName = SysAllocString(name.c_str());
				if (SysStringLen(fwBstrName) == 0)
				{
					KG_SAFE_RELEASE(fwApp);
					KG_SAFE_RELEASE(fwApps);
					SysFreeString(fwBstrProcessImageFileName);
					BOOST_THROW_EXCEPTION(boost::system::system_error(KG_WINDOWS_FIREWALL_CATEGORY_put_ProcessImageFileName,firewall_category::get()));
				}
		
				// Set the application friendly name.
				hr = fwApp->put_Name(fwBstrName);
				if (FAILED(hr))
				{
					KG_SAFE_RELEASE(fwApp);
					KG_SAFE_RELEASE(fwApps);
					SysFreeString(fwBstrProcessImageFileName);
					SysFreeString(fwBstrName);
					BOOST_THROW_EXCEPTION(boost::system::system_error(KG_WINDOWS_FIREWALL_CATEGORY_put_Name,firewall_category::get()));
				}
		
				// Add the application to the collection.
				hr = fwApps->Add(fwApp);
				if (FAILED(hr))
				{
					KG_SAFE_RELEASE(fwApp);
					KG_SAFE_RELEASE(fwApps);
					SysFreeString(fwBstrProcessImageFileName);
					SysFreeString(fwBstrName);
					BOOST_THROW_EXCEPTION(boost::system::system_error(KG_WINDOWS_FIREWALL_CATEGORY_AddRule,firewall_category::get()));
				}

				KG_SAFE_RELEASE(fwApp);
				KG_SAFE_RELEASE(fwApps);
				SysFreeString(fwBstrProcessImageFileName);
				SysFreeString(fwBstrName);
			}
			
			/**
			*	\brief 添加規則 阻止一個 程式 通過 防火牆
			*
			*	\exception boost::system::system_error
			*
			*	\param name 規則名稱
			*	\param path 程式全路徑
			*/
			void app_disable(const std::string& name,const std::string& path)
			{
				app_disable(utf::to_utf16(name),utf::to_utf16(path));
			}
			/**
			*	\brief 添加規則 阻止一個 程式 通過 防火牆
			*
			*	\exception boost::system::system_error
			*
			*	\param name 規則名稱
			*	\param path 程式全路徑
			*/
			void app_disable(const std::wstring& name,const std::wstring& path)
			{
				if(!app_status(path))
				{
					return;
				}

				INetFwAuthorizedApplication* fwApp = NULL;
				INetFwAuthorizedApplications* fwApps = NULL;
				//獲取授權程式集
				HRESULT  hr = _profile->get_AuthorizedApplications(&fwApps);
				if (FAILED(hr))
				{
					BOOST_THROW_EXCEPTION(boost::system::system_error(KG_WINDOWS_FIREWALL_CATEGORY_get_AuthorizedApplications,firewall_category::get()));
				}
		
				// Create an instance of an authorized application.
				hr = CoCreateInstance(
					__uuidof(NetFwAuthorizedApplication),
					NULL,
					CLSCTX_INPROC_SERVER,
					__uuidof(INetFwAuthorizedApplication),
					(void**)&fwApp
					);
				if (FAILED(hr))
				{
					KG_SAFE_RELEASE(fwApps);
					BOOST_THROW_EXCEPTION(boost::system::system_error(KG_WINDOWS_FIREWALL_CATEGORY_CoCreateInstance,firewall_category::get()));
				}
		
				// Allocate a BSTR for the process image file name.
				BSTR fwBstrProcessImageFileName = SysAllocString(path.c_str());
				if (fwBstrProcessImageFileName == NULL)
				{
					KG_SAFE_RELEASE(fwApp);
					KG_SAFE_RELEASE(fwApps);
					BOOST_THROW_EXCEPTION(boost::system::system_error(KG_WINDOWS_FIREWALL_CATEGORY_SysAllocString,firewall_category::get()));
				}
		
				// Set the process image file name.
				hr = fwApp->put_ProcessImageFileName(fwBstrProcessImageFileName);
				if (FAILED(hr))
				{
					KG_SAFE_RELEASE(fwApp);
					KG_SAFE_RELEASE(fwApps);
					SysFreeString(fwBstrProcessImageFileName);
					BOOST_THROW_EXCEPTION(boost::system::system_error(KG_WINDOWS_FIREWALL_CATEGORY_put_ProcessImageFileName,firewall_category::get()));
				}
		
				// Allocate a BSTR for the application friendly name.
				BSTR fwBstrName = SysAllocString(name.c_str());
				if (SysStringLen(fwBstrName) == 0)
				{
					KG_SAFE_RELEASE(fwApp);
					KG_SAFE_RELEASE(fwApps);
					SysFreeString(fwBstrProcessImageFileName);
					BOOST_THROW_EXCEPTION(boost::system::system_error(KG_WINDOWS_FIREWALL_CATEGORY_put_ProcessImageFileName,firewall_category::get()));
				}
		
				// Set the application friendly name.
				hr = fwApp->put_Name(fwBstrName);
				if (FAILED(hr))
				{
					KG_SAFE_RELEASE(fwApp);
					KG_SAFE_RELEASE(fwApps);
					SysFreeString(fwBstrProcessImageFileName);
					SysFreeString(fwBstrName);
					BOOST_THROW_EXCEPTION(boost::system::system_error(KG_WINDOWS_FIREWALL_CATEGORY_put_Name,firewall_category::get()));
				}

				//disable
				hr = fwApp->put_Enabled(VARIANT_FALSE);
				if (FAILED(hr))
				{
					KG_SAFE_RELEASE(fwApp);
					KG_SAFE_RELEASE(fwApps);
					SysFreeString(fwBstrProcessImageFileName);
					SysFreeString(fwBstrName);
					BOOST_THROW_EXCEPTION(boost::system::system_error(KG_WINDOWS_FIREWALL_CATEGORY_put_Enabled,firewall_category::get()));
				}

				// Add the application to the collection.
				hr = fwApps->Add(fwApp);

				KG_SAFE_RELEASE(fwApp);
				KG_SAFE_RELEASE(fwApps);
				SysFreeString(fwBstrProcessImageFileName);
				SysFreeString(fwBstrName);

				if (FAILED(hr))
				{
					BOOST_THROW_EXCEPTION(boost::system::system_error(KG_WINDOWS_FIREWALL_CATEGORY_AddRule,firewall_category::get()));
				}
			}
			/**
			*	\brief 刪除 程式的 防火牆 規則
			*
			*	\exception boost::system::system_error
			*
			*	\param path 程式全路徑
			*/
			void app_remove(const std::string& path)
			{
				app_remove(utf::to_utf16(path));
			}
			/**
			*	\brief 刪除 程式的 防火牆 規則
			*
			*	\exception boost::system::system_error
			*
			*	\param path 程式全路徑
			*/
			void app_remove(const std::wstring& path)
			{
				INetFwAuthorizedApplications* fwApps = NULL;
				//獲取授權程式集
				HRESULT  hr = _profile->get_AuthorizedApplications(&fwApps);
				if (FAILED(hr))
				{
					BOOST_THROW_EXCEPTION(boost::system::system_error(KG_WINDOWS_FIREWALL_CATEGORY_get_AuthorizedApplications,firewall_category::get()));
				}
				BSTR fwBstrProcessImageFileName = SysAllocString(path.c_str());
				if (fwBstrProcessImageFileName == NULL)
				{
					KG_SAFE_RELEASE(fwApps);
					BOOST_THROW_EXCEPTION(boost::system::system_error(KG_WINDOWS_FIREWALL_CATEGORY_SysAllocString,firewall_category::get()));
				}
				hr = fwApps->Remove(fwBstrProcessImageFileName);
				if (FAILED(hr))
				{
					KG_SAFE_RELEASE(fwApps);
					SysFreeString(fwBstrProcessImageFileName);
					BOOST_THROW_EXCEPTION(boost::system::system_error(KG_WINDOWS_FIREWALL_CATEGORY_RemoveRule,firewall_category::get()));
				}
				KG_SAFE_RELEASE(fwApps);
				SysFreeString(fwBstrProcessImageFileName);
			}
			/**
			*	\brief 返回 一個 端口 是否被 防火牆 放行
			*
			*	\exception boost::system::system_error
			*
			*	\param port 端口
			*	\param protocol 協議 NET_FW_IP_PROTOCOL_TCP NET_FW_IP_PROTOCOL_UDP NET_FW_IP_PROTOCOL_ANY
			*/
			bool port_status(LONG port,NET_FW_IP_PROTOCOL protocol = NET_FW_IP_PROTOCOL_ANY)
			{
				VARIANT_BOOL fwEnabled;
				INetFwOpenPort* fwOpenPort = NULL;
				INetFwOpenPorts* fwOpenPorts = NULL;

				// Retrieve the globally open ports collection.
				HRESULT hr = _profile->get_GloballyOpenPorts(&fwOpenPorts);
				if (FAILED(hr))
				{
					BOOST_THROW_EXCEPTION(boost::system::system_error(KG_WINDOWS_FIREWALL_CATEGORY_get_GloballyOpenPorts,firewall_category::get()));
				}

				// Attempt to retrieve the globally open port.
				bool yes;
				hr = fwOpenPorts->Item(port, protocol, &fwOpenPort);
				if (SUCCEEDED(hr))
				{
					// Find out if the globally open port is enabled.
					hr = fwOpenPort->get_Enabled(&fwEnabled);
					if (FAILED(hr))
					{
						KG_SAFE_RELEASE(fwOpenPort);
						KG_SAFE_RELEASE(fwOpenPorts);
						BOOST_THROW_EXCEPTION(boost::system::system_error(KG_WINDOWS_FIREWALL_CATEGORY_get_Enabled,firewall_category::get()));	
					}

					yes = fwEnabled != VARIANT_FALSE;
				}
				else
				{
					yes= false;
				}
				KG_SAFE_RELEASE(fwOpenPort);
				KG_SAFE_RELEASE(fwOpenPorts);
				return yes;
			}
		
			/**
			*	\brief 添加規則 允許一個 端口 通過 防火牆
			*
			*	\exception boost::system::system_error
			*
			*	\param name 規則名
			*	\param port 端口
			*	\param protocol 協議 NET_FW_IP_PROTOCOL_TCP NET_FW_IP_PROTOCOL_UDP NET_FW_IP_PROTOCOL_ANY
			*/
			inline void port_allow(const std::string& name,LONG port,NET_FW_IP_PROTOCOL protocol = NET_FW_IP_PROTOCOL_ANY)
			{
				port_allow(utf::to_utf16(name),port,protocol);
			}
			/**
			*	\brief 添加規則 允許一個 端口 通過 防火牆
			*
			*	\exception boost::system::system_error
			*
			*	\param name 規則名
			*	\param port 端口
			*	\param protocol 協議 NET_FW_IP_PROTOCOL_TCP NET_FW_IP_PROTOCOL_UDP
			*/
			void port_allow(const std::wstring& name,LONG port,NET_FW_IP_PROTOCOL protocol = NET_FW_IP_PROTOCOL_ANY)
			{
				if(port_status(protocol))
				{
					return;
				}

				INetFwOpenPort* fwOpenPort = NULL;
				INetFwOpenPorts* fwOpenPorts = NULL;

				// Retrieve the collection of globally open ports.
				HRESULT hr = _profile->get_GloballyOpenPorts(&fwOpenPorts);
				if (FAILED(hr))
				{
					BOOST_THROW_EXCEPTION(boost::system::system_error(KG_WINDOWS_FIREWALL_CATEGORY_get_GloballyOpenPorts,firewall_category::get()));
				}
				
				// Create an instance of an open port.
				hr = CoCreateInstance(
					__uuidof(NetFwOpenPort),
					NULL,
					CLSCTX_INPROC_SERVER,
					__uuidof(INetFwOpenPort),
					(void**)&fwOpenPort
				);
				if (FAILED(hr))
				{
					KG_SAFE_RELEASE(fwOpenPorts);
					BOOST_THROW_EXCEPTION(boost::system::system_error(KG_WINDOWS_FIREWALL_CATEGORY_CoCreateInstance,firewall_category::get()));
				}

				// Set the port number.
				hr = fwOpenPort->put_Port(port);
				if (FAILED(hr))
				{
					KG_SAFE_RELEASE(fwOpenPort);
					KG_SAFE_RELEASE(fwOpenPorts);
					BOOST_THROW_EXCEPTION(boost::system::system_error(KG_WINDOWS_FIREWALL_CATEGORY_put_Port,firewall_category::get()));
				}

				// Set the IP protocol.
				hr = fwOpenPort->put_Protocol(protocol);
				if (FAILED(hr))
				{
					KG_SAFE_RELEASE(fwOpenPort);
					KG_SAFE_RELEASE(fwOpenPorts);
					BOOST_THROW_EXCEPTION(boost::system::system_error(KG_WINDOWS_FIREWALL_CATEGORY_put_Protocol,firewall_category::get()));
				}

				// Allocate a BSTR for the friendly name of the port.
				BSTR fwBstrName = SysAllocString(name.c_str());
				if (SysStringLen(fwBstrName) == 0)
				{
					KG_SAFE_RELEASE(fwOpenPort);
					KG_SAFE_RELEASE(fwOpenPorts);
					BOOST_THROW_EXCEPTION(boost::system::system_error(KG_WINDOWS_FIREWALL_CATEGORY_SysAllocString,firewall_category::get()));
				}

				// Set the friendly name of the port.
				hr = fwOpenPort->put_Name(fwBstrName);
				if (FAILED(hr))
				{
					SysFreeString(fwBstrName);
					KG_SAFE_RELEASE(fwOpenPort);
					KG_SAFE_RELEASE(fwOpenPorts);
					BOOST_THROW_EXCEPTION(boost::system::system_error(KG_WINDOWS_FIREWALL_CATEGORY_put_Name,firewall_category::get()));
				}
				
				// Opens the port and adds it to the collection.
				hr = fwOpenPorts->Add(fwOpenPort);
				
				SysFreeString(fwBstrName);
				KG_SAFE_RELEASE(fwOpenPort);
				KG_SAFE_RELEASE(fwOpenPorts);
				
				if (FAILED(hr))
				{
					BOOST_THROW_EXCEPTION(boost::system::system_error(KG_WINDOWS_FIREWALL_CATEGORY_AddRule,firewall_category::get()));
				}
			}
		};
	};
};
#endif // KG_WINDOWS_CONSOLE_HEADER_HPP
