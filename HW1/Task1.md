### Как физически располагаются данные классов в памяти в случае одиночного и в случае множественного наследования?

---

* Сначала рассмотрим одиночное наследование:

```c++
struct Parent { 
    int p;
}; 

struct Child : public Parent { 
    int c;
};
```

В этом случае данные располагаются строго последовательно: сначала родитель, 
потом ребёнок, в данном случае: 

```c++
int main() {
    Child c; // Parent::p | Child::c
    std::cout << reinterpret_cast<long long>(&c.c)
                 - reinterpret_cast<long long>(&c.p) << std::endl; // 4

    return 0;
}
```

* Теперь рассмотрим множественное наследование:

```c++
struct Mom { 
    int m;
}; 

struct Dad { 
    int d;
}; 

struct Child : public Mom, public Dad { 
    int c;
};
```

В этом случае также сначала располагаются родители, потом ребёнок, 
порядок расположения родителей не определён стандартом,
но на практике всегда будет тот же, который в объявлении наследования: в данном случае:

```c++
int main() {
    Child c; // Mom::m | Dad::d | Child::c
    std::cout << reinterpret_cast<long long>(&c.c)
                 - reinterpret_cast<long long>(&c.d) << std::endl << // 4
              reinterpret_cast<long long>(&c.d)
              - reinterpret_cast<long long>(&c.m) << std::endl; // 4

    return 0;
}
```

* Случай виртуального наследования описан [во второй задаче](Task2.md)
