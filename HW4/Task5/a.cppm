export module Example:A;

import Example:internals;
import <iostream>;

namespace MyNamespace {
    export struct A {
        void f() {
            std::cout << "Hello from A::f" << std::endl << "    ";
            f_internal();
        }
    };
}// namespace MyNamespace