# IAR Toolchain Configuration
CC_IAR = iccarm
LD_IAR = ilinkarm

# Target CPU Core (Adjust if necessary)
CPU_CORE = Cortex-M7

# Compiler Flags
CFLAGS_IAR = --cpu $(CPU_CORE) -Iinclude -Oh --debug --diag_suppress Pa050

# Linker Flags
LDFLAGS_IAR = --config Pflash_mirroredEnv.icf

# Rust Build Configuration
RUST_TARGET = thumbv7em-none-eabi
RUST_LIB_PATH = target/$(RUST_TARGET)/release
RUST_LIB = libcrustyArch.a

# C++ Source and Object
C_SRC = main.cpp
C_OBJ = main.o

# Final Output
OUTPUT = crusty_firmware.out

# Default target
all: $(OUTPUT)

# Build Rust library
$(RUST_LIB_PATH)/$(RUST_LIB): Cargo.toml src/lib.rs src/c_api.rs build.rs
	cargo build --release --target $(RUST_TARGET)

# Compile C++ source
$(C_OBJ): $(C_SRC) include/CrustyArch.h Makefile
	$(CC_IAR) $(CFLAGS_IAR) $(C_SRC) -o $(C_OBJ)

# Link final executable
$(OUTPUT): $(C_OBJ) $(RUST_LIB_PATH)/$(RUST_LIB) Pflash_mirroredEnv.icf Makefile
	$(LD_IAR) $(LDFLAGS_IAR) $(C_OBJ) $(RUST_LIB_PATH)/$(RUST_LIB) -o $(OUTPUT)

# Clean build artifacts
clean:
	cargo clean
	rm -f $(C_OBJ) $(OUTPUT)

# Phony targets
.PHONY: all clean
