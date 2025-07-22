# SIREN Sonar System
# MISRA C++ 2008 Compliant Makefile
# Single Responsibility: Build launcher only

CXX = clang++
CXXFLAGS = -std=c++17 -O3 -Wall -Wextra -DNDEBUG
TARGET = SIREN
SOURCE = main.cpp

# macOS M1 optimization
ifeq ($(shell uname -s),Darwin)
    ifeq ($(shell uname -m),arm64)
        CXXFLAGS += -mcpu=apple-m1
    endif
endif

# Default target
$(TARGET): $(SOURCE)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCE)

# Clean launcher only (preserve subsystem builds)
clean:
	rm -f $(TARGET)

# Build all subsystems (optional convenience)
build-all: backend frontend $(TARGET)

backend:
	$(MAKE) -C backend/build

frontend:
	$(MAKE) -C frontend/build

# Install launcher
install: $(TARGET)
	install -m 755 $(TARGET) /usr/local/bin/

.PHONY: clean build-all backend frontend install