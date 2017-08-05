#ifndef KG_NET_TCP_ANSWER_HEADER_HPP
#define KG_NET_TCP_ANSWER_HEADER_HPP

#include <vector>

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/typeof/typeof.hpp>
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/unordered_map.hpp>

#include "../types.hpp"
//#include "../bytes/buffer.hpp"
namespace kg
{
namespace net
{
	/*	typedef boost	*/
	typedef boost::asio::io_service io_service_t;
	typedef boost::asio::io_service::work work_t;
	typedef boost::asio::ip::tcp::acceptor acceptor_t;
	typedef boost::asio::ip::tcp::socket socket_t;
	typedef boost::shared_ptr<io_service_t> io_service_spt;
	typedef boost::shared_ptr<work_t> work_spt;
	typedef boost::shared_ptr<acceptor_t> acceptor_spt;
	typedef boost::shared_ptr<socket_t> socket_spt;

	typedef boost::thread thread_t;
	typedef boost::shared_ptr<thread_t> thread_spt;

	typedef boost::asio::deadline_timer deadline_timer_t;

	/**
	*	\brief 無效的 ip 地址
	*/
	class bad_address_t:public std::exception
	{
	public:
		virtual const char* what() const noexcept
		{
			return "address is invalid";
		}
	};

	/**
	*	\brief 一個tcp 應答服務器
	*
	*	孤 發現 很多時候 需要 實現一個 tcp 應答服務器 類似 rpc/http 服務器\n
	*	(rpc 客戶端 服務器 實現起來都 比較 繁瑣 且 不同客戶端 語言 也遠是個麻煩 rpc 也沒有 session)\n
	*	(應答服務器 接收一個 客戶請求 之後 返回響應 基本和 http 工作模式 一樣 只是 現在 保存了 狀態 並且 是長連接的)\n
	*
	*	\param SessionT	一個支持copy語義的 自定義 對象 可以用來存儲自定義的 session 數據
	*	\param BUFFER_SIZE	recv 緩衝區大小
	*	\param COMPARE_GET	將tcp分配到不同 cpu時 忽略的大小 COMPARE_GET最小取值爲 1
	*
	*/
	template<typename SessionT,std::size_t BUFFER_SIZE = 1024,std::size_t COMPARE_GET = 50>
	class tcp_answer_t
		:boost::noncopyable
	{
	public:
		/**
		*	\brief class 型別
		*
		*/
		typedef tcp_answer_t type_t;

		/**
		*	\brief session 型別
		*
		*/
		typedef SessionT session_t;

		/**
		*	\brief 如何創建 session 默認 session_t()
		*
		*/
		typedef boost::function<session_t(socket_spt)> create_session_bft;

		/**
		*	\brief 如何銷毀 session
		*
		*/
		typedef boost::function<void(socket_spt,session_t)> destroy_session_bft;
	private:
		create_session_bft _create_session;
		destroy_session_bft _destroy_session;
	public:
		/**
		*	\brief 設置 創建 session 回調 在新連接創建成功時 回調此函數 默認 SessionT()
		*
		*	\warning 一旦 run 之後 邊 不能再 調用此函數
		*/
		inline void create_session(create_session_bft bf)
		{
			_create_session = bf;
		}
		/**
		*	\brief 設置 銷毀 session 回調 在連接調開前 回調此函數
		*
		*	\warning 一旦 run 之後 邊 不能再 調用此函數
		*/
		inline void destroy_session(destroy_session_bft bf)
		{
			_destroy_session = bf;
			BOOST_FOREACH(service_info_spt service,_services)
			{
				service->destroy_session = _destroy_session;
			}
		}

		inline create_session_bft create_session()const
		{
			return _create_session;
		}
		inline destroy_session_bft destroy_session()const
		{
			return _destroy_session;
		}
	private:

		/**
		*	\brief 響應 服務器
		*
		*/
		class service_info_t
		{
			//typedef kg::bytes::buffer_t<> buffer_t;
			/**
			*	\brief 消息 解析器
			*
			*/
			class reader_t
			{
			public:
				boost::shared_array<kg::byte_t> bytes;
				std::size_t len;

				//緩衝區
				//buffer_t buffer;

				//是否 讀取了 header
				bool header;
				//記錄 消息長度
				kg::uint32_t size;
				reader_t(std::size_t n)
					:len(n)
					,header(false)
					//,buffer(n)
				{
					bytes = boost::shared_array<kg::byte_t>(new kg::byte_t[n]);
				}

			};

			/**
			*	\brief 客戶消息
			*
			*/
			class client_t
			{
			public:
				typedef client_t type_t;
				//socket
				socket_spt sock;

				//消息 解析器
				reader_t reader;
				//客戶自定義 session
				session_t session;

				//定時器 用於 客戶端 超時 處理
				deadline_timer_t timer;

				client_t(std::size_t n,io_service_t& service)
					:reader(n),timer(service)
				{
				}
			public:
				//超時 斷開 連接
				void async_timeout(boost::posix_time::time_duration duration)
				{
					boost::system::error_code ec;
					timer.cancel(ec);

					timer.expires_from_now(duration,ec);
					if(ec)
					{
						close();
						return;
					}

					timer.async_wait(boost::bind(&type_t::handler_timeout,this,_1));

				}
				//關閉 超時定時器 以便 準備 釋放 client 資源
				inline void stop()
				{
					boost::system::error_code ec;
					timer.cancel(ec);
				}
			private:
				void handler_timeout(const boost::system::error_code& ec)
				{
					if(ec)
					{
						return;
					}

					close();
				}
				inline void close()
				{
					boost::system::error_code ec;
					sock->shutdown(boost::asio::ip::tcp::socket::shutdown_both,ec);
					sock->close(ec);
				}
			};
			typedef boost::shared_ptr<client_t> client_spt;
		public:
			typedef service_info_t type_t;
			destroy_session_bft destroy_session;
		private:
			std::size_t _num;

			io_service_spt _service;
			work_spt _work;
			typedef boost::unordered_map<socket_spt,client_spt> clients_t;
			clients_t _clients;
			boost::posix_time::time_duration _timeout;
		public:
			service_info_t(io_service_spt service,work_spt work,boost::posix_time::time_duration timeout)
				:_service(service),_work(work)
				,_timeout(timeout)
				,_num(0)
			{

			}
			/**
			*	\brief 停止服務
			*
			*/
			inline void stop()
			{
				_service->dispatch(boost::bind(&type_t::handler_dispatch_stop,this));
			}
		private:
			void handler_dispatch_stop()
			{
				_service->stop();

				_work.reset();


				BOOST_AUTO(iter,_clients.begin());
				for(;iter != _clients.end();++iter)
				{
					boost::system::error_code ec;

					socket_spt sock = iter->first;
					client_spt client = iter->second;

					//timer
					client->stop();

					//shutdown
					sock->shutdown(boost::asio::ip::tcp::socket::shutdown_both,ec);

					//callback
					if(destroy_session)
					{
						destroy_session(sock,client->session);
					}

					//close
					sock->close(ec);
				}


				_clients.clear();
				_service.reset();
			}
		public:
			/**
			*	\brief 返回 asio 服務器
			*
			*/
			inline std::size_t num()const
			{
				return _num;
			}
			/**
			*	\brief 返回 asio 服務器
			*
			*/
			inline io_service_t& get_io_service()const
			{
				return *_service;
			}
			/**
			*	\brief 在連接成功時 投遞 recv
			*
			*/
			inline void dispatch_read(socket_spt sock,session_t session)
			{
				_service->dispatch(boost::bind(&type_t::handler_dispatch_accept,this,sock,session));
			}
		private:
			void handler_dispatch_accept(socket_spt sock,session_t session)
			{
				client_spt client;
				try
				{
					//創建 客戶信息
					client = boost::make_shared<client_t>(BUFFER_SIZE,*_service);
					client->sock = sock;
					client->session = session;
				}
				catch(const std::bad_alloc&)
				{
					//內存不夠 直接 關閉連接
					boost::system::error_code ec;
					//shutdown
					sock->shutdown(boost::asio::ip::tcp::socket::shutdown_both,ec);

					//callback
					if(destroy_session)
					{
						destroy_session(sock,session);
					}

					//close
					sock->close(ec);
					return;
				}

				//保存 客戶端
				_clients[sock] = client;

				//客戶端 計算 加 1
				++_num;

				//recv
				post_read(client);
			}
			void post_read(client_spt client)
			{
				//重置 timeout
				client->async_timeout(_timeout);

				//post
				socket_t& sock = *(client->sock);
				reader_t& reader = client->reader;
				sock.async_read_some(boost::asio::buffer(reader.bytes.get(),reader.len),
						boost::bind(&type_t::handler_post_read,
								this,
								boost::asio::placeholders::error,
								client,
								boost::asio::placeholders::bytes_transferred
							)
						);
			}
			void handler_post_read(const boost::system::error_code& ec,
                       client_spt client,
                       const std::size_t n)
			{
				if(ec)
				{
					close(client->sock);
					return;
				}

				//reader
				reader_t& reader = client->reader;
				//buffer_t& buffer = reader.buffer;
				while(true)
				{

				}

				//post
				post_read(client);
			}
			void close(socket_spt sock)
			{
				//已經被刪除 read_close and write_close
				BOOST_AUTO(find,_clients.find(sock));
				if(find == _clients.end())
				{
					return;
				}

				--_num;

				//timer
				find->second->stop();

				boost::system::error_code ec;
				//shutdown
				sock->shutdown(boost::asio::ip::tcp::socket::shutdown_both,ec);

				//callback
				if(destroy_session)
				{
					destroy_session(sock,find->second->session);
				}

				//close
        		sock->close(ec);

				_clients.erase(find);

			}

		};
		typedef boost::shared_ptr<service_info_t> service_info_spt;
		typedef std::vector<service_info_spt> services_info_t;
	public:
		/**
		*	\brief 啓動服務器 並監聽 指定地址
		*
		*	\exception class std::bad_alloc
		*	\exception class boost::system::system_error
		*	\exception class bad_listen_address_t
		*
		*	\param addr		形如 :port XXX:port 的監聽地址
		*	\param timeout	client 超時 時間
		*
		*/
		tcp_answer_t(std::string addr,boost::posix_time::time_duration timeout = boost::posix_time::hours(1))
			:_timer(_service),
			_flag(0),_pos(0)
		{
			//分解 服務器 地址
			boost::algorithm::trim(addr);
			BOOST_AUTO(find,addr.find(":"));
			if(find == addr.npos)
			{
				throw bad_address_t();
			}
			std::string ip = addr.substr(0,find);
			addr = addr.substr(find+1);
			unsigned short port;
			try
			{
				port = boost::lexical_cast<unsigned short>(addr);
				if(port == 0)
				{
					throw bad_address_t();
				}
			}
			catch(const boost::bad_lexical_cast&)
			{
				throw bad_address_t();
			}

			//監聽 端口
			if(ip.empty())
			{
				//監聽
				boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address_v4::any(),port);
				_acceptor = boost::make_shared<acceptor_t>(_service,endpoint);
			}
			else
			{
				//監聽
				boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address::from_string(ip),port);
				_acceptor = boost::make_shared<acceptor_t>(_service,endpoint);
			}


