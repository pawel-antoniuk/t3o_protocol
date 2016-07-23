#pragma once
#include <boost/asio.hpp>
#include <functional>
#include "operation_base.hpp"
#include "../event/event.hpp"

namespace t3o
{
namespace detail
{

struct operation_environment
{
	boost::asio::ip::tcp::socket& socket;
	boost::asio::mutable_buffers_1 work_buffer;	
	std::function<void(operation_base&)> inner_operation_complete_handler;
	event<void()> disconnected_event;
};

}
}
