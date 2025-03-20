CXX = g++
CXXFLAGS := -std=c++23 -Wall -Wextra -Wpedantic -O3

server: src/server/main.cpp
	${CXX} ${CXXFLAGS} $^ -o $@ 
client:
	${CXX} ${CXXFLAGS} $^ -o $@

.PHONY: clean
clean:
	rm -f server client
