#include <iostream>
#include "tls_srv.hpp"
#include <functional>
#include "app_singleton.hpp"

int main(int argc, char* argv[]) {
    std::string ip = "0.0.0.0";
    uint32_t port = 1024;

    try{
        auto& app = app::app_singleton::instance();
        srv::handler_t handler = std::bind(&app::app_singleton::operator(),
                                                    &app,
                                                    std::placeholders::_1,
                                                    std::placeholders::_2);
        auto port_str = std::to_string(port);
        auto server = std::make_shared<srv::server>(ip, port_str, handler);
        server->run();
    }
    catch(const std::exception& e ) {
        std::cout<< e.what() << std::endl;
    }
    return 0;

}