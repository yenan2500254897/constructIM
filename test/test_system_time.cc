#include <iostream>

using std::cout;
using std::endl;

using namespace ircd;
void test_timedate() {
    cout<<(system_point)now()<<endl;
}

int main() {
    test_timedate();
    return 0;
}