#pragma once
#include "kvstore.hpp"
#include "reactor.hpp"
#include <vector>
#include <memory>
#include <sstream>
enum METHOD {
    METHOD_STRAT = 0,
    SET = METHOD_STRAT,
    GET,
    DEL,
    MOD,
    METHOD_END
};

static const std::vector<std::string> method_arr{"SET", "GET", "DEL", "MOD"};


std::vector<std::string> kvstore_spilit(const char* msg) {
    std::stringstream input(msg);
    std::vector<std::string> ret;
    std::string token;
    while(input >> token) {
        ret.push_back(token);
    }
    return ret;
}

void kvstore_parse_protocol(const std::vector<std::string>& token, my_reactor::conn_item& it) {
    for (int i = METHOD_STRAT; i < METHOD_END; i++) {
        if (method_arr[i] == token[0]) {
            switch (i) {
                case SET: {
                    if (kvstore_set(token[1], token[2])) {
                        snprintf(it.wbuffer, BUFFER_LENGTH, "set success!\n");
                        it.wlen = strlen(it.wbuffer);
                    } else {
                        snprintf(it.wbuffer, BUFFER_LENGTH, "set fail!\n");
                        it.wlen = strlen(it.wbuffer);
                    }
                    break;
                }
                case GET:{
                    auto& value = kvstore_get(token[1]);
                    if (value == "") {
                        std::cout << "get fail" << std::endl;
                        return;
                    }
                    snprintf(it.wbuffer, BUFFER_LENGTH, "%s\n", value.c_str());
                    it.wlen = value.size() + 1;
                    break;
                }
                case DEL:{
                    kvstore_del(token[1]);
                    break;
                }
                case MOD:{
                    kvstore_mod(token[0], token[1]);
                    break;
                }
                default:
                    break;
            }
            break;
        }
    }

}


void kv_request(my_reactor::conn_item& it) {
    // std::cout << "kv_request: " << it.rbuffer << std::endl;
    auto token = kvstore_spilit(it.rbuffer);
    if (token.size() > 3) return;
    // printf("method: %s;  key: %s;  value: %s\n", token[0].c_str(), token[1].c_str(), token[2].c_str());

    kvstore_parse_protocol(token, it);
}


int main() {
    my_reactor::reactor& entry = my_reactor::reactor::get_Singleton();    
    entry.init();
    entry.run();
}