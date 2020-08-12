#!/usr/bin/env sh

cc -o main src/*.c -I includes/ -lglfw3 -framework Cocoa -framework OpenGL -framework IOKit