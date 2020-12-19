#ifndef APP_SINGLETON_HPP
#define APP_SINGLETON_HPP

#include "srv.hpp"

namespace app{

struct app_singleton {
    static app_singleton& instance();
    std::pair<std::string, srv::code_te > operator()(const std::string&, const std::string&);
private:
    app_singleton();
    ~app_singleton();
};

}



#endif //APP_SINGLETON_HPP
