
#ifndef DB_SERVICE_DB_SINGLETON_HPP
#define DB_SERVICE_DB_SINGLETON_HPP

#include <algorithm>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/string.hpp>
#include <boost/interprocess/managed_mapped_file.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/member.hpp>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>


namespace db_app{

using boost::multi_index_container;
using namespace boost::multi_index;
namespace bip=boost::interprocess;

typedef bip::basic_string<
        char,std::char_traits<char>,
        bip::allocator<char,bip::managed_mapped_file::segment_manager>
> shared_string;

struct db_record
{
    shared_string key;
    shared_string value;

    db_record(const shared_string::allocator_type& al):
            key(al),value(al)
    {}

    friend std::ostream& operator<<(std::ostream& os,const db_record& b){
        os<<b.key<<": "<<b.value << std::endl;
        return os;
    }
};

typedef multi_index_container<
db_record,indexed_by< ordered_unique<BOOST_MULTI_INDEX_MEMBER(db_record,shared_string,key)>>,
bip::allocator<db_record,bip::managed_mapped_file::segment_manager>
> db_container;

typedef nth_index<db_container,0>::type index_by_key;

enum struct op_status {
    ok = 0,
    key_absent,
    key_exist,
    value_match
};

struct db_singleton{

public:
    static db_singleton& instance();
    op_status insert(db_record&);
    op_status update(db_record&);
    op_status delete_(db_record&);
    op_status get_(db_record&);

private:
    db_singleton();
    ~db_singleton();

    bip::managed_mapped_file seg;
    bip::named_mutex mutex;
    db_container* pbc;
};

}
#endif //DB_SERVICE_DB_SINGLETON_HPP
