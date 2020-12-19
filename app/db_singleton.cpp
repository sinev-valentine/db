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
    bip::scoped_lock<bip::named_mutex> lock(mutex);
    auto& ind = get<0>(*pbc);
    auto it = ind.find(rec.key);
    if (it != ind.end())
        return op_status::key_exist;
    db_record b(shared_string::allocator_type(seg.get_segment_manager()));
    b.key = rec.key;
    b.value = rec.value;
    pbc->insert(b);
    return op_status::ok;
}


op_status db_singleton::update(db_record& rec){
    bip::scoped_lock<bip::named_mutex> lock(mutex);
    auto& ind = get<0>(*pbc);
    auto it = ind.find(rec.key);
    if(it == ind.end())
        return op_status::key_absent;
    if(it->value == rec.value)
        return op_status::value_match;
    ind.modify(it, [&rec](auto& p){p.value = rec.value;});
    return op_status::ok;
}

op_status db_singleton::delete_(std::string& key ){
    bip::scoped_lock<bip::named_mutex> lock(mutex);
    auto& ind = get<0>(*pbc);
    db_record b(shared_string::allocator_type(seg.get_segment_manager()));
    b.key = key.c_str();
    auto it = ind.find(b.key);
    if(it == ind.end())
        return op_status::key_absent;
    ind.erase(it);
    return op_status::ok;
}

op_status db_singleton::get_(std::string& key, std::string& value) {
    bip::scoped_lock<bip::named_mutex> lock(mutex);
    auto& ind = get<0>(*pbc);
    db_record b(shared_string::allocator_type(seg.get_segment_manager()));
    b.key = key.c_str();
    auto it = ind.find(b.key);
    if(it == ind.end())
        return op_status::key_absent;
    value = it->value.c_str();
    return op_status::ok;
}

}