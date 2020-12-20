#ifndef CLIENT_HPP
#define CLIENT_HPP
#include <boost/core/noncopyable.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/system/error_code.hpp>
#include <boost/asio.hpp>

#define CRLF "\r\n\r\n"

namespace client {
    using boost::asio::ip::tcp;

    using handler_t = std::function<void(std::string&, int code)>;

class client : public boost::enable_shared_from_this<client>, boost::noncopyable{
    client(const std::string& ip, int port, std::vector<std::string>& head, std::string json, handler_t h);
    void do_connect();
    void do_read();
    void do_write();
    void close();
    void on_connect(const boost::system::error_code& );
    void on_write(const boost::system::error_code&, size_t);
    void on_read(const boost::system::error_code&, size_t);
    void on_read_content(const boost::system::error_code&, size_t);

    handler_t handler;
public:
    static boost::shared_ptr<client> instance(const std::string& ip, int port, std::vector<std::string>& head,
            std::string json, handler_t h);
    ~client();

    boost::asio::io_context io_context;
    boost::asio::ip::tcp::endpoint endpoint;
    boost::asio::ip::tcp::tcp::socket socket;
    boost::asio::streambuf rx;
    std::string tx;
    bool started;
    std::vector<std::string> headers;
    std::string m_request_content;
    std::string m_response_content;
    int http_code;
};

}
#endif //CLIENT_HPP
