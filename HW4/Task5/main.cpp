import Example;

int main() {
    MyNamespace::A a;
    a.f();

    MyNamespace::B b;
    b.g();

    // MyNamespace::f_internal(); // CE: 'f_internal' is not a member of 'MyNamespace'
    // MyNamespace::g_internal(); // CE: 'g_internal' is not a member of 'MyNamespace'
}