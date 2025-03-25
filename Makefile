CXX = g++
QT_DIR = /mingw64

CXXFLAGS = -Wall -Iinclude -std=c++17 `pkg-config --cflags Qt5Widgets Qt53DCore Qt53DExtras Qt53DRender` \
    -I/mingw64/include

LDFLAGS = -L"C:/msys64/mingw64/lib" `pkg-config --libs Qt5Widgets Qt53DCore Qt53DExtras Qt53DRender` \
    -L/mingw64/lib \
    -lboost_graph-mt

LIBS = 

SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(SOURCES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
EXECUTABLE = $(BIN_DIR)/prog

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS) $(LIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ_DIR)/*.o $(EXECUTABLE)

.PHONY: all clean
