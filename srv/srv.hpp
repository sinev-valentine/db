
#ifndef SRV_HPP
#define SRV_HPP
#include <string>

namespace srv {
enum struct code_te {
    unknown = 0,
    ok = 200,
    invalid_json = 400,
    invalid_cmd = 404,
    server_error = 500
};

auto as_string = [](code_te code){
    switch (code) {
        case code_te::ok: return "ok";
        case code_te::invalid_json: return "invalid_json";
        case code_te::invalid_cmd: return "invalid_cmd";
        case code_te::server_error: return "server_error";
        default: return "unknown";
    }
};
}

#endif //SRV_HPP
