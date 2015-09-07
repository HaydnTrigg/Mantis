#include "web_client.hpp"
#include <boost/asio/io_service.hpp>
#include <utils/logger.hpp>
#include <boost/bind.hpp>
#include <boost/algorithm/string.hpp>

using namespace mantis::web;

web_client* web_client::m_Instance = nullptr;
web_client* web_client::GetInstance()
{
	if (!m_Instance)
		m_Instance = new web_client;

	return m_Instance;
}

web_client::web_client() : resolver_(io_service), socket_(io_service)
{
}

web_client::~web_client()
{
}

void web_client::connect()
{
	std::string path("/api/update/manifest");
	std::string server("localhost");

	// Form the request. We specify the "Connection: close" header so that the
	// server will close the socket after transmitting the response. This will
	// allow us to treat all data up until the EOF as the content.
	std::ostream request_stream(&request_);
	request_stream << "GET " << path << " HTTP/1.0\r\n";
	request_stream << "Host: " << server << "\r\n";
	request_stream << "Accept: */*\r\n";
	request_stream << "Connection: close\r\n\r\n";

	// Start an asynchronous resolve to translate the server and service names
	// into a list of endpoints.
	tcp::resolver::query query(server, "3000");
	resolver_.async_resolve(query,
		boost::bind(&web_client::handle_resolve, this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::iterator));

	std::thread s_connectThread([this]() { io_service.run(); });
	s_connectThread.detach();

	WriteLog("web_client connect.");
}


void web_client::handle_resolve(const boost::system::error_code& err, tcp::resolver::iterator endpoint_iterator)
{
	if (!err)
	{
		WriteLog("resolve");

		// Attempt a connection to the first endpoint in the list. Each endpoint
		// will be tried until we successfully establish a connection.
		tcp::endpoint endpoint = *endpoint_iterator;
		socket_.async_connect(endpoint,
			boost::bind(&web_client::handle_connect, this,
				boost::asio::placeholders::error, ++endpoint_iterator));
	}
	else
	{
		WriteLog("Error: %s", err.message().c_str());
	}
}

void web_client::handle_connect(const boost::system::error_code& err, tcp::resolver::iterator endpoint_iterator)
{
	if (!err)
	{
		WriteLog("connect");

		// The connection was successful. Send the request.
		boost::asio::async_write(socket_, request_,
			boost::bind(&web_client::handle_write_request, this,
				boost::asio::placeholders::error));
	}
	else if (endpoint_iterator != tcp::resolver::iterator())
	{
		// The connection failed. Try the next endpoint in the list.
		WriteLog("connect next");
		socket_.close();
		tcp::endpoint endpoint = *endpoint_iterator;
		socket_.async_connect(endpoint,
			boost::bind(&web_client::handle_connect, this,
				boost::asio::placeholders::error, ++endpoint_iterator));
	}
	else
	{
		WriteLog("Error: %s", err.message().c_str());
	}
}

void web_client::handle_write_request(const boost::system::error_code& err)
{
	if (!err)
	{
		WriteLog("write_request");
		// Read the response status line. The response_ streambuf will
		// automatically grow to accommodate the entire line. The growth may be
		// limited by passing a maximum size to the streambuf constructor.
		boost::asio::async_read_until(socket_, response_, "\r\n",
			boost::bind(&web_client::handle_read_status_line, this,
				boost::asio::placeholders::error));
	}
	else
	{
		WriteLog("Error: %s", err.message().c_str());
	}
}

void web_client::handle_read_status_line(const boost::system::error_code& err)
{
	if (!err)
	{
		WriteLog("read_status_line");

		// Check that response is OK.
		std::istream response_stream(&response_);
		std::string http_version;
		response_stream >> http_version;
		unsigned int status_code;
		response_stream >> status_code;
		std::string status_message;
		std::getline(response_stream, status_message);
		if (!response_stream || http_version.substr(0, 5) != "HTTP/")
		{
			WriteLog("Invalid response");
			return;
		}
		if (status_code != 200)
		{
			WriteLog("Response returned with status code: %d", status_code);
			return;
		}

		// Read the response headers, which are terminated by a blank line.
		boost::asio::async_read_until(socket_, response_, "\r\n\r\n",
			boost::bind(&web_client::handle_read_headers, this,
				boost::asio::placeholders::error));
	}
	else
	{
		WriteLog("Error: %s", err.message().c_str());
	}
}

void web_client::handle_read_headers(const boost::system::error_code& err)
{
	if (!err)
	{
		WriteLog("read_headers");
		// Process the response headers.
		std::istream response_stream(&response_);
		std::string header;
		while (std::getline(response_stream, header) && header != "\r")
			m_Response << header;

		// Write whatever content we already have to output.
		if (response_.size() > 0)
			m_Response << &response_;

		// Start reading remaining data until EOF.
		boost::asio::async_read(socket_, response_,
			boost::asio::transfer_at_least(1),
			boost::bind(&web_client::handle_read_content, this,
				boost::asio::placeholders::error));
	}
	else
	{
		WriteLog("Error: %s", err.message().c_str());
	}
}

void web_client::handle_read_content(const boost::system::error_code& err)
{
	if (!err)
	{
		WriteLog("read_content");

		// Write all of the data that has been read so far.
		m_Response << &response_;

		// Continue reading remaining data until EOF.
		boost::asio::async_read(socket_, response_,
			boost::asio::transfer_at_least(1),
			boost::bind(&web_client::handle_read_content, this,
				boost::asio::placeholders::error));
	}
	else if (err != boost::asio::error::eof)
	{
		WriteLog("Error: %s", err.message().c_str());
	}

	auto s_Response = m_Response.str();
	boost::replace_all(s_Response, "\r\n", "\n");

	WriteLog("Response Buffer: %s", s_Response.c_str());
}