#ifndef DB_SERVICE_JSON_HPP
#define DB_SERVICE_JSON_HPP

#include <boost/program_options.hpp>
#include <boost/fusion/sequence.hpp>
#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <nlohmann/json.hpp>
#include <boost/fusion/include/mpl.hpp>
#include <boost/mpl/range_c.hpp>

struct A {
    std::string key;
    std::string value;
};

BOOST_FUSION_ADAPT_STRUCT( A, key, value);

namespace db_lib {

namespace mpl=boost::mpl;
using json = nlohmann::json;

template <typename T>
struct from_fusion{
    from_fusion(const T& seq, json& j):seq_(seq), res_(j){}
    const T& seq_;
    json& res_;
    template <typename ind>
    void operator() (ind i) const{
        std::string key = boost::fusion::extension::struct_member_name<T,i>::call();
        res_[key] =  boost::fusion::at<ind>(seq_);
    }
};

template<typename T>
std::string to_json(T const& v){
    json j;
    typedef mpl::range_c<unsigned, 0, boost::fusion::result_of::size<T>::value > range;
    boost::fusion::for_each(range(), from_fusion<T>(v, j));
    return j.dump();
}

template <typename T>
struct to_fusion{
    to_fusion(T& seq, json& j):seq_(seq), res_(j){}
    T& seq_;
    json& res_;
    template <typename ind>
    void operator() (ind i) const{
        std::string key = boost::fusion::extension::struct_member_name<T,i>::call();
        boost::fusion::at<ind>(seq_) = res_[key];
    }
};

template <typename T>
T from_json(std::string& json_str){
    json j = json::parse(json_str);
    T v;
    typedef mpl::range_c<unsigned, 0, boost::fusion::result_of::size<T>::value > range;
    boost::fusion::for_each(range(), to_fusion<T>(v, j));
    return v;
}

}

#endif //DB_SERVICE_JSON_HPP
