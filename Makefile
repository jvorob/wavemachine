CXX = g++
# Update these paths to match your installation
# You may also need to update the linker option rpath, which sets where to look for
# the SDL2 libraries at runtime to match your install
SDL_LIB = -Lsdl_lib -lSDL2 -Wl,-rpath=sdl_lib/lib
SDL_INCLUDE = -Isdl_lib/include
# You may need to change -std=c++11 to -std=c++0x if your compiler is a bit older
CXXFLAGS = -Wall -c -std=c++11 $(SDL_INCLUDE)
LDFLAGS = $(SDL_LIB)
EXE = FluidTest

all: $(EXE)

$(EXE): main.o sim.o
	$(CXX) $^ $(LDFLAGS) -o $@

main.o: main.c sim.h
	$(CXX) $(CXXFLAGS) $< -o $@

sim.o: sim.c sim.h
	$(CXX) $(CXXFLAGS) $< -o $@


run: all
	./$(EXE)

clean:
	rm *.o && rm $(EXE)
