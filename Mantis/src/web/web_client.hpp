#pragma once
#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>

using boost::asio::ip::tcp;

namespace mantis
{
	namespace web
	{
		class web_client
		{
		private:
			static web_client* m_Instance;
			boost::asio::io_service io_service;
			tcp::resolver resolver_;
			tcp::socket socket_;
			boost::asio::streambuf request_;
			boost::asio::streambuf response_;
			std::stringstream m_Response;

		public:
			static web_client* GetInstance();
			web_client();
			~web_client();

			void connect();

		private:
			void handle_resolve(const boost::system::error_code& err,
				tcp::resolver::iterator endpoint_iterator);
			void handle_connect(const boost::system::error_code& err,
				tcp::resolver::iterator endpoint_iterator);
			void handle_write_request(const boost::system::error_code& err);
			void handle_read_status_line(const boost::system::error_code& err);
			void handle_read_headers(const boost::system::error_code& err);
			void handle_read_content(const boost::system::error_code& err);
		};
	}
}