g++ -c rsdl.cpp
g++ rsdl.o demo1.cpp -lSDL2 -lSDL2_image -lSDL2_ttf -o help.out
g++ rsdl.o demo2.cpp -lSDL2 -lSDL2_image -lSDL2_ttf -o game_scene.out
g++ rsdl.o demo3.cpp -lSDL2 -lSDL2_image -lSDL2_ttf -o walking_pea.out
g++ rsdl.o demo4.cpp -lSDL2 -lSDL2_image -lSDL2_ttf -o suns.out
