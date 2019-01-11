BIN=info-beamer
LINKS=-lglfw -lGLEW -lGLU -lGL -lX11 -lpthread -lXrandr -lXi -ldl -lIL -lILU 
# Debugging links are costly, comment out when not needed 
DEBUG=-g -ggdb -ggdb3

all: main.cpp
	g++ -o $(BIN) $(LINKS) $(DEBUG) main.cpp