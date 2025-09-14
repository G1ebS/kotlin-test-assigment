CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra
SRC = main.cpp
TARGET = library_app

all: $(TARGET)

$(TARGET): $(SRC) library_app.h library.h users.h book.h
    $(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

clean:
    del $(TARGET).exe 2>nul || rm -f $(TARGET)