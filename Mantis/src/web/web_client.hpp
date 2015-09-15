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
			boost::asio::io_service m_service;
			tcp::resolver m_resolver;
			tcp::socket m_socket;
			boost::asio::streambuf m_request;
			boost::asio::streambuf m_response;
			std::stringstream m_responseStream;

			std::function<void(int, std::string)> m_callback;

		public:
			web_client();
			web_client(std::function<void(int, std::string)> p_dataCallback);

			~web_client();

			void connect(std::string p_server, std::string p_path);

		private:
			void send_callback(int p_error, std::string p_data = "");
			void handle_resolve(const boost::system::error_code& p_err,
				tcp::resolver::iterator endpoint_iterator);
			void handle_connect(const boost::system::error_code& p_err,
				tcp::resolver::iterator endpoint_iterator);
			void handle_write_request(const boost::system::error_code& p_err);
			void handle_read_status_line(const boost::system::error_code& p_err);
			void handle_read_headers(const boost::system::error_code& p_err);
			void handle_read_content(const boost::system::error_code& p_err);
		};
	}
}