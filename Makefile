CXX = x86_64-w64-mingw32-g++
CXXFLAGS = -Wall -std=c++17 -I./include -Os # Optimize for size
LDFLAGS = -s # Strip symbols from final executable

all: prep main_rust.exe main_cpp.exe

# Build Rust library first
prep:
	@echo "Building Rust library for GNU target..."
	cargo build --release --target x86_64-pc-windows-gnu

main_rust.exe: main_rust.cpp
	@echo "Building main_rust.exe..."
	$(CXX) $(CXXFLAGS) -o $@ $< -L./target/x86_64-pc-windows-gnu/release $(LDFLAGS) -lcrustyArchitecture -lws2_32 -luserenv -lntdll -lstdc++ -lgcc_s -lgcc -ladvapi32 -lkernel32 -lmingwex -lmsvcrt

# Build main_cpp.exe using only main_cpp.cpp (now uses std::queue)
main_cpp.exe: main_cpp.cpp
	@echo "Building main_cpp.exe (using std::queue)..."
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS)

clean:
	@echo "Cleaning..."
	cargo clean
	del /f /q main_rust.exe main_cpp.exe 2>nul || (exit 0)

.PHONY: all prep clean
