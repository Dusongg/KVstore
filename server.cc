#pragma once
#include "kvstore.hpp"
#include <vector>
#include <memory>
#include <sstream>

#include "./netlib/reactor.hpp"
// #include "./netlib/io_uring.hpp"

#define ENABLE_LOG

#ifdef ENABLE_LOG
//LOG("its a %s\n", "log");
#define LOG(_fmt, ...) fprintf(stdout, "[%s:%d]: %s"_fmt, __FILE__, __LINE__);

#else
#endif

#define NETWORK_REACTOR 1
#define NETWORK_IO_URING 2

//选择网络框架
#define ENABLED_NETWORK NETWORK_REACTOR

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

void kvstore_parse_protocol(const std::vector<std::string>& token, ns_reactor::conn_item& it) {
    int token_size = token.size();
    for (int i = METHOD_STRAT; i < METHOD_END; i++) {
        if (method_arr[i] == token[0]) {
            switch (i) {
                case SET: {
                    if (token_size == 3 && kvstore_set(token[1], token[2])) {
                        snprintf(it.wbuffer, BUFFER_LENGTH, "set success!\n");
                    } else {
                        snprintf(it.wbuffer, BUFFER_LENGTH, "set fail or key already exists!\n");
                    }
                    break;
                }
                case GET:{
                    auto ret_pair = kvstore_get(token[1]);
                    if (token_size == 2 && ret_pair.second == true) {
                        std::string& value = ret_pair.first;
                        snprintf(it.wbuffer, BUFFER_LENGTH, "get value: %s\n", value.c_str());
                    } else {
                        snprintf(it.wbuffer, BUFFER_LENGTH, "get failed!\n");
                    }

                    break;
                }
                case DEL:{
                    if (token_size == 2 && kvstore_del(token[1])) {
                        snprintf(it.wbuffer, BUFFER_LENGTH, "delete success!\n");
                    } else {
                        snprintf(it.wbuffer, BUFFER_LENGTH, "delete failed\n");
                    }
                    break;
                }
                case MOD:{
                    if (token_size == 3 && kvstore_mod(token[1], token[2])) {
                        snprintf(it.wbuffer, BUFFER_LENGTH, "modify success!\n");
                    } else {
                        snprintf(it.wbuffer, BUFFER_LENGTH, "modify failed!\n");
                    }
                    break;
                }
                default:
                    break;
            }
            break;
        }
    }

}

//声明：kvstore.hpp  调用：./netlib
template<typename T>
void reactor_kv_request(T& it) {
    // std::cout << "kv_request: " << it.rbuffer << std::endl;
    auto token = kvstore_spilit(it.rbuffer);
    if (token.size() > 3 || token.size() == 1) return;
    // printf("method: %s;  key: %s;  value: %s\n", token[0].c_str(), token[1].c_str(), token[2].c_str());
    kvstore_parse_protocol(token, it);
}


int main() {
#if (ENABLED_NETWORK == NETWORK_REACTOR)

    ns_reactor::reactor& entry = ns_reactor::reactor::get_Singleton();    
    entry.init();
    entry.run();

#elif (ENABLED_NETWORK == NETWORK_IO_URING)
#endif
}