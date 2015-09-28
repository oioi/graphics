CXX=mingw32-g++
PREFIX = D:/v.petrov/mingw
SDL_LIB = -L$(PREFIX)/SDL2-2.0.3/i686-w64-mingw32/lib -lSDL2main -lSDL2
SDL_INCLUDE = -I$(PREFIX)/SDL2-2.0.3/include

CXXFLAGS = -Wall -Wextra -c -std=c++11 -g $(SDL_INCLUDE)
LDFLAGS = -lmingw32 -mwindows -mconsole $(SDL_LIB)
OBJS=main.o graphics.o

lb1:	$(OBJS)
	$(CXX) $(OBJS) $(LDFLAGS) -o lb1

clean:
	rm -f lb1
	rm -f $(OBJS)

main.o: graphics.h main.cpp
graphics.o: graphics.h graphics.cpp
