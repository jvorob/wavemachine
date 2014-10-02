CXX = g++
# Update these paths to match your installation
# You may also need to update the linker option rpath, which sets where to look for
# the SDL2 libraries at runtime to match your install
SDL_LIB = -L/usr/local/lib -lSDL2 -Wl,-rpath=/usr/local/lib
SDL_INCLUDE = -I/usr/local/include
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
