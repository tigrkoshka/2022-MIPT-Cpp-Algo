module Example:internals;

import <iostream>;

namespace MyNamespace {
    void f_internal() {
        std::cout << "Hello from f_internal!" << std::endl;
    }

    void g_internal() {
        std::cout << "Hello from g_internal!" << std::endl;
    }
}// namespace MyNamespace