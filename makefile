all:
	gcc src/main.cpp -g -o pikassho `wx-config-gtk3 --libs --cxxflags --gl-libs` -lGL -lGLU -lGLEW -lm -lstdc++ -std=c++17
