
#include "app_singleton.hpp"
#include "cmd_singleton.hpp"

namespace app{

app_singleton::app_singleton(){}
app_singleton::~app_singleton(){}

app_singleton& app_singleton::instance(){
    static app_singleton instance;
    return instance;
}

app::cmd_list_te uri_to_enum(const std::string& uri){
    if (uri == "insert")
        return cmd_list_te::insert_;
    if (uri == "update")
        return cmd_list_te::update_;
    if (uri == "delete")
        return cmd_list_te::delete_;
    if (uri == "get")
        return cmd_list_te::get_;
    return cmd_list_te::unknown;
}

std::pair<std::string, srv::code_te > app_singleton::operator()(const std::string& uri, const std::string& json)
{
    auto cmd = uri_to_enum(uri);
    if (cmd == app::cmd_list_te::unknown){
        auto response = std::make_pair("", srv::code_te::invalid_cmd);
        return response;
    }
    try{
        auto& functor_map = cmd_singleton::instance();
        auto func = functor_map[cmd];
        return (*func)(json);
    }
    catch( const std::exception& e ) {
        auto response = std::make_pair("", srv::code_te::server_error);
        return response;
    }
}

}