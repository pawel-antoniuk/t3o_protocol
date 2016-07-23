#pragma once
#include <boost/asio.hpp>
#include <queue>
#include <functional>
#include <memory>
#include <iostream> //debug
#include "operation_base.hpp"

namespace t3o
{
namespace detail
{

namespace detail
{
	using namespace boost::asio;
}

class operation_queue_base
{
protected:
	virtual ~operation_queue_base(){}

	operation_queue_base() :
		_is_executing(false)
	{
	}
	
	void push_operation(std::unique_ptr<operation_base>&& operation)
	{
		//std::cout << "[DEBUG]: pushed new operation: " 
		//	<< typeid(*operation).name() << std::endl;
		//std::cout << "queue size: " << _operations.size() << std::endl;
		_operations.push(std::move(operation));
		_update_queue();
	}
	
	void inner_operation_complete(operation_base& operation)
	{
		//std::cout << "[DEBUG]: operation complete: " 
		//	<< typeid(operation).name() << std::endl;
		//std::cout << "queue size: " << _operations.size() << std::endl;
		_operations.pop();
		_is_executing = false;
		_update_queue();
	}

	void clean_queue()
	{
		decltype(_operations) empty;
		_operations.swap(empty);
	}

private:
	void _update_queue()
	{
		if(_is_executing) return;
		if(!_operations.empty())
		{
			_operations.front()->async_execute();	
			_is_executing = true;
		}
	}
	
	std::queue<std::unique_ptr<operation_base>> _operations;
	bool _is_executing;
};

}
}
