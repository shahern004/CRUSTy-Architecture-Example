CXX = mingw32-g++-4.7.2.exe
CXXFLAGS = -Wall -std=c++11 -Iinclude
LDFLAGS = -static

all: prep main

prep:
		cargo build -v --release --target thumbv7em-none-eabi

main: main.cpp
		$(CXX) $(CXXFLAGS) -o $@ $< -L./target/debug -lcrustyArch $(LDFLAGS)

clean:
		cargo clean
		rm -f main.exe

.PHONY: all prep clean
