#include "db_singleton.hpp"

namespace app {

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

op_status db_singleton::insert(table& rec){
    bip::scoped_lock<bip::named_mutex> lock(mutex);
    db_record b(shared_string::allocator_type(seg.get_segment_manager()));
    b.key = rec.key.c_str();
    b.value = rec.value.c_str();

    auto& ind = get<0>(*pbc);
    auto it = ind.find(b.key);
    if (it != ind.end())
        return op_status::key_exist;
    pbc->insert(b);
    return op_status::ok;
}


op_status db_singleton::update(table& rec){
    bip::scoped_lock<bip::named_mutex> lock(mutex);
    db_record b(shared_string::allocator_type(seg.get_segment_manager()));
    b.key = rec.key.c_str();
    b.value = rec.value.c_str();

    auto& ind = get<0>(*pbc);
    auto it = ind.find(b.key);
    if(it == ind.end())
        return op_status::key_absent;
    if(it->value == b.value)
        return op_status::value_match;
    ind.modify(it, [&b](auto& p){p.value = b.value;});
    return op_status::ok;
}

op_status db_singleton::delete_(field& param ){
    bip::scoped_lock<bip::named_mutex> lock(mutex);
    db_record b(shared_string::allocator_type(seg.get_segment_manager()));
    b.key = param.key.c_str();

    auto& ind = get<0>(*pbc);
    auto it = ind.find(b.key);
    if(it == ind.end())
        return op_status::key_absent;
    ind.erase(it);
    return op_status::ok;
}

op_status db_singleton::get_(field& param, field& res) {
    bip::scoped_lock<bip::named_mutex> lock(mutex);
    db_record b(shared_string::allocator_type(seg.get_segment_manager()));
    b.key = param.key.c_str();

    auto& ind = get<0>(*pbc);
    auto it = ind.find(b.key);
    if(it == ind.end())
        return op_status::key_absent;
    res.key = it->value.c_str();
    return op_status::ok;
}

}