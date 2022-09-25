### Что такое виртуальное наследование, к каким последствиям оно приводит? Как расположены данные в памяти? 

--- 

Рассмотрим ромбовидное наследование (_diamond problem_):

```c++
struct Granny {
    int g;
};

struct Mom : public Granny {
    int m;
};

struct Dad : public Granny {
    int d;
};

struct Child : public Mom, public Dad {
    int c;
};
```

В таком случае объект типа `Child` содержит два экземпляра `Granny`, 
поскольку и он содержит по одному экземпляру `Mom` и `Dad`, 
каждый из которых содержит по одному экземпляру `Granny`.

В связи с этим невозможно разрешить обращение `Chind.g` 
(непонятно к какому экземпляру `Granny` обращаться).
Приходится указывать путь к конкретному экземпляру:

```c++
int main() {
    Child c; // Mom::Granny::g | Mom::m | Dad::Granny::g | Dad::d | Child::c

    // c.g; --> CE! "member found by ambiguous name lookup"

    c.Mom::g; // OK
    c.Dad::g; // OK

    std::cout << reinterpret_cast<long long>(&c.Dad::g)
                 - reinterpret_cast<long long>(&c.Mom::g) << std::endl; // 8

    return 0;
}
```

Для предотвращения создания двух экземпляров можно использовать виртуальное наследование:

```c++
// initialize fields, because otherwise the default constructor cannot in case of virtual inheritance

struct Granny {
    int g = 239;
};

struct Mom : public virtual Granny {
    int m = 1;
};

struct Dad : public virtual Granny {
    int d = 2;
};

struct Child : public Mom, public Dad {
    int c = 3;
};
```

В таком случае при создании экземпляра `Child`
создаётся только один экземпляр `Granny`, 
принадлежащий одновременно и `Mom`, и `Dad`.

```c++
int main() {
    Child c;

    c.g; // OK

    c.Mom::g; // OK
    c.Dad::g; // OK

    std::cout << reinterpret_cast<long long>(&c.Dad::g)
                 - reinterpret_cast<long long>(&c.Mom::g) << std::endl; // 0

    std::cout << reinterpret_cast<long long>(&c.g)
                 - reinterpret_cast<long long>(&c.Mom::g) << std::endl; // 0

    return 0;
}
```

Реализуется такое поведение через указатель на `vtable`:

```c++
int main() {
    Child c; // Mom::vptr | Mom::m | <padding> | Dad::vptr | Dad::d | Child::c | Granny::g | padding
             //
             // The first padding occurs, because Dad::vptr is 8 bytes, but Mom::m only takes up 4,
             // so 4 more bytes is needed to reach the beginning of a new machine word
             //
             // The second padding occurs because an object must take up an integer number of machine words

    std::cout << reinterpret_cast<long long>(&c.m)
                 - reinterpret_cast<long long>(&c) << std::endl; // 8

    std::cout << reinterpret_cast<long long>(&c.d)
                 - reinterpret_cast<long long>(&c.m) << std::endl; // 16 (4 from Mom::m + 4 from padding + 8 from Dad::vptr)

    std::cout << reinterpret_cast<long long>(&c.c)
                 - reinterpret_cast<long long>(&c.d) << std::endl; // 4

    std::cout << reinterpret_cast<long long>(&c.g)
                 - reinterpret_cast<long long>(&c.c) << std::endl; // 4

    return 0;
}
```

Проблемы виртуального наследования:

 - Занимается сильно больше памяти из-за `vptr` (в данном примере 40 байт вместо 20)


 - Усложняется обращение к полям, так как надо разыменовывать много указателей 
   (в данном примере `c.g` это на самом деле `c -> Mom::vptr -> vtable -> c.g`)
