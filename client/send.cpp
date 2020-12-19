
#include <iostream>
#include "client.hpp"
#include "srv.hpp"
#include "cmd_singleton.hpp"
#include <fstream>

#include <fc_light/exception/exception.hpp>
#include "sha_wrapper.hpp"
#include <fc_light/crypto/base64.hpp>
#include "xml_singleton.hpp"
#include "send.hpp"
#include <boost/program_options.hpp>
#include <boost/fusion/sequence.hpp>
#include <boost/fusion/include/sequence.hpp>
#include <boost/fusion/algorithm.hpp>
#include <boost/fusion/include/algorithm.hpp>

response_t result;
std::string response_content;

response_t* send_message(const std::string& key, const std::string& value) {
    try{
        std::string ip = "0.0.0.0";
        uint32_t port = 1024;

        result.params = const_cast<char*>("");

        auto http_handler = [](const std::string json, int http_code) {
            if (http_code != static_cast<int>(srv::code_te::ok)) {
                response_content.assign("response error:  " +
                            fc_light::variant(static_cast<srv::code_te >(http_code)).as_string()+ " " + json);
                result.params = const_cast<char*>(response_content.c_str());
                result.res = false;
                return;
            }
            asio_app::cmd_handler<asio_app::cmd_list_te::xml_sign>::content_t response;
            try{
                 response = fc_light::json::from_string(json).as<asio_app::cmd_handler<asio_app::cmd_list_te::xml_sign>::content_t>();
            }
            catch(fc_light::bad_cast_exception& ext){
                response_content.assign("bad cast response: "+ json);
                result.params = const_cast<char*>(response_content.c_str());
                result.res = false;
                return;
            }
            // if OK, then verify
            auto& xml = asio_app::xml_singleton::instance();
            result.res = xml.verify(response.raw_signed_xml);
            response_content.assign(response.raw_signed_xml.data(), response.raw_signed_xml.size());
            result.params = const_cast<char*>(response_content.c_str());
        };

        namespace fields
        {
            struct key;
            struct value;
        }

        typedef map<fusion::pair<fields::key, std::string> , fusion::pair<fields::value, std::string> >  data;

        struct Foo_s { int i; char k[100]; };
        BOOST_FUSION_ADAPT_STRUCT( Foo_s,  (int, i)  (char, k[100]) )

        struct Bar_s { int v; Foo_s w; };
        BOOST_FUSION_ADAPT_STRUCT( Bar_s, (int, v)  (Foo_s, w) )

        template <typename T2> struct Dec_s {  static void decode(T2   & f); };
        struct AppendToTextBox {
            template <typename T>
            void operator()(T& t) const {
                //decode T and t as the original code here...
                Dec_s<T>::decode(t);
            }
        };

        template <typename T2> void Dec_s<T2>::decode(T2 & f) {
            for_each(f, AppendToTextBox());
        };
        template<> void Dec_s<int >::decode(int  & f) {};
        template<> void Dec_s<char>::decode(char & f) {};

        Bar_s f = { 2, { 3, "abcd" } };
        Dec_s<Bar_s>::decode(f);

        std::string xml(request->params);
        std::vector<char> raw_xml(xml.begin(), xml.end());

        asio_app::params_t params;
        params.raw_xml = std::move(raw_xml);
        auto body = fc_light::json::to_string(fc_light::variant(params));
        auto uri = std::string (request->cmd);

        std::vector<std::string> headers = { "POST "+ uri,"Host: "+ip+":"+std::to_string(port)};
        client::handler_t handler = std::bind(http_handler, std::placeholders::_1, std::placeholders::_2 );
        client::client::instance(ip, port, headers, body, handler);

        return &result;

    }catch(fc_light::exception& exc){
        auto error_msg = asio_app::exc_handler(exc);
        result.res = false;
        result.params = const_cast<char*>(error_msg.c_str());
    }
    catch( const std::exception& e ) {
        auto error_msg = fc_light::json::to_pretty_string(
                fc_light::variant(asio_app::err_msg(0, fc_light::std_exception_code,  std::string(e.what()))));
        result.res = false;
        result.params = const_cast<char*>(error_msg.c_str());
    }
    return &result;
}

int main(int argc, char* argv[]){

    std::string ip, cmd, key, val;
    po::options_description desc("Options");
    desc.add_options()
            ("help,h", "Show help")
            ("ip", po::value<std::string>(&task_type), "server IP");
            ("cmd", po::value<std::string>(&task_type), "command");
            ("key", po::value<std::string>(&task_type), "key");
            ("val", po::value<std::string>(&task_type), "value");

    po::variables_map options;
    try
    {
        po::parsed_options parsed = po::command_line_parser(argc, argv).options(desc).allow_unregistered().run();
        po::store(parsed, options);
        po::notify(options);

        if (options.count("help") ||
            options.count("ip") == 0 ||
            options.count("cmd") == 0 ||
            options.count("key") == 0 ||
            ){
            std::cout << desc << std::endl;
            return 0;
        }

        struct sockaddr_in sa;
        if (inet_pton(AF_INET, ip.c_str(), &(sa.sin_addr) == 0){
            std::cout << desc << std::endl;
            return 0
        }
    }
    catch (std::exception& ex)
    {
        std::cout << desc << std::endl;
        return 0;
    }

}