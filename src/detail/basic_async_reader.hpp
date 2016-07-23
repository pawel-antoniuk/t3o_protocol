#pragma once
#include <boost/asio.hpp>
#include <queue>
#include <functional>
#include <memory>
#include <array>
#include <cstdint>
#include "operation_queue_base.hpp"
#include "basic_async_reading_operation.hpp"
#include "operation_environment.hpp"
#include "../event/event.hpp"

namespace t3o
{
namespace detail
{

namespace detail
{
	using namespace boost::asio;
}

template<typename InputSerializer>
class basic_async_reader : public operation_queue_base
{
public:
	basic_async_reader(detail::ip::tcp::socket& socket) :
		_environment{socket,
			boost::asio::buffer(_work_buffer.data(), _work_buffer.size())}
	{
		using namespace std::placeholders;
		auto complete_binder 
			= std::bind(&basic_async_reader<InputSerializer>
					::inner_operation_complete, this, _1);
		auto disconnect_binder 
			= std::bind(&basic_async_reader<InputSerializer>
					::_handle_disconnect, this);
		_environment.inner_operation_complete_handler = complete_binder;
		_environment.disconnected_event += disconnect_binder;
	}

	template<typename... Serializables>
	void async_read(std::function<void(const Serializables&)>... completion_handlers)
	{
		push_operation(
				std::make_unique<basic_async_reading_operation<InputSerializer, Serializables...>>(
					_environment, completion_handlers...));
	}

	auto& event_disconnected()
	{
		return _environment.disconnected_event;
	}

private:
	void _handle_disconnect()
	{
		clean_queue();
	}

	std::array<uint8_t, 512> _work_buffer;
	operation_environment _environment;
};

}
}
