#pragma once
#include <stdexcept>

namespace t3o
{
	class packet_format_error : std::runtime_error
	{
		public:
			explicit packet_format_error() :
				runtime_error("recived packet has wrong format")
			{
			}
	};
}
