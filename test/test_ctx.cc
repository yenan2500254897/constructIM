#include<iostream>
#include<string>

using std::cout;
using std::endl;
using std::string;

#if defined(BOOST_ASIO_HAS_EPOLL)

extern "C" int
__real_epoll_wait(int __epfd,
                  struct epoll_event *__events,
                  int __maxevents,
                  int __timeout);

extern "C" int
__wrap_epoll_wait(int __epfd,
                  struct epoll_event *const __events,
                  int __maxevents,
                  int __timeout)
{
	// see addl documentation in ircd/ios
	return ircd::ios::epoll_wait<__real_epoll_wait>
	(
		__epfd,
		__events,
		__maxevents,
		__timeout
	);
}

#endif

void printStr(string s){
    // ircd::ctx::ole::init _ole_; 
    cout<<"hello "<<s<<endl;
}

void test_ctx() {
    boost::asio::io_context io_context;
    ircd::init(io_context.get_executor());
    io_context.run();
    cout<<"main thread id:"<<ircd::ios::main_thread_id<<" is main thread: "<<ircd::ios::is_main_thread<<endl;
}

int main() {
    test_ctx();
    return 0;
}
