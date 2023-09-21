#include<iostream>
#include<string>
#include<functional>
#include<cstring>
#include<memory>
#include<limits>

using namespace ircd::allocator;
using namespace ircd::buffer;
using ircd::string_view;
using std::cout;
using std::endl;
using std::function;
using std::distance;
using std::string;

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
    cout<<"-----------test state start--------------------"<<endl;

    cout<<"init allocator 6 unsigned long long"<<endl;
    unsigned long long* buf= new unsigned long long[6];
    state st(6, (unsigned long long* const)buf);
    cout<<"need 5 unsigned long long, available: "<<(st.available(5) ? "true":"false")<<endl;
    cout<<"allocate 5 unsigned long long, pos:"<< st.allocate(5, 0)<<endl;
    cout<<"need 2 unsigned long long, available: "<<(st.available(2) ? "true":"false")<<endl;

    cout<<"deallocate 3 int from pos 1"<<endl;
    st.deallocate(1, 3);
    cout<<"need 4 int, available: "<<(st.available(4) ? "true":"false")<<endl;
    cout<<"allocate 2 int pos:"<< st.allocate(2, 0)<<endl;
    cout<<"deallocate 1 int from pos 4"<<endl;
    st.deallocate(4, 1);
    cout<<"need 2 int, available: "<<(st.available(2) ? "true":"false")<<endl;
    cout<<"allocate 2 int pos:"<< st.allocate(2, 0)<<endl;
    cout<<"need 1 int, available: "<<(st.available(1) ? "true":"false")<<endl;
    cout<<"allocate 1 int pos:"<< st.allocate(1, 0)<<endl;
    cout<<"-----------test state end--------------------"<<endl;

}

void test_allocator_dynamic() {
    cout<<"-----------test dynamic start--------------------"<<endl;
    cout<<"init dynamic with 17 int"<<endl;
    dynamic<int> dn(17);
    auto dn_ac = dn();
    cout<<"dynamic allocate 10 int, ";
    int* p = dn_ac.allocate(10);
    for(int i=0; i<10; i++) {
        p[i] = i;
    }
    cout<<"pos:"<<p<<endl;
    cout<<"dynamic allocate 2 int, ";
    int* p1 = dn_ac.allocate(2);
    cout<<"pos:"<<p1<<endl;
    cout<<"tow pointer distance: "<<distance(p, p1)<<endl;
    
    cout<<"dynamic deallocate 6 int from pos 2"<<endl;
    dn_ac.deallocate(p+2, 6);
    cout<<"dynamic allocate 4 int "<<endl;
    int* p2 = dn_ac.allocate(4);
    cout<<"p2 to p distance:"<<distance(p, p2)<<endl;
    cout<<"dynamic allocate 5 int "<<endl;
    int* p3 = dn_ac.allocate(5);
    cout<<"p3 to p distance:"<<distance(p, p3)<<endl;
    cout<<"dynamic data size: "<<dn_ac.max_size()<<endl;
    cout<<"dynamic header size: "<<distance((int*)dn_ac.getArena(), p)<<endl;
    cout<<"-----------test dynamic end-------------------------"<<endl;

}

void test_allocator_fixed() {
    cout<<"-----------test fixed start--------------------"<<endl;
    cout<<"init fixed with 10 int"<<endl;
    fixed<int, 10> fx;
    auto fx_ac = fx();
    cout<<"fixed allocate 5 int "<<endl;
    auto *p_fixed = fx_ac.allocate(5);
    cout<<"fixed allocate 3 int "<<endl;
    auto *p1_fixed = fx_ac.allocate(3);
    cout<<"p_fixed and p1_fixed distance: "<<distance(p_fixed, p1_fixed)<<endl;
    cout<<"fixed deallocate 4 from pos 4 "<<endl;
    fx_ac.deallocate(p_fixed+4, 4);
    cout<<"fixed allocate 6 int "<<endl;
    auto *p2_fixed = fx_ac.allocate(6);
    cout<<"p_fixed and p2_fixed distance: "<<distance(p_fixed, p2_fixed)<<endl;
    cout<<"-----------test fixed end--------------------"<<endl;
}

void test_allocator_allocate() {
    cout<<"-----------test allocate start--------------------"<<endl;
    string s = "test_allocator";
    char* ptr = allocate(16, s.size());
    buffer bf(ptr, s.size());
    for(int i=0; i<s.size();i++) {
        bf[i] = s[i];
    }
    cout<<"bf : "<<bf<<endl;
    mutable_buffer mt_bf(bf);
    cout<<"mt_bf : "<<mt_bf<<endl;
    cout<<"-----------test allocate end--------------------"<<endl;
}


int main(){
    test_allocator_callback();
    test_allocator_state();
    test_allocator_dynamic();
    test_allocator_fixed();
    test_allocator_allocate();
    return 0;
}