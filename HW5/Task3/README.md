### Содержимое

* Исходные коды:
  * Решение, проходящее тесты на Leetcode: [Simple.cpp](Simple.cpp)
  * Решение с использованием библиотеки `Boost`: [Boost.cpp](Boost.cpp)
* Скомпилированный бинарь для [Boost.cpp](Boost.cpp): [bin/grapher](bin/grapher)
* Пример входного файла: [in/input.txt](in/input.txt)
* Пример результата работы программы (для входного файла, указанного выше):
  * Сам граф: [out/graph.dot](out/graph.dot)
  * Одно из остовных деревьев: [out/mst.dot](out/mst.dot)
  * Критические и псевдокритические рёбра: [out/edges.dot](out/edges.dot)
* Пример визуализации получившихся графов:
  * Сам граф: [png/graph.png](png/graph.png)
  * Одно из остовных деревьев: [png/mst.png](png/mst.png)
  * Критические и псевдокритические рёбра: [png/edges.png](png/edges.png)
* Скрипт запуска бинаря (подробнее про его работу ниже): [run.sh](run.sh)

### Компиляция [Boost.cpp](Boost.cpp):

Чтобы скомпилировать [Boost.cpp](Boost.cpp) необходимо (на MacOS):

* Установить библиотеку `Boost`:

```shell
brew install boost
```

* Посмотреть, где установлена библиотека, можно, запустив:

```shell
brew info boost | grep "$(brew --prefix)/Cellar/boost" | awk '{print $1;}'
```

* Скомпилировать с включением необходимых библиотек:

```shell
chmod +x compile.sh && ./compile.sh
```

### Генерирование картинок

Чтобы сгенерировать визуальное представление графов, необходимо (на MacOS):

* Установить утилиту `dot`:

```shell
brew install graphviz
```

* Запустить её, указав входной и выходной файл:

```shell
dot -Tpng <input.dot> -o <output.png>
```

### Запуск

После компилирования бинаря [bin/grapher](bin/grapher) и установки утилиты `dot`
можно запустить в текущей директории скрипт [run.sh](run.sh), который ожидает, 
что файл [in/input.txt](in/input.txt) содержит входные данные, запускает на них 
[bin/grapher](bin/grapher), складывает выходные файлы в [out/](out),
а соответствующие картинки в формате `.png` в [png/](png):

```shell
chmod +x run.sh && ./run.sh
```

Можно менять входные данные в [in/input.txt](in/input.txt) и перезапускать скрипт,
при этом результаты будут перезаписаны.

В получившемся файле [png/edges.png](png/edges.png) критические рёбра отмечены красным цветом,
а псевдокритические — синим.