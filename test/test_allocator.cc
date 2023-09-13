#include<iostream>
#include "ircd/allocator/allocator.h"
#include<string>
#include<functional>

using namespace ircd::allocator;
using std::cout;
using std::endl;
using std::function;

void test_allocator_callback() {
    //test allocator int
    int *buf = new int(1);
    // int* firstVal = new(buf) int(1);
    // cout<<"int val:"<<*firstVal<<endl;
    function<int* (const size_t &, const int *const &)> int_ac = [&](const size_t & size, const int *const & p) -> int*{
        int* newVal = new((int* const)p) int(2);
        int* nextVal = new((int* const)p+1) int(3);
        cout<<"allocator int val:"<<*newVal<<" "<<*nextVal<<endl;
        return newVal;
    };
    //TODO:delete failed
    function<void (int *const &, const size_t &)> int_dc = [&](int *const & p, const size_t & size) {
        if(p != nullptr) {
            delete p;
            cout<<"deallocator int val "<< (p == nullptr?"success":"failed") <<endl;
        }
        return ;
    };
    struct callback<int> cb_int(int_ac, int_dc);
    auto ac_int= cb_int();
    ac_int.allocate(2, buf);
    ac_int.deallocate(buf, 2);

}

void test_allocator_state() {
    cout<<"init allocator 6 int"<<endl;
    int* buf= new int[6];
    state st(6, (unsigned long long* const)buf);
    cout<<"need 5 int, available: "<<(st.available(5) ? "true":"false")<<endl;

    cout<<"allocate 5 int, pos:"<< st.allocate(5, 0)<<endl;
    cout<<"need 2 int, available: "<<(st.available(2) ? "true":"false")<<endl;
    cout<<"need 1 int, available: "<<(st.available(1) ? "true":"false")<<endl;

    cout<<"deallocate 3 int from pos 1"<<endl;
    st.deallocate(1, 3);
    cout<<"need 4 int, available: "<<(st.available(4) ? "true":"false")<<endl;
    cout<<"need 3 int, available: "<<(st.available(3) ? "true":"false")<<endl;

    cout<<"allocate 2 int pos:"<< st.allocate(2, 0)<<endl;
    cout<<"need 2 int, available: "<<(st.available(2) ? "true":"false")<<endl;

    cout<<"deallocate 1 int from pos 4"<<endl;
    st.deallocate(4, 1);
    cout<<"need 2 int, available: "<<(st.available(2) ? "true":"false")<<endl;
    cout<<"allocate 2 int pos:"<< st.allocate(2, 0)<<endl;

}

int main(){
    test_allocator_callback();
    test_allocator_state();
    return 0;
}