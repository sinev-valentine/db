
#include <iostream>
#include "client.hpp"
#include "srv.hpp"
#include "cmd_singleton.hpp"
#include <boost/program_options.hpp>
#include "json.hpp"

std::atomic<int> count;

std::string enum_to_uri(const app::cmd_list_te cmd){
    switch (cmd) {
        case app::cmd_list_te::insert_: return "insert";
        case app::cmd_list_te::update_: return "update";
        case app::cmd_list_te::delete_: return "delete";
        case app::cmd_list_te::get_: return "get";
        default: return "unknown";
    }
}

void send_message(const app::cmd_list_te cmd, const std::string& key, const std::string& value,
                  std::string& ip, int port) {
    try {
        auto http_handler = [](const std::string result, int http_code) {
            if (http_code != static_cast<int>(srv::code_te::ok)) {
                std::cout << result << std::endl;
            }
            count++;
        };

        std::string uri, body;
        app::table table;
        table.key = key;
        table.value = value;
        app::field field;
        field.key = key;
        switch (cmd) {
            case app::cmd_list_te::insert_:
                uri = "insert";
                body = app::to_json(table);
                break;
            case app::cmd_list_te::update_:
                uri = "update";
                body = app::to_json(table);
                break;
            case app::cmd_list_te::delete_:
                uri = "delete";
                body = app::to_json(field);
                break;;
            case app::cmd_list_te::get_:
                uri = "get";
                body = app::to_json(field);
                break;
            default:
                uri = "unknown";
        }
        std::vector<std::string> headers = {"POST " + enum_to_uri(cmd), "Host: " + ip + ":" + std::to_string(port)};
        client::handler_t handler = std::bind(http_handler, std::placeholders::_1, std::placeholders::_2);
        client::client::instance(ip, port, headers, body, handler);
    }
    catch( const std::exception& e ) {
       std::cout << e.what() << std::endl;
    }
}

int main(int argc, char* argv[]){
    namespace po = boost::program_options;
    std::string ip, port;
    int port_;
    po::options_description desc("Options");
    desc.add_options()
            ("help,h", "show help")
            ("ip,i", po::value<std::string>(&ip), "server ip")
            ("port,p", po::value<std::string>(&port), "server port");

    po::variables_map options;
    try
    {
        po::parsed_options parsed = po::command_line_parser(argc, argv).options(desc).allow_unregistered().run();
        po::store(parsed, options);
        po::notify(options);

        if (options.count("help") ||
            options.count("ip") == 0 ||
            options.count("port") == 0
            ){
            std::cout << desc << std::endl;
            return 0;
        }

        boost::system::error_code ec;
        boost::asio::ip::address::from_string( ip, ec );
        port_ = std::stoi(port);
        if (ec || port_ < 1024 || port_ > 65535) {
            std::cout << desc << std::endl;
            return 0;
        }

        while(true){
            std::cout << "введите комманду: " << std::endl;
            std::cout << "0 - exit" << std::endl;
            std::cout << "1 - insert" << std::endl;
            std::cout << "2 - update" << std::endl;
            std::cout << "3 - delete" << std::endl;
            std::cout << "4 - get" << std::endl;
            std::string str;
            std::getline(std::cin, str);
            std::istringstream ss(str);
            int cmd;
            ss>>cmd;

            std::string key, value;
            switch (static_cast<app::cmd_list_te>(cmd)) {
                case app::cmd_list_te::unknown: return 0;
                case app::cmd_list_te::insert_:
                case app::cmd_list_te::update_: {
                    std::cout << "key: ";
                    std::getline(std::cin, key);
                    std::cout << "value: ";
                    std::getline(std::cin, value);
                    break;
                }
                case app::cmd_list_te::delete_:
                case app::cmd_list_te::get_:{
                    std::cout << "key: ";
                    std::getline(std::cin, key);
                    break;
                }
                default: continue;
            }
//            int cnt = count;
            send_message(static_cast<app::cmd_list_te>(cmd), key, value, ip, port_);
//            if (cnt == count)
//                return 0;
        }
    }
    catch (std::exception& e){
        std::cout << desc << std::endl;
        return 0;
    }

}