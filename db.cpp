#if !defined(NDEBUG)
#define BOOST_MULTI_INDEX_ENABLE_INVARIANT_CHECKING
#define BOOST_MULTI_INDEX_ENABLE_SAFE_MODE
#endif

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

using boost::multi_index_container;
using namespace boost::multi_index;
namespace bip=boost::interprocess;

typedef bip::basic_string<
        char,std::char_traits<char>,
        bip::allocator<char,bip::managed_mapped_file::segment_manager>
> shared_string;

struct db
{
    shared_string key;
    shared_string value;

    db(const shared_string::allocator_type& al):
            key(al),value(al)
    {}

    friend std::ostream& operator<<(std::ostream& os,const db& b)
    {
        os<<b.key<<": "<<b.value << std::endl;
        return os;
    }
};

typedef multi_index_container<
        db,
        indexed_by<
                ordered_unique<
                        BOOST_MULTI_INDEX_MEMBER(db,shared_string,key)
                >
        >,
        bip::allocator<db,bip::managed_mapped_file::segment_manager>
> db_container;


template<typename T>
void enter(const char* msg,T& t)
{
    std::cout<<msg;
    std::string str;
    std::getline(std::cin,str);
    std::istringstream iss(str);
    iss>>t;
}

void enter(const char* msg,std::string& str)
{
    std::cout<<msg;
    std::getline(std::cin,str);
}

void enter(const char* msg,shared_string& str)
{
    std::cout<<msg;
    std::string stdstr;
    std::getline(std::cin,stdstr);
    str=stdstr.c_str();
}

int main()
{
    bip::managed_mapped_file seg(
            bip::open_or_create,"./data.db",
            1048576);
    bip::named_mutex mutex(
            bip::open_or_create,"7FD6D7E8-320B-11DC-82CF-F0B655D89593");


    db_container* pbc=seg.find_or_construct<db_container>("db container")(
            db_container::ctor_args_list(),
            db_container::allocator_type(seg.get_segment_manager()));

    std::string command_info=
            "1. get value by key\n"
            "2. insert a key:value\n"
            "3. delete a key\n"
            "4. upate value by key\n"
            "0. exit\n";

    std::cout<<command_info;

    /* main loop */

    for(bool exit=false;!exit;){
        int command=-1;
        enter("command: ",command);

        switch(command){
            case 0:{ /* exit */
                exit=true;
                break;
            }
            case 1:{ /* list dbs by author */
                std::string key;
                enter("key (empty=all keys): ",key);

                /* operations with the container must be mutex protected */

                bip::scoped_lock<bip::named_mutex> lock(mutex);

                std::pair<db_container::iterator,db_container::iterator> rng;
                if(key.empty()){
                    rng=std::make_pair(pbc->begin(),pbc->end());
                }
                else{
                    rng=pbc->equal_range(
                            shared_string(
                                    key.c_str(),
                                    shared_string::allocator_type(seg.get_segment_manager())));
                }

                if(rng.first==rng.second){
                    std::cout<<"no records\n";
                }
                else{
                    std::copy(
                            rng.first,rng.second,std::ostream_iterator<db>(std::cout));
                }
                break;
            }
            case 2:{ /* insert a db */
                db b(shared_string::allocator_type(seg.get_segment_manager()));

                enter("key: ",b.key);
                enter("value: "  ,b.value);

                std::cout<<"insert the following?\n"<<b<<"(y/n): ";
                char yn='n';
                enter("",yn);
                if(yn=='y'||yn=='Y'){
                    bip::scoped_lock<bip::named_mutex> lock(mutex);
                    pbc->insert(b);
                }

                break;
            }
            case 3:{ /* delete a db */
                shared_string key(
                        shared_string::allocator_type(seg.get_segment_manager()));
                enter(
                        "key of record: ",
                        key);

                typedef nth_index<db_container,0>::type index_by_key;
                index_by_key&          idx=get<0>(*pbc);
                index_by_key::iterator it;
                db b(shared_string::allocator_type(seg.get_segment_manager()));

                {
                    /* Look for a db whose title begins with name. Note that we
                     * are unlocking after doing the search so as to not leave the
                     * container blocked during user prompting. That is also why a
                     * local copy of the db is done.
                     */

                    bip::scoped_lock<bip::named_mutex> lock(mutex);

                    it=idx.find(key);
                    if(it==idx.end()){
                        std::cout<<"no such db found\n";
                        break;
                    }
                    b=*it;
                }

                std::cout<<"delete the following?\n"<<b<<"(y/n): ";
                char yn='n';
                enter("",yn);
                if(yn=='y'||yn=='Y'){
                    bip::scoped_lock<bip::named_mutex> lock(mutex);
                    idx.erase(it);
                }

                break;
            }
            case 4:{
                db rec(shared_string::allocator_type(seg.get_segment_manager()));

                enter(
                        "key of record: ",
                        rec.key);
                enter("value: " , rec.value);

                typedef nth_index<db_container,0>::type index_by_key;
                index_by_key&          idx=get<0>(*pbc);
                index_by_key::iterator it;
                db b(shared_string::allocator_type(seg.get_segment_manager()));

                {
                    bip::scoped_lock<bip::named_mutex> lock(mutex);
                    it=idx.find(rec.key);
                    if(it==idx.end()){
                        std::cout<<"no such db found\n";
                        break;
                    }
                    b=*it;
                }

                bip::scoped_lock<bip::named_mutex> lock(mutex);
                idx.modify(it, [&rec](auto& p){p.value = rec.value;});

                break;
            }
            default:{
                std::cout<<"select one option:\n"<<command_info;
                break;
            }
        }
    }

    return 0;
}
