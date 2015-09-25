CXX=mingw32-g++
PREFIX = F:/mingw
SDL_LIB = -L$(PREFIX)/SDL2-2.0.3/i686-w64-mingw32/lib -lSDL2main -lSDL2
SDL_INCLUDE = -I$(PREFIX)/SDL2-2.0.3/include

# If your compiler is a bit older you may need to change -std=c++11 to -std=c++0x
CXXFLAGS = -Wall -Wextra -c -std=c++11 $(SDL_INCLUDE)
LDFLAGS = -lmingw32 -mwindows -mconsole $(SDL_LIB)
OBJS=main.o graphics.o

lb1:	$(OBJS)
	$(CXX) $(OBJS) $(LDFLAGS) -o lb1

clean:
	rm -f lb1
	rm -f $(OBJS)

main.o: main.cpp
graphics.o: graphics.h graphics.cpp
