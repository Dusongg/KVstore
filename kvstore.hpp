#pragma once
#include <iostream>
namespace my_reactor {
    struct conn_item;
}

void kv_request(const my_reactor::conn_item& it);

