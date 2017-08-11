#include <iostream>

#include <kg/net/echo_client.hpp>
#define ADDRESS "127.0.0.1:1102"
#define FLAG 1102

typedef kg::net::echo_client_t echo_client_t;
void write_str(echo_client_t& c,const std::string& str);
int test_sync();
int main()
{
	int rs = test_sync();
	if(rs)
	{
		return rs;
	}
	return rs;
}
int test_sync()
{
	int rs = 0;
	try
	{
		//創建客戶端
		echo_client_t c(4);

		//設置 回調
        c.reader([](kg::byte_t* b,std::size_t n)->int{
				if(n!=4)
				{
					return -1;
				}
				kg::uint16_t* p = (kg::uint16_t*)b;

				if(FLAG != *p)
				{
					return -1;
				}

				return int(*(p+1));
		});

        //連接服務器
        c.connect("127.0.0.1:1102");

        //發送 數據
        std::string str = "i'm king";
        write_str(c,str);

        //讀取數據
        echo_client_t::bytes_t bytes = c.read();
        bytes = bytes.range(4);
        if(str != std::string((char*)bytes.get(),bytes.size()))
		{
			std::cout<<"rs error"<<std::endl;
		}

		//re send
		str = "cerberus is an idea";
        write_str(c,str);
		bytes = c.read();
        bytes = bytes.range(4);
        if(str != std::string((char*)bytes.get(),bytes.size()))
		{
			std::cout<<"rs 2 error"<<std::endl;
		}
	}
	catch(const boost::system::system_error& e)
    {
		std::cout<< boost::diagnostic_information(e)<<std::endl;
		rs = 1;
    }
    return rs;
}

void write_str(echo_client_t& c,const std::string& str)
{
	std::size_t n = str.size() + 4;
	boost::shared_array<kg::byte_t> buffer(new kg::byte_t[n]);
	kg::uint16_t* ptr = (kg::uint16_t*)buffer.get();
	*ptr = FLAG;
	*(ptr + 1) = n;
	memcpy(buffer.get()+4,str.data(),str.size());

	kg::net::socket_t& sock = c.get();
	sock.write_some(boost::asio::buffer(buffer.get(),n));

}
