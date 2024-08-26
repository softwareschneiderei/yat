#include <iostream>
#include <yat/Version.h>

int main() {
    yat::Version::set("test_yat", "1.0");
    std::cout << yat::Version::get() << std::endl;
    return 0;
}
