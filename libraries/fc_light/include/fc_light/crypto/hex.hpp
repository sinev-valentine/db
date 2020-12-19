#pragma once
#include <fc_light/string.hpp>
#include <fc_light/utility.hpp>
#include <vector>

namespace fc_light {
    uint8_t from_hex( char c );
    fc_light::string to_hex( const char* d, uint32_t s );
    std::string to_hex( const std::vector<char>& data );

    /**
     *  @return the number of bytes decoded
     */
    size_t from_hex( const fc_light::string& hex_str, char* out_data, size_t out_data_len );
} 
