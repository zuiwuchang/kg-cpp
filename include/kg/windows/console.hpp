#ifndef KG_WINDOWS_CONSOLE_HEADER_HPP
#define KG_WINDOWS_CONSOLE_HEADER_HPP

#include <Windows.h>
namespace kg
{
	namespace windows
	{
		/**
		*	\brief 控制檯
		*
		*/
		class console
		{
		private:
			console();
			~console();
		public:
			/**
			*	\brief 創建 控制台
			*
			*/
			static bool alloc()
			{
				if(!AllocConsole())
				{
					return false;
				}


				static FILE *f_out = NULL;
				static FILE *f_err = NULL;
				static FILE *f_in = NULL;

				if(f_in)
				{
					fclose(f_in);
					f_in = NULL;
				}
				if(f_out)
				{
					fclose(f_out);
					f_out = NULL;
				}
				if(f_err)
				{
					fclose(f_err);
					f_err = NULL;
				}



				f_in = freopen("conin$", "r", stdin);
				if(!f_in)
				{
					return false;
				}

				f_out = freopen("conout$", "w", stdout );
				if(!f_out)
				{
					return false;
				}

				f_err = freopen( "conout$", "w", stderr );
				if(!f_err)
				{
					return false;
				}

				return true;
			}
			/**
			*	\brief 釋放 控制檯
			*
			*/
			static bool free()
			{
				return FreeConsole() == TRUE;
			}
		};
	};
};
#endif // KG_WINDOWS_CONSOLE_HEADER_HPP
