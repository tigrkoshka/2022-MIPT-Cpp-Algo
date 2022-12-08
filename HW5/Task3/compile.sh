#!/bin/bash

BOOST_LIB=$(brew info boost | grep "$(brew --prefix)/Cellar/boost" | awk '{print $1;}')

clang++ -std=c++17 \
        -lboost_filesystem \
        -lboost_graph \
        -lboost_program_options \
        -I "$BOOST_LIB" \
        Boost.cpp \
        -o ./bin/grapher