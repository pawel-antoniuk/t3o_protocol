#pragma once
#include <cstdint>
#include <sstream>
#include <cstring>
#include <boost/archive/text_iarchive.hpp>
#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream.hpp>

namespace t3o
{
namespace detail
{

class text_iserializer
{
public:
	template<typename Serializable>
	static Serializable process_input_data(uint8_t* input_data, std::size_t input_size)
	{
		using device_t = boost::iostreams::basic_array_source<char>;
		device_t device(reinterpret_cast<char*>(input_data), input_size);
		boost::iostreams::stream<device_t> strm(device);
		boost::archive::text_iarchive archive(strm, boost::archive::no_header);
		Serializable s;
		archive >> s;
		return s;
	}
};

}
}
