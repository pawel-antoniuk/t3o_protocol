#include <boost/archive/text_oarchive.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/device/back_inserter.hpp>
#include <boost/utility/string_ref.hpp>

namespace t3o
{
namespace detail
{

class text_oserializer
{
public:
	template<typename Serializable>
	static std::size_t process_output_data(const Serializable& input_data,
			uint8_t* output_data, std::size_t output_size)
	{
		using device_t = boost::iostreams::array_sink;
		device_t buffer(reinterpret_cast<char*>(output_data), output_size);
		boost::iostreams::stream<device_t> strm(buffer);
		boost::archive::text_oarchive archive(strm, boost::archive::no_header);
		archive << input_data;
		strm.flush();
		return strm.tellp();
	}
};

}
}
