CXX = x86_64-w64-mingw32-g++
CXXFLAGS = -Wall -std=c++17 -IC:\Users\Owner\CRUSTy-Architecture-Example\include
LDFLAGS = -static
PROJECT_DIR = C:\Users\Owner\CRUSTy-Architecture-Example

all: $(PROJECT_DIR)/main_rust.exe $(PROJECT_DIR)/main_cpp.exe

$(PROJECT_DIR)/main_rust.exe: $(PROJECT_DIR)/main_rust.cpp
	@echo "Building main_rust.exe..."
	$(CXX) $(CXXFLAGS) -o $@ $^ -L$(PROJECT_DIR)/target/release -lcrustyArchitecture $(LDFLAGS) || exit 1

$(PROJECT_DIR)/main_cpp.exe: $(PROJECT_DIR)/main_cpp.cpp $(PROJECT_DIR)/fifo_cpp.cpp
	@echo "Building main_cpp.exe..."
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS) || exit 1

clean:
	@echo "Cleaning..."
	cargo clean
	rm -f $(PROJECT_DIR)/main_rust.exe $(PROJECT_DIR)/main_cpp.exe

.PHONY: all clean
