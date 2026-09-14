SRC = main.cpp
OUT = main.exe
HEAD = $(wildcard src/*.hpp gfx/*.hpp)

$(OUT): $(SRC) $(HEAD)
	g++ -Wall -std=c++23 -gdwarf -o $(OUT) $(SRC) \
		-IC:/bin/devlib/raylib/include -LC:/bin/devlib/raylib/lib \
		-lraylib -lopengl32 -lgdi32 -lwinmm -mwindows -mconsole \
		-IC:/bin/devlib/pugixml -LC:/bin/devlib/pugixml \
		-lpugixml
