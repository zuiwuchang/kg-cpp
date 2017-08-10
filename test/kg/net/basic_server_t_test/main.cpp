#include <iostream>
#include <kg/net/basic_server.hpp>
#define ADDRESS "127.0.0.1:1102"
void input(kg::net::basic_server_t& s);

int main(int argc, char* argv[])
{
	int rs = 0;
	try
    {
    	//創建 服務
        kg::net::basic_server_t s(ADDRESS,10,3600);


        //設置 回調
        s.connected([](kg::net::socket_spt s,boost::asio::yield_context ctx){
				//std::cout<<"one in"<<std::endl;
				return true;
		});
		s.closed([](kg::net::socket_spt s,boost::asio::yield_context ctx){
				//std::cout<<"one out"<<std::endl;
		});
		s.readed([](kg::net::socket_spt s,kg::byte_t* b,std::size_t n,boost::asio::yield_context ctx){
				try
				{
					s->async_write_some(boost::asio::buffer(b,n),ctx);
				}
				catch(const boost::system::system_error&)
				{
					return false;
				}
				return true;
		});

		//運行 服務
		boost::thread thead([&s=s](){
					s.run();
		});
		input(s);
		thead.join();
    }
    catch(const boost::system::system_error& e)
    {
		std::cout<< boost::diagnostic_information(e)<<std::endl;
		rs = 1;
    }
    return rs;
}
void input(kg::net::basic_server_t& s)
{
	std::string cmd;
	while(true)
	{
		std::cout<<"\n$>";
		std::cin>>cmd;

		if(cmd == "e")
		{
			s.stop();
			break;
		}
	}
}
