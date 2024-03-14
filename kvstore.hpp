#include <string>
#include <unordered_map>
#include <iostream>
#include <vector>

namespace ns_reactor
{
    struct conn_item;
} // namespace ns_reactor

namespace ns_io_uring
{
    struct conn_item;
} // namespace ns_io_uring

//在io_uring.hpp 和 reactor.hpp中
#define BUFFER_LENGTH 1024



enum METHOD {
    METHOD_STRAT = 0,
    SET = METHOD_STRAT,
    GET,
    DEL,
    MOD,
    METHOD_END
};

static const std::vector<std::string> method_arr{"SET", "GET", "DEL", "MOD"};

bool kvstore_set(const std::string& key, const std::string& value);

std::pair<std::string, bool> kvstore_get(const std::string& key);

bool kvstore_del(const std::string& key);

bool kvstore_mod(const std::string& key, const std::string& value);


std::vector<std::string> kvstore_spilit(const char* msg);

template<typename T>
void kvstore_parse_protocol(const std::vector<std::string>& token, T& it) {
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

//调用：./netlib    
template<typename T>
void kv_request(T& it) {
    // std::cout << "kv_request: " << it.rbuffer << std::endl;
    auto token = kvstore_spilit(it.rbuffer);
    if (token.size() > 3 || token.size() == 1) return;
    // printf("method: %s;  key: %s;  value: %s\n", token[0].c_str(), token[1].c_str(), token[2].c_str());
    kvstore_parse_protocol(token, it);
}
