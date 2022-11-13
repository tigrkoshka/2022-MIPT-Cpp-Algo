export module Example:B;

import Example:internals;
import <iostream>;

namespace MyNamespace {
    export struct B {
        void g() {
            std::cout << "Hello from B::g" << std::endl << "    ";
            g_internal();
        }
    };
}// namespace MyNamespace