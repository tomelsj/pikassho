all:
	gcc src/main.cpp -g -o gl `wx-config-gtk3 --libs --cxxflags --gl-libs` -lGL -lGLU -lGLEW -lm -lstdc++ -std=c++17
