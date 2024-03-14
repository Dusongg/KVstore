#pragma once

#define NETWORK_REACTOR 1
#define NETWORK_IO_URING 2

//选择网络框架
#define ENABLED_NETWORK NETWORK_REACTOR

#if (ENABLED_NETWORK == NETWORK_REACTOR)
//使用reactor建立通信
#include "./netlib/reactor.hpp"

#elif (ENABLED_NETWORK == NETWORK_IO_URING)
//使用io_uring建立通信
#include "./netlib/io_uring.hpp"

#endif


#define ENABLE_LOG

#ifdef ENABLE_LOG
//LOG("its a %s\n", "log");
#define LOG(_fmt, ...) fprintf(stdout, "[%s:%d]: %s"_fmt, __FILE__, __LINE__);

#else
#endif



int main() {
#if (ENABLED_NETWORK == NETWORK_REACTOR)

    ns_reactor::reactor& entry = ns_reactor::reactor::get_Singleton();    
    entry.init();
    entry.run();

#elif (ENABLED_NETWORK == NETWORK_IO_URING)
    ns_io_uring::proactor& entry = ns_io_uring::proactor::get_instance();
    entry.init();
    entry.run();
#endif
}