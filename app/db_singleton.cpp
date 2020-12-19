#include "db_singleton.hpp"

namespace db_app {

db_singleton::db_singleton():
seg(bip::open_or_create,"./data.db", 1048576),
mutex(bip::open_or_create,"7FD6D7E8-320B-11DC-82CF-F0B655D89593"),
pbc(seg.find_or_construct<db_container>("db container")(db_container::ctor_args_list(),
            db_container::allocator_type(seg.get_segment_manager()))){}

db_singleton::~db_singleton() {};

db_singleton &db_singleton::instance() {
    static db_singleton instance;
    return instance;
}

op_status db_singleton::insert(db_record& rec){
    db_record b(shared_string::allocator_type(seg.get_segment_manager()));
    index_by_key& idx = get<0>(*pbc);
    index_by_key::iterator it;

    bip::scoped_lock<bip::named_mutex> lock(mutex);
    it = idx.find(rec.key);
    if (it != idx.end())
        return op_status::key_exist;
    b.key = rec.key;
    b.value = rec.value;
    pbc->insert(b);
    return op_status::ok;
}


op_status db_singleton::update(db_record& rec){
    db_record b(shared_string::allocator_type(seg.get_segment_manager()));
    index_by_key& idx = get<0>(*pbc);
    index_by_key::iterator it;

    bip::scoped_lock<bip::named_mutex> lock(mutex);
    it=idx.find(rec.key);
    if(it == idx.end())
        return op_status::key_absent;
    if(it->value == rec.value)
        return op_status::value_match;
    b=*it;
    idx.modify(it, [&rec](auto& p){p.value = rec.value;});
    return op_status::ok;
}

}