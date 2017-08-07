#include <kg/net/tcp_answer.hpp>

typedef std::string* session_t;
typedef kg::net::tcp_answer_t<session_t> service_t;

typedef service_t::configure_t configure_t;
typedef service_t::configure_spt configure_spt;

typedef configure_t::slice_t slice_t;

#define HEADER_SIZE		4
#define CMD_SIZE		2
#define MIN_MSG_SIZE	((HEADER_SIZE + CMD_SIZE))
#define MAX_MSG_SIZE	4096

#define CMD_ECHO	1
#define CMD_EXIT	2
#define CMD_UNKNOW	666
void async_write_handler(const boost::system::error_code& e,kg::net::socket_spt sock,slice_t slice)
{
	if(e)
	{
		//出錯 關閉 連接
		boost::system::error_code ec;
		sock->shutdown(boost::asio::ip::tcp::socket::shutdown_both,ec);
        sock->close(ec);
		return;
	}

}
void async_write(kg::net::socket_spt sock,slice_t slice,kg::uint16_t cmd = 0)
{
	if(cmd)
	{
		kg::uint16_t* ptr = (kg::uint16_t*)slice.get();
		*ptr = 1102;
		*(ptr + 1) = (kg::uint16_t)slice.size();
		*(ptr + 2) = cmd;
	}

    sock->async_write_some(boost::asio::buffer(slice.get(),slice.size()),
                           boost::bind(async_write_handler,
                                       boost::asio::placeholders::error,
                                       sock,
                                       slice
                                      )
                          );
}
int main(int argc, char* argv[])
{
    int rs = 0;
    try
    {

        //配置服務器
        configure_spt cnf = boost::make_shared<service_t::configure_t>(HEADER_SIZE);

        /***	設置 回調	***/
        //創建 session
        cnf->create_session([](kg::net::socket_spt sock)
        {
            auto endpoint = sock->remote_endpoint();
            std::string* str = new std::string(endpoint.address().to_string() + ":" + boost::lexical_cast<std::string>(endpoint.port()));
            //std::cout<<"one in : "<<*str<<std::endl;
            return str;
        });
        //銷毀 session
        cnf->destroy_session([](kg::net::socket_spt sock,session_t str)
        {
            //std::cout<<"one out : "<<*str<<std::endl;
            delete str;
        });

        //解析消息
        cnf->reader_header([](kg::byte_t* b,std::size_t n)->kg::uint32_t
        {
            kg::uint16_t* ptr = (kg::uint16_t*)b;
            if(*ptr != 1102)
            {
                return 0;
            }

            kg::uint32_t len = (kg::uint32_t)(*(ptr + 1));
            if(len < MIN_MSG_SIZE || len > MAX_MSG_SIZE)
            {
                return 0;
            }
            return len;
        });

        //響應 消息
        cnf->message([](kg::net::socket_spt sock,session_t str,slice_t slice)
        {
            slice_t body = slice.range(HEADER_SIZE);
            kg::uint16_t* ptr = (kg::uint16_t*)(body.get());
            switch(*ptr)
            {
            case CMD_ECHO:
            	//return echo
				async_write(sock,slice);
                break;
            case CMD_EXIT:
            	//close client
                return false;
            default:
            	//return unkow
            	slice_t data(HEADER_SIZE+CMD_SIZE);
            	async_write(sock,slice,kg::uint16_t(CMD_UNKNOW));
                break;
            }
            return true;
        });

        //監聽
        std::string addr = ":1102";
        service_t s(addr,cnf);
        //std::cout<<"work at "<<addr<<std::endl;

        //運行
        s.run();


        //s.stop();

        //s.join();
        std::string cmd;
        while(true)
        {
            std::cin>>cmd;
            if(cmd == "exit")
            {
                s.stop();
                break;
            }
        }
    }
    catch(const std::bad_alloc& e)
    {
        std::cout<<e.what()<<std::endl;
        return 1;
    }
    catch(const std::exception& e)
    {
        std::cout<<e.what()<<std::endl;
        return 1;
    }


    return rs;
}
