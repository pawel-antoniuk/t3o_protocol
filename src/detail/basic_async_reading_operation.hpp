#pragma once
#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>
#include <tuple>
#include <array>
#include <iostream>
#include <boost/fusion/algorithm/iteration/for_each.hpp>
#include "operation_base.hpp"
#include "operation_environment.hpp"

namespace t3o
{
namespace detail
{

namespace detail
{
	using namespace boost::asio;
}

template<typename InputSerializer, typename... Serializables>
class basic_async_reading_operation : 
	public operation_base
{
public:
	basic_async_reading_operation(
			operation_environment& environment,
			std::function<void(const Serializables&)>... completion_handlers
			) :
		_environment(environment),
		_completion_handlers(completion_handlers...)
	{
	}

	void async_execute() override
	{
		using namespace std::placeholders;
		auto buffer = detail::buffer(_environment.work_buffer, 2);
		auto binder	= std::bind(
				&basic_async_reading_operation<InputSerializer, Serializables...>
				::_on_header_read, this, _1, _2);
		detail::async_read(_environment.socket, buffer, binder);
	}

private:
	bool _check_connection(const boost::system::error_code& er)
	{
		if(er != boost::system::errc::success)
		{
			_environment.disconnected_event();
			return true;
		}
		return false;
	}

	template<typename Serializable>
	bool _process_header()
	{
		using namespace std::placeholders;
		auto buffer_ptr = boost::asio::buffer_cast<uint8_t*>(_environment.work_buffer);
		auto packet_id = buffer_ptr[0];
		auto packet_size = buffer_ptr[1];

		if(packet_id != Serializable::packet_id) return false;

		auto buffer = boost::asio::buffer(buffer_ptr + 2, packet_size);
		auto binder = std::bind(
				&basic_async_reading_operation<InputSerializer, Serializables...>
				::_on_body_read<Serializable>, this, _1, _2);
		detail::async_read(_environment.socket, buffer, binder);

		return true;
	}

	void _on_header_read(const boost::system::error_code& er, std::size_t size)
	{
		if(_check_connection(er)) return;

		//auto buf_ptr = boost::asio::buffer_cast<uint8_t*>(_environment.work_buffer);
		//auto buf_size = buf_ptr[1];
		//std::cout << "[DEBUG]: packet size before process: " << (int)buf_ptr[1] << std::endl;
		//std::cout << "[DEBUG]: packet id before process: " << (int)buf_ptr[0] << std::endl;
		//std::cout << "[DEBUG]: header size recived: " << size << std::endl;

		auto results = { _process_header<Serializables>()... };
		auto it = std::find(std::begin(results), std::end(results), true);
		if(it == std::end(results))
		{
			std::cout << "[DEBUG]: packet not found" << std::endl;
			_environment.disconnected_event();
		}
			//throw std::runtime_error("packet not found"); //TODO
	}

	template<typename Serializable>
	void _on_body_read(const boost::system::error_code& er, std::size_t size)
	{
		if(_check_connection(er)) return;

		auto data = boost::asio::buffer_cast<uint8_t*>(_environment.work_buffer);
		auto serializable_obj = InputSerializer
			::template process_input_data<Serializable>(&data[2], data[1]);
		using handler_t = std::function<void(const Serializable&)>;
		auto handler = std::get<handler_t>(_completion_handlers);

		//std::cout << "[DEBUG]: avaiable: " << (int)_environment.socket.available() << std::endl;
		//std::cout << "[DEBUG]: body size recived: " << size << std::endl;
		
		handler(serializable_obj);
		_environment.socket.get_io_service().dispatch([this]
		{
			_environment.inner_operation_complete_handler(*this);
		});
	}

	operation_environment& _environment;
	std::tuple<std::function<void(const Serializables&)>...> _completion_handlers;
};

}
}