			//創建 響應 服務 線程
			std::size_t n = boost::thread::hardware_concurrency();
			for(std::size_t i=0; i<n; ++i)
			{
				//創建 服務
				io_service_spt service = boost::make_shared<io_service_t>();
				work_spt work = boost::make_shared<work_t>(*service);
				service_info_spt info = boost::make_shared<service_info_t>(service,work,timeout);

				_services.push_back(info);


				//啓動 工作線程
				_threads.create_thread(boost::bind(works,service));
			}
		}
		~tcp_answer_t()
		{

		}

		/**
		*	\brief 運行服務器
		*
		*	\exception class std::bad_alloc
		*
		*	\warning	一旦 stop 之後 不能再次 run
		*/
		void run()
		{
			if(!_thread)
			{
				_thread = boost::make_shared<thread_t>(&tcp_answer_t::work,this);
			}
		}
		/**
		*	\brief 停止服務器 並等待 服務結束
		*
		*	\warning	一旦 stop 之後 不能再次 run
		*/
		void stop()
		{
			//停止 監聽服務器
			if(_acceptor)
			{
				boost::system::error_code ec;

				//stop
				_service.stop();
				_acceptor->cancel(ec);

				//等待線程 結束
				if(_thread)
				{
					_thread->join();
					_thread.reset();
				}

				//釋放資源
				_acceptor.reset();
			}

			//停止響應服務器
			if(!_services.empty())
			{
				//stop
				BOOST_FOREACH(service_info_spt& info,_services)
				{
					info->stop();
				}

				//等待線程 結束
				_threads.join_all();

				//釋放資源
				_services.clear();
			}
		}

