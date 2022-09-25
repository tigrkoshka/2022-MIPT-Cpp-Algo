### Как физически работает преобразование указателя на потомка к указателю на предка в случае разных типов наследования?

---

**_Пояснение:_** во всех комментариях в сниппетах в квадратные скобки взята часть памяти, не видная по соответствующему указателю

* Сначала рассмотрим одиночное наследование:

```c++
struct Parent { 
    int p;
}; 

struct Child : public Parent { 
    int c;
};
```

В этом случае `static_cast` не будет двигать указатель, 
ради этого затевались правила расположения в памяти (см. [первую задачу](Task1)):

```c++
int main() {
    Child c; //  Parent::p | Child::c
             // &c

    Parent* p = &c; //  Parent::p  [ Child::c ]
                    //  p
                    //
                    //  Численное значение указателя не поменялось,
                    //  то есть была затрачена только 1 машинная команда на сохранение указателя p


    std::cout << reinterpret_cast<long long>(&c)
                 - reinterpret_cast<long long>(p) << std::endl; // 0

    Child* cc = static_cast<Child*>(p); //  Parent::p | Child::c
                                        //  cc
                                        //
                                        //  Численное значение указателя не поменялось,
                                        //  то есть была затрачена только 1 машинная команда на сохранение указателя cc

    std::cout << reinterpret_cast<long long>(&c)
                 - reinterpret_cast<long long>(cc) << std::endl; // 0

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

В этом случае `static_cast` двигает указатели к началу места памяти результирующего класса
(он на компиляции знает, как данные будут расположены в памяти, поэтому знает, на сколько нужно двигать):

```c++
int main() {
    Child c; //  Mom::m | Dad::d | Child::c
             // &c

    Mom* cm = &c; //  Mom::m  [ Dad::d | Child::c ]
                  //  cm
                  //
                  //  Численное значение указателя не поменялось,
                  //  то есть была затрачена только 1 машинная команда на сохранение указателя mc

    std::cout << reinterpret_cast<long long>(cm)
                 - reinterpret_cast<long long>(&c) << std::endl; // 0

    Dad* cd = &c; //  [ Mom::m ]  Dad::d  [ Child::c ]
                  //              cd
                  //
                  //  Численное значение указателя поменялось,
                  //  то есть была затрачена дополнительная машинная команда

    std::cout << reinterpret_cast<long long>(cd)
                 - reinterpret_cast<long long>(&c) << std::endl; // 4

    Child* cmc = static_cast<Child*>(cm); //  Mom::m | Dad::d | Child::c
                                          //  cmc
                                          //
                                          //  Численное значение указателя не поменялось,
                                          //  то есть была затрачена только 1 машинная команда на сохранение указателя cmc

    std::cout << reinterpret_cast<long long>(cmc)
                 - reinterpret_cast<long long>(cm) << std::endl; // 0

    Child* cdc = static_cast<Child*>(cd); //  Mom::m | Dad::d | Child::c
                                          //  cdc
                                          //
                                          //  Численное значение указателя поменялось,
                                          //  то есть была затрачена дополнительная машинная команда

    std::cout << reinterpret_cast<long long>(cdc)
                 - reinterpret_cast<long long>(cd) << std::endl; // -4

    return 0;
}
```
