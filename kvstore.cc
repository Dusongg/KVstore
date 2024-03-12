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
static 


std::vector<std::string> kvstore_spilit(const char* msg) {
    std::stringstream input(msg);
    std::vector<std::string> ret;
    std::string token;
    while(input >> token) {
        ret.push_back(token);
    }
    return ret;
}

void kvstore_parse_protocol(const std::vector<std::string>& token) {

}


void kv_request(const my_reactor::conn_item& it) {
    // std::cout << "kv_request: " << it.rbuffer << std::endl;
    auto token = kvstore_spilit(it.rbuffer);
    if (token.size() != 3) return;
    printf("method: %s;  key: %s;  value: %s\n", token[0].c_str(), token[1].c_str(), token[2].c_str());

}


int main() {
    std::unique_ptr<my_reactor::reactor> entry(my_reactor::reactor::get_Singleton());    
    entry->init();
    entry->run();
}