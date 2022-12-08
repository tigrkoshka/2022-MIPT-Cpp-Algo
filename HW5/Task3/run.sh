#!/bin/bash

set -e

./bin/grapher --input ./in/input.txt \
              --output-graph ./out/graph.dot \
              --output-mst ./out/mst.dot \
              --output-edges ./out/edges.dot

dot -Tpng ./out/graph.dot -o ./png/graph.png
dot -Tpng ./out/mst.dot -o ./png/mst.png
dot -Tpng ./out/edges.dot -o ./png/edges.png