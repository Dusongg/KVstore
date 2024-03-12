#pragma once
#include <unordered_map>
#include <iostream>
namespace my_reactor {
    struct conn_item;
}

void kv_request(const my_reactor::conn_item& it);

static std::unordered_map<std::string, std::string> store;

bool kvstore_set(const std::string& key, const std::string& value) {
    return true;
}

auto kvstore_get(const std::string& key) -> decltype(store[key]) {
    return store[key];
}

bool kvstore_del(const std::string& key) {
    return true;
}

bool kvstore_mod(const std::string& key, const std::string& value) {
    return true;
}