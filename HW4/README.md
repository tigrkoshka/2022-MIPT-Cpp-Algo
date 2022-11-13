### Инструкция по запуску

В этом задании активно используются нововведения стандарта C++20, которые поддержаны пока что 
далеко не всеми компиляторами. Так, Apple Clang 14.0.0 и LLVM Clang 15.0.3 (последние версии на момент 
написания в ноябре 2022) не поддерживают `std::views`. 

Однако, версия GCC 12.2.0 уже поддерживает `std::views`, поэтому будем пользоваться ею. 

GCC не поставляется на MacOS "из коробки", а существующая команда `g++` является всего лишь псевдонимом Apple Clang,
в чем можно убедиться, запустив команду:

```bash
g++ -v
```

Она на момент написания выдает:

```
Apple clang version 14.0.0 (clang-1400.0.29.202)
Target: x86_64-apple-darwin21.6.0
Thread model: posix
InstalledDir: /Library/Developer/CommandLineTools/usr/bin
```

В связи с этим, для запуска этого кода на MacOS нужно проделать следующие действия:

  * Установить GCC (версия указана для воспроизводимости последующих команд, можно устанавливать без указания версии,
    тогда будет установлена последняя версия, но поменяются пути до компилятора и стандартной библиотеки)

```shell
brew install gcc@12
```

  * Компилировать код с помощью установленного компилятора и при использовании 
    "родной" для него версии стандартной библиотеки. Пример для [Task 1](Task1.cpp):

```shell
/usr/local/opt/gcc@12/bin/g++-12 -std=c++20 -L /usr/local/opt/gcc@12/include/c++/12 Task1.cpp -o Task1
```






















