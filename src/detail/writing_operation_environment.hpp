#pragma once
#include <boost/asio.hpp>
#include <functional>
#include "operation_base.hpp"

namespace t3o
{
namespace detail
{

struct writing_operation_environment
{
	boost::asio::ip::tcp::socket& socket;
	std::function<void(operation_base&)> inner_operation_complete_handler;
};

}
}
