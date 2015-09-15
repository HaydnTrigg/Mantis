#include "web_client.hpp"
#include <boost/asio/io_service.hpp>
#include <utils/logger.hpp>
#include <boost/bind.hpp>
#include <boost/algorithm/string.hpp>

using namespace mantis::web;

web_client::web_client() : 
	m_resolver(m_service), 
	m_socket(m_service),
	m_callback(nullptr)
{
}

web_client::web_client(std::function<void(int, std::string)> p_dataCallback) : 
	m_resolver(m_service), 
	m_socket(m_service), 
	m_callback(p_dataCallback)
{
}

web_client::~web_client()
{
}

void web_client::connect(std::string p_server, std::string p_path)
{
	// Form the request. We specify the "Connection: close" header so that the
	// server will close the socket after transmitting the response. This will
	// allow us to treat all data up until the EOF as the content.
	std::ostream request_stream(&m_request);
	request_stream << "GET " << p_path << " HTTP/1.0\r\n";
	request_stream << "Host: " << p_server << "\r\n";
	request_stream << "Accept: */*\r\n";
	request_stream << "Connection: close\r\n\r\n";

	try
	{
		// Start an asynchronous resolve to translate the server and service names
		// into a list of endpoints.
		tcp::resolver::query query(p_server, "3000");
		m_resolver.async_resolve(query,
			boost::bind(&web_client::handle_resolve, this,
				boost::asio::placeholders::error,
				boost::asio::placeholders::iterator));
	}
	catch (...)
	{
		WriteLog("Exception thrown on resolve");
	}

	std::thread s_connectThread([this]() 
	{ 
		try
		{
			m_service.run();
		}
		catch (...)
		{
			send_callback(-1, "could not connect");
		}
	});
	s_connectThread.detach();
}

void web_client::send_callback(int p_error, std::string p_data)
{
	if (m_callback)
		m_callback(p_error, p_data);
}

void web_client::handle_resolve(const boost::system::error_code& p_err, tcp::resolver::iterator endpoint_iterator)
{
	if (!p_err)
	{
		// Attempt a connection to the first endpoint in the list. Each endpoint
		// will be tried until we successfully establish a connection.
		tcp::endpoint endpoint = *endpoint_iterator;
		m_socket.async_connect(endpoint,
			boost::bind(&web_client::handle_connect, this,
				boost::asio::placeholders::error, ++endpoint_iterator));
	}
	else
	{
		WriteLog("Error: %s", p_err.message().c_str());
		send_callback(p_err.value());
	}
}

void web_client::handle_connect(const boost::system::error_code& p_err, tcp::resolver::iterator endpoint_iterator)
{
	if (!p_err)
	{
		// The connection was successful. Send the request.
		async_write(m_socket, m_request,
			boost::bind(&web_client::handle_write_request, this,
				boost::asio::placeholders::error));
	}
	else if (endpoint_iterator != tcp::resolver::iterator())
	{
		// The connection failed. Try the next endpoint in the list.
		m_socket.close();
		tcp::endpoint endpoint = *endpoint_iterator;
		m_socket.async_connect(endpoint,
			boost::bind(&web_client::handle_connect, this,
				boost::asio::placeholders::error, ++endpoint_iterator));
	}
	else
	{
		WriteLog("Error: %s", p_err.message().c_str());
		send_callback(p_err.value());
	}
}

void web_client::handle_write_request(const boost::system::error_code& p_err)
{
	if (!p_err)
	{
		// Read the response status line. The response_ streambuf will
		// automatically grow to accommodate the entire line. The growth may be
		// limited by passing a maximum size to the streambuf constructor.
		async_read_until(m_socket, m_response, "\r\n",
			boost::bind(&web_client::handle_read_status_line, this,
				boost::asio::placeholders::error));
	}
	else
	{
		WriteLog("Error: %s", p_err.message().c_str());
		send_callback(p_err.value());
	}
}

void web_client::handle_read_status_line(const boost::system::error_code& p_err)
{
	if (!p_err)
	{
		// Check that response is OK.
		std::istream response_stream(&m_response);
		std::string http_version;
		response_stream >> http_version;
		unsigned int status_code;
		response_stream >> status_code;
		std::string status_message;
		std::getline(response_stream, status_message);
		if (!response_stream || http_version.substr(0, 5) != "HTTP/")
		{
			WriteLog("Invalid response");
			send_callback(-1, "invalid response");
			return;
		}
		if (status_code != 200)
		{
			WriteLog("Response returned with status code: %d", status_code);
			send_callback(status_code, "invalid status code");
			return;
		}

		// Read the response headers, which are terminated by a blank line.
		boost::asio::async_read_until(m_socket, m_response, "\r\n\r\n",
			boost::bind(&web_client::handle_read_headers, this,
				boost::asio::placeholders::error));
	}
	else
	{
		WriteLog("Error: %s", p_err.message().c_str());
		send_callback(p_err.value());
	}
}

void web_client::handle_read_headers(const boost::system::error_code& p_err)
{
	if (!p_err)
	{
		WriteLog("read_headers");
		// Process the response headers.
		std::istream response_stream(&m_response);
		std::string header;
		while (std::getline(response_stream, header) && header != "\r")
			m_responseStream << header;

		// Write whatever content we already have to output.
		if (m_response.size() > 0)
			m_responseStream << &m_response;

		// Start reading remaining data until EOF.
		async_read(m_socket, m_response,
			boost::asio::transfer_at_least(1),
			boost::bind(&web_client::handle_read_content, this,
				boost::asio::placeholders::error));
	}
	else
	{
		WriteLog("Error: %s", p_err.message().c_str());
		send_callback(p_err.value());
	}
}

void web_client::handle_read_content(const boost::system::error_code& p_err)
{
	if (!p_err)
	{
		// Write all of the data that has been read so far.
		m_responseStream << &m_response;

		// Continue reading remaining data until EOF.
		boost::asio::async_read(m_socket, m_response,
			boost::asio::transfer_at_least(1),
			boost::bind(&web_client::handle_read_content, this,
				boost::asio::placeholders::error));
	}
	else if (p_err != boost::asio::error::eof)
	{
		WriteLog("Error: %s", p_err.message().c_str());

		send_callback(p_err.value());
	}

	auto s_response = m_responseStream.str();
	//s_response = boost::replace_all_copy(s_response, "\r\n", "\n");

	auto s_position = s_response.find_last_of("\r\n\r\n");
	auto s_contentOnly = s_response.substr(s_position);

	send_callback(p_err.value(), s_contentOnly);
}