
#include "client.hpp"
#include <boost/asio/ip/address.hpp>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio/read_until.hpp>
#include "srv.hpp"
#define DEFAULT_CONTEXT

namespace client {

using handler_t = std::function<void(std::string&, int code)>;


    client::client(const std::string& ip, int port, std::vector<std::string>& head, std::string request_json, handler_t h):
            endpoint(boost::asio::ip::address::from_string(ip), port),
            started(true), headers(head), m_request_content(request_json), io_context(DEFAULT_CONTEXT), socket(io_context),
            handler(h), m_response_content(""), http_code(static_cast<int>(srv::code_te::unknown)){
    }

client::~client(){
    socket.close();
}

void client::on_read_content(const boost::system::error_code& ec, std::size_t bytes_transferred){

    if (!ec){
        std::string content( (std::istreambuf_iterator<char>(&rx)), std::istreambuf_iterator<char>() );
        m_response_content += content.substr(0, bytes_transferred);
        handler(m_response_content, http_code);
        close();
    }
    else if (ec != boost::asio::error::operation_aborted){
        close();
    }

}
void client::on_read(const boost::system::error_code& ec, std::size_t bytes_transferred){
    if (!ec){
        std::istream strm(&rx);
        uint32_t content_length=0;
        bool found = false;
        std::string value, http_status;
        strm >> http_status >> http_status;
        try{
            http_code = std::stoi(http_status);
        }
        catch(std::exception & exc){
            http_code = static_cast<int>(srv::code_te::unknown);
        }

        while(!strm.eof()){
            if (found){
                strm >> content_length;
                break;
            }
            strm >> value;
            std::transform(value.begin(), value.end(), value.begin(), ::tolower);
            if (value=="content-length:"){ found = true;}
        }

        std::string fragment( (std::istreambuf_iterator<char>(&rx)), std::istreambuf_iterator<char>() );
        auto pos = fragment.find(CRLF);
        if (pos != std::string::npos){
            m_response_content = fragment.substr(pos+4);
        }

        if (m_response_content.length() < content_length){
            boost::asio::async_read(socket, rx, boost::asio::transfer_exactly(content_length-m_response_content.length()),
                                          boost::bind(&client::on_read_content, shared_from_this(), _1, _2));
        }
        else{
            handler(m_response_content, http_code);
            close();
        }

    }
    else if (ec != boost::asio::error::operation_aborted){
        close();
    }
}

void client::do_read(){

    boost::asio::async_read_until(socket, rx, CRLF,
            boost::bind(&client::on_read, shared_from_this(), _1, _2));
}

void client::do_write(){
    if(!started) return;
    for (auto& a : headers){
        tx += a + "\r\n";
    }
    if(m_request_content.size()){
        tx += "Content-Length: "+ std::to_string(m_request_content.size())+"\r\n";
    }
    tx += "\r\n"+m_request_content;

    boost::asio::async_write(socket, boost::asio::buffer(tx),
                            boost::bind(&client::on_write, shared_from_this(), _1, _2));
}

void client::on_write(const boost::system::error_code& error, size_t bytes){
    do_read();
}

void client::do_connect(){
    socket.async_connect(endpoint, boost::bind(&client::on_connect, shared_from_this(), _1));
    io_context.run();
}

void client::on_connect(const boost::system::error_code& error){
    if (!error){
        do_write();
    } else{
        std::cout << "https connection failed" << std::endl;
        close();
    }
}

boost::shared_ptr<client> client::instance(const std::string& ip, int port, std::vector<std::string>& head,
        std::string request_json, handler_t h){

    auto instance = boost::shared_ptr<client>(new client(ip, port, head, request_json, h));
    instance->do_connect();
    return instance;
}

void client::close(){
    if(!started) return;
    started = false;
    socket.close();
}

}

