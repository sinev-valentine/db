#include <fc_light/io/datastream.hpp>
#include <fc_light/exception/exception.hpp>

NO_RETURN void fc_light::detail::throw_datastream_range_error(char const* method, size_t len, int64_t over)
{
  FC_LIGHT_THROW_EXCEPTION( out_of_range_exception, "${method} datastream of length ${len} over by ${over}", ("method",fc_light::string(method))("len",len)("over",over) );
}
