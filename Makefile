CXX = g++
CXXFLAGS := -std=c++23 -Wall -Wextra -Wpedantic -O3 -Isrc/common

.PHONY: all server client
all: server client
server: src/server/main.cpp
	${CXX} ${CXXFLAGS} $^ -o $@ 
client: src/client/main.cpp
	${CXX} ${CXXFLAGS} $^ -o $@

.PHONY: clean
clean:
	rm -f server client
