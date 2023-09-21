#include <iostream>
#include <vector>
#include <string>
#include <tuple>
#include <functional>
#include <cstring>

using namespace ircd::buffer;
using std::cout;
using std::endl;
using std::vector;
using std::string;


void test_buffer_base() {
    cout<<"-----------------------test buffer buffer_base start---------------------"<<endl;
    vector<int> vec(5, 0);
    int limit = vec.size();
    for(int i=1; i<limit;i++){
        vec[i] = i;
    }
    buffer bf_base(vec.begin(), vec.end());
    int n = size(bf_base);
    for(int i=0; i<n;i++){
        cout<<"bf_base["<<i<<"] = "<<bf_base[i]<<endl;
    }
    bf_base[4] = 5;
    cout<<"change bf_base["<< 4 <<"] to "<<bf_base[4]<<endl;
    cout<<"-----------------------test buffer buffer_base end-----------------------"<<endl;

    cout<<"-----------------------test buffer mutable_buffer start---------------------"<<endl;
    string s = "hello";
    mutable_buffer mt_bf(s);
    s.insert(s.begin()+1, 'e');
    cout<<"after inserted, s = "<< (string)(mt_bf) <<endl;
    char c_arr[] = "hello world";
    mutable_buffer mt_bf1(c_arr);
    cout<<"mt_bf1 = "<< (string)(mt_bf1) <<endl;
    cout<<"-----------------------test buffer mutable_buffer end---------------------"<<endl;

    cout<<"-----------------------test buffer const_buffer start---------------------"<<endl;
    char ct_arr[] = "hello const buffer";
    const_buffer ct_bf(ct_arr);
    cout<<"ct_bf = "<< (string)(ct_bf) <<endl;
    cout<<"-----------------------test buffer const_buffer end---------------------"<<endl;

    cout<<"-----------------------test buffer fixed_buffer start---------------------"<<endl;
    // fixed_buffer<const_buffer, 30> fx_bf(ct_bf);
    // cout<<"fx_bf = "<<fx_bf[0]<<endl;
    cout<<"-----------------------test buffer fixed_buffer end---------------------"<<endl;
}

int main() {
    test_buffer_base();
    return 0;
}

