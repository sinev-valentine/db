#ifndef APP_CMD_SINGLETON_HPP
#define APP_CMD_SINGLETON_HPP

#include <iostream>
#include <boost/hana.hpp>
#include <boost/hana/range.hpp>
#include <boost/hana/for_each.hpp>
#include <boost/hana/size.hpp>
#include "json.hpp"
#include "srv.hpp"

namespace app {
struct table {
    std::string key;
    std::string value;
    table(const std::string& k, const std::string& v): key(k), value(v){}
};

struct field {
    std::string key;
    field(const std::string& k): key(k){};
};
}
BOOST_FUSION_ADAPT_STRUCT( app::table, key, value);
BOOST_FUSION_ADAPT_STRUCT( app::field, key);

namespace app{

namespace hana=boost::hana;

enum struct cmd_list_te{
    unknown=0,
    insert_,
    update_,
    delete_,
    get_,
    last
};

using api_cmd_result = std::pair<std::string, srv::code_te>;
struct cmd_handler_base {
    cmd_handler_base(){};
    virtual ~cmd_handler_base(){};
    virtual api_cmd_result operator()(const std::string&) const = 0;
};

template<cmd_list_te T>
struct cmd_handler : cmd_handler_base {
    virtual ~cmd_handler(){};
    virtual api_cmd_result operator()(const std::string&) const override{
        return std::make_pair("command is not implemented", srv::code_te::server_error);
    };
};

template<>
struct cmd_handler<cmd_list_te::insert_> : cmd_handler_base {
    virtual ~cmd_handler(){};
    virtual api_cmd_result operator()(const std::string& json) const override{
        try{
            auto rec = from_json<table>(json);
            std::cout << rec.key <<", " << rec.value << std::endl;
        }
        catch(std::exception& exc){
            return std::make_pair("json parse error", srv::code_te::invalid_json);
        }
    };
};

template<>
struct cmd_handler<cmd_list_te::update_> : cmd_handler_base {
    virtual ~cmd_handler(){};
    virtual api_cmd_result operator()(const std::string& json) const override{
        try{
            auto rec = from_json<table>(json);
            std::cout << rec.key <<", " << rec.value << std::endl;
        }
        catch(std::exception& exc){
            return std::make_pair("json parse error", srv::code_te::invalid_json);
        }
    };
};
template<>
struct cmd_handler<cmd_list_te::delete_> : cmd_handler_base {
    virtual ~cmd_handler(){};
    virtual api_cmd_result operator()(const std::string& json) const override{
        try{
            auto param = from_json<field>(json);
            std::cout << param.key << std::endl;
        }
        catch(std::exception& exc){
            return std::make_pair("json parse error", srv::code_te::invalid_json);
        }
    };
};
template<>
struct cmd_handler<cmd_list_te::get_> : cmd_handler_base {
    virtual ~cmd_handler(){};
    virtual api_cmd_result operator()(const std::string& json) const override{
        try{
            auto param = from_json<field>(json);
            std::cout << param.key << std::endl;
        }
        catch(std::exception& exc){
            return std::make_pair("json parse error", srv::code_te::invalid_json);
        }
    };
};

struct cmd_singleton {
    using handler_ptr = std::shared_ptr<cmd_handler_base>;
    static const cmd_singleton& instance();
    const handler_ptr operator[](cmd_list_te) const;
private:
    cmd_singleton();
    ~cmd_singleton();
    std::vector<handler_ptr> m_handler_list;
};

constexpr auto cmd_static_list =
        hana::make_range(
                hana::int_c<static_cast<int>(cmd_list_te::unknown)>,
                hana::int_c<static_cast<int>(cmd_list_te::last)>);
}

#endif //APP_CMD_SINGLETON_HPP
