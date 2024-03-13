#pragma once
#include <unordered_map>
#include <iostream>

namespace ns_reactor {
    struct conn_item;
}

namespace ns_io_uring
{
    struct conn_item;
} // namespace 


static std::unordered_map<std::string, std::string> store;

template<typename T>
void reactor_kv_request(T& it);

bool kvstore_set(const std::string& key, const std::string& value);

std::pair<std::string, bool> kvstore_get(const std::string& key);

bool kvstore_del(const std::string& key);

bool kvstore_mod(const std::string& key, const std::string& value);