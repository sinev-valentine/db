#include <iostream>
#include "tls_srv.hpp"
#include <functional>
#include "app_singleton.hpp"
#include <boost/program_options.hpp>
#include "db_singleton.hpp"


int main(int argc, char* argv[]) {

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
    }
    catch (std::exception& e){
        std::cout << desc << std::endl;
        return 0;
    }

    try{
        auto& app = app::app_singleton::instance();
        srv::handler_t handler = std::bind(&app::app_singleton::operator(),
                                           &app,
                                           std::placeholders::_1,
                                           std::placeholders::_2);
        auto& db = app::db_singleton::instance();
        srv::timer_handler_t statictic_logger = std::bind(&app::db_singleton::statistic_log, &db);
        auto server = std::make_shared<srv::server>(ip, port, handler, statictic_logger);
        server->run();
    }
    catch(const std::exception& e ) {
        std::cout<< e.what() << std::endl;
    }
    return 0;
}

