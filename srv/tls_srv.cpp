
#include "tls_srv.hpp"
#include <signal.h>
#include <utility>

namespace srv {

void server::run(){
    do_accept();
    io_context_.run();
}

void server::on_await_stop(const boost::system::error_code& ec, int signo){
    acceptor_.close();
    tls_peer_list_.stop_all();
}

void server::do_await_stop(){
    signals_.async_wait(boost::bind(&server::on_await_stop, this, _1, _2));
}

void server::do_accept(){
    acceptor_.async_accept( [this](const boost::system::error_code& ec, boost::asio::ip::tcp::socket socket){
        if (!acceptor_.is_open()){
            return;
        }
        if (!ec){
            tls_peer_list_.start(std::make_shared<tls_peer>(
                    std::move(socket), tls_peer_list_, handler));
        }
        do_accept();
    });
}

server::server(const std::string& address, const std::string& port, handler_t func):
            io_context_(1),
            signals_(io_context_),
            acceptor_(io_context_),
            tls_peer_list_(),
            handler(func)  {

    signals_.add(SIGINT);
    signals_.add(SIGTERM);
#if defined(SIGQUIT)
    signals_.add(SIGQUIT);
#endif // defined(SIGQUIT)

    do_await_stop();

    boost::asio::ip::tcp::resolver resolver(io_context_);
    boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(address, port).begin();
    acceptor_.open(endpoint.protocol());
    acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
    acceptor_.bind(endpoint);
    acceptor_.listen();
}

}
