#ifndef KG_NET_TCP_ANSWER_SERVICE_INFO_HEADER_HPP
#define KG_NET_TCP_ANSWER_SERVICE_INFO_HEADER_HPP


#include "tcp_answer_configure.hpp"
#include "../slice.hpp"

namespace kg
{
namespace net
{
	/**
    *	\brief 響應 服務器
    *
    */
    template<typename T>
    class answer_service_info_t
    {
	public:
		typedef T session_t;
		typedef answer_service_info_t type_t;

		typedef	tcp_answer_configure_t<T> 	configure_t;
		typedef	boost::shared_ptr<configure_t> configure_spt;
	private:
   		configure_spt _cnf;
	private:
        typedef kg::bytes::buffer_t<> buffer_t;
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
            buffer_t buffer;

            //是否 讀取了 header
            bool header;
            //記錄 消息長度
            kg::uint32_t msg_size;
            reader_t(std::size_t n)
                :len(n)
                ,header(false)
                ,buffer(n)
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

    private:
        std::size_t _num;

        io_service_spt _service;
        work_spt _work;
        typedef boost::unordered_map<socket_spt,client_spt> clients_t;
        clients_t _clients;
    public:
        answer_service_info_t(io_service_spt service,work_spt work,configure_spt cnf)
            :_service(service),_work(work)
            ,_cnf(cnf)
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
            for(; iter != _clients.end(); ++iter)
            {
                boost::system::error_code ec;

                socket_spt sock = iter->first;
                client_spt client = iter->second;

                //timer
                client->stop();

                //shutdown
                sock->shutdown(boost::asio::ip::tcp::socket::shutdown_both,ec);

                //callback
                BOOST_AUTO(destroy_session,_cnf->destroy_session());
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
                client = boost::make_shared<client_t>(_cnf->buffer_size(),*_service);
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
                BOOST_AUTO(destroy_session,_cnf->destroy_session());
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
            client->async_timeout(_cnf->timeout());

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
            	std::cout<<ec<<" "<<"\n";
                close(client->sock);
                return;
            }

            //reader
            reader_t& reader = client->reader;
            buffer_t& buffer = reader.buffer;
            if(n != buffer.write(reader.bytes.get(),n))
            {
                close(client->sock);
                client->stop();
                return;
            }

            try
            {
				while(true)
				{
					if(!reader.header)
					{
						//讀取 header
						kg::uint32_t headerSize = _cnf->header_size();
						if(buffer.size() < headerSize)
						{
							//等待 header
							break;
						}

						boost::shared_array<kg::byte_t> bytes(new kg::byte_t[headerSize]);
						buffer.copy_to(bytes.get(),headerSize);

						BOOST_AUTO(reader_header,_cnf->reader_header());
						if(!reader_header)
						{
							//沒有 解析器 斷開連接
							close(client->sock);
							client->stop();
							std::cerr<<"configure not set reader_header_bft"<<std::endl;
							return;
						}
						kg::uint32_t n = reader_header(bytes.get(),headerSize);
						if(n < headerSize)
						{
							//錯誤消息 頭 斷開連接
							close(client->sock);
							client->stop();
							return;
						}

						//記錄 消息長
						reader.msg_size = n;
						reader.header = true;
					}

					//讀取 body
					if(buffer.size() < reader.msg_size)
					{
						//等待 body
						break;
					}
					kg::slice_t<kg::byte_t> msg(reader.msg_size);
					buffer.read(msg.get(),msg.size());

					reader.header = false;

					//通知 響應
					BOOST_AUTO(message,_cnf->message());
					if(message && !message(client->sock,client->session,msg))
					{
						//錯誤消息 body 斷開連接
						close(client->sock);
						client->stop();
						return;
					}
				}
            }
            catch(const std::bad_alloc&)
            {
            	//內存不夠 斷開連接
            	close(client->sock);
                client->stop();
                return;
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
            BOOST_AUTO(destroy_session,_cnf->destroy_session());
            if(destroy_session)
            {
                destroy_session(sock,find->second->session);
            }

            //close
            sock->close(ec);

            _clients.erase(find);

        }

    };


};
};
#endif	//KG_NET_TCP_ANSWER_SERVICE_INFO_HEADER_HPP
