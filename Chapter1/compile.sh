#!/bin/bash
gcc -Wall `pkg-config --cflags glfw3` -o main main.cpp `pkg-config --static --libs glfw3` -framework OpenGL
