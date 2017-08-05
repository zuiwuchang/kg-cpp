#include <gtest/gtest.h>
#include <kg/net/tcp_answer.hpp>

TEST(ExampleTest, HandleNoneZeroInput)
{
	//使用 == 比較相等
	EXPECT_EQ(1,1);
	EXPECT_EQ(std::string("12"),"12");
}


int main(int argc, char* argv[])
{
	int rs = 0;
	try
	{
		//監聽
		kg::net::tcp_answer_t<std::string> s(":1102"/*,boost::posix_time::seconds(3)*/);

		//設置 回調
		s.create_session([](kg::net::socket_spt sock){
			auto endpoint = sock->remote_endpoint();
			std::string str = endpoint.address().to_string() + ":" + boost::lexical_cast<std::string>(endpoint.port());
			std::cout<<"one in : "<<str<<std::endl;
			return str;
		});
		s.destroy_session([](kg::net::socket_spt sock,std::string str){
			std::cout<<"one out : "<<str<<std::endl;
		});

		//運行
		s.run();

		//testing::InitGoogleTest(&argc, argv);
    	//rs =  RUN_ALL_TESTS();

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
