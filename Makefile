CXX = x86_64-w64-mingw32-g++
CXXFLAGS = -Wall -std=c++17 -I./include
LDFLAGS = -static

all: prep main_rust.exe main_cpp.exe

# Build Rust library first
prep:
	@echo "Building Rust library..."
	cargo build --release

main_rust.exe: main_rust.cpp
	@echo "Building main_rust.exe..."
	$(CXX) $(CXXFLAGS) -o $@ $< -L./target/release -lcrustyArchitecture $(LDFLAGS)

main_cpp.exe: main_cpp.cpp fifo_cpp.cpp
	@echo "Building main-cpp.exe..."
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	@echo "Cleaning..."
	cargo clean
	rm -f main_rust.exe main_cpp.exe

.PHONY: all prep clean