#include "kvstore.hpp"


template<typename T>
void kv_request(T& it);


bool kvstore_set(const std::string& key, const std::string& value) {
    auto ret = store.emplace(key, value);
    return ret.second;
}

std::pair<std::string, bool> kvstore_get(const std::string& key){
    /*C++20
    if (!store.contains(key)) return "";
    else {
        return store[key];
    }
    */
    if (store.find(key) != store.end()) {
        return {store[key], true};
    } else {
        return {"", false};
    }
}

bool kvstore_del(const std::string& key) {
    if (store.find(key) != store.end()) {
        size_t n_erase = store.erase(key);
        return n_erase == 1;
    } else {
        return false;
    }
}

bool kvstore_mod(const std::string& key, const std::string& value) {
    if (store.find(key) == store.end())  {
        kvstore_set(key, value);
    }
    store[key] = value;
    return true;
}