		/**
		*	\brief 等待服務器 停止
		*
		*/
		inline void join()
		{
			_threads.join_all();
		}
	private:
		/**
		*	\brief 接收連接的 asio 服務
		*
		*/
		io_service_t _service;
		/**
		*	\brief 與 _service 關聯的 定時器
		*
		*/
		deadline_timer_t _timer;
		/**
		*	\brief asio 接受器
		*
		*/
		acceptor_spt _acceptor;
		/**
		*	\brief 監聽線程
		*
		*/
		thread_spt _thread;
		void work()
		{
			_service.dispatch(boost::bind(&type_t::dispatch_accept,this));
			_service.run();
		}
		void dispatch_accept()
		{
			post_accept();
		}

		/**
		*	\brief 響應請求的 asio 服務 數組
		*
		*/
		services_info_t _services;


		/**
		*	\brief 響應線程
		*
		*/
		boost::thread_group _threads;
		static void works(io_service_spt s)
		{
			s->run();
		}

	private:
		std::size_t _flag;
		std::size_t _pos;
		/**
		*	\brief 返回一個 響應服務器
		*
		*/
		service_info_spt get_service()
		{
			if(_flag)
			{
				--_flag;
				 return _services[_pos];
			}

			std::size_t min = _services[0]->num();
			_pos = 0;
			for(std::size_t i=1; i<_services.size(); ++i)
			{
				std::size_t num = _services[0]->num();
				if(num < min)
				{
					min = num;
					_pos = i;
				}
			}
			_flag = COMPARE_GET - 1;
			return _services[_pos];
		}
		/**
		*	\brief 投遞一個 接收連接
		*
		*/
		void post_accept(service_info_spt info=service_info_spt())
		{
			if(!info)
			{
				info = get_service();
			}

			try
			{

				socket_spt sock = boost::make_shared<socket_t>(info->get_io_service());
				_acceptor->async_accept(*sock,
					boost::bind(&tcp_answer_t::handler_accept,
						this,
						boost::asio::placeholders::error,
						info,
						sock
					)
				);

			}
			catch(const std::bad_alloc&)
			{
				//內存不足 等待一秒後 重新 post
				std::cerr<<"tcp_answer_t::post_accept bad_alloc";

				_timer.expires_from_now(boost::posix_time::seconds(1));
				_timer.async_wait(boost::bind(&tcp_answer_t::handler_timer_accept,this,_1,info));
			}
		}
		/**
		*	\brief 重新投遞 accept
		*
		*/
		void handler_timer_accept(const boost::system::error_code& e,service_info_spt info)
		{
			if(e)
			{
				return;
			}

			post_accept(info);
		}

		/**
		*	\brief handler accept
		*
		*/
		void handler_accept(const boost::system::error_code& ec,service_info_spt info,socket_spt sock)
		{
			post_accept();

			if(ec)
			{
				return;
			}

			//通知新連接
			session_t session = session_t();
			if(_create_session)
			{
				session = _create_session(sock);
			}

			//post read
			info->dispatch_read(sock,session);
		}
	};

};
};
#endif	//KG_NET_TCP_ANSWER_HEADER_HPP
