#pragma once
#include <cstdint>
#include <array>
#include <ctime>

namespace t3o
{
	namespace detail
	{
		namespace protocol
		{
			struct client_handshake
			{
				uint8_t mode;
				std::array<char, 16> name;

				template<typename Archive>
				void serialize(Archive& ar, const unsigned)
				{
					ar & mode & name;
				}

				static const unsigned packet_id = 1;
			};

			struct server_handshake
			{
				uint8_t symbol, width, height;

				template<typename Archive>
				void serialize(Archive& ar, const unsigned)
				{
					ar & symbol & width & height;
				}
				static const unsigned packet_id = 2;
			};

			struct feedback
			{
				uint8_t result;

				template<typename Archive>
				void serialize(Archive& ar, const unsigned)
				{
					ar & result;
				}
				static const unsigned packet_id = 3;
			};

			struct field_set_packet
			{
				uint8_t x, y, field;

				template<typename Archive> 
				void serialize(Archive& ar, const unsigned) 
				{ 
					ar & x & y & field;
				}
				static const unsigned packet_id = 4;
			};
			
			struct keepalive
			{
				time_t timestamp;

				template<typename Archive>
				void serialize(Archive& ar, const unsigned)
				{
					ar & timestamp;
				}
				static const unsigned packet_id = 5;
			};
			
			struct game_end
			{
				uint8_t result;

				template<typename Archive>
				void serialize(Archive& ar, const unsigned)
				{
					ar & result;
				}
				static const unsigned packet_id = 6;
			};

		}
	}
}
