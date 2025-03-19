flags := -std=c++20 -Wall -Wextra -Wpedantic -O3

server_bin: server/src/main.cpp
	g++ $^ ${flags} -o $@ 
client_bin:
	g++ $^ ${flags} -o $@

.PHONY: clean
clean:
	rm -f server_bin client_bin
