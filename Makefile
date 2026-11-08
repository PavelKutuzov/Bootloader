BUILD_DIR := artifacts/build

.PHONY: all configure build clean distclean

all: build

configure:
	cmake -S . -B $(BUILD_DIR) -DCMAKE_TOOLCHAIN_FILE=cmake/arm-none-eabi.cmake

build: configure
	cmake --build $(BUILD_DIR) -j

clean:
	cmake --build $(BUILD_DIR) --target clean
