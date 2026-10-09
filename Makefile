# ============================================================
#  Makefile for Student Management System v3.0
#  Usage:
#    Windows (MinGW):  mingw32-make
#    Linux / macOS:    make
# ============================================================

CXX      = g++
CXXFLAGS = -std=c++17 -Wall -I include
SRC      = src/main.cpp src/Student.cpp src/StudentManager.cpp
TARGET   = sms

# Windows: add .exe extension
ifeq ($(OS), Windows_NT)
    TARGET := sms.exe
    RM     := del /Q
else
    RM     := rm -f
endif

# ── Default target: build ─────────────────────────────────
all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)
	@echo Build successful! Run: ./$(TARGET)

# ── Run after build ───────────────────────────────────────
run: $(TARGET)
	./$(TARGET)

# ── Clean compiled output ─────────────────────────────────
clean:
	$(RM) sms.exe sms *.o
	@echo Cleaned.

.PHONY: all run clean
