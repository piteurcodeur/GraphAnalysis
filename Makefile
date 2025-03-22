CXX = g++
QT_DIR = /mingw64

CXXFLAGS = -Wall -Iinclude -std=c++17 `pkg-config --cflags Qt5Widgets` \
    -I/mingw64/include \
    -I$(QT_DIR)/include/Qt3DCore \
    -I$(QT_DIR)/include/Qt3DExtras \
    -I$(QT_DIR)/include/Qt3DRender \
    -I$(QT_DIR)/include/Qt3DInput \
    -I$(QT_DIR)/include/Qt3DLogic \
    -I$(QT_DIR)/include/Qt3DAnimation \
    -I$(QT_DIR)/include/Qt3DQuick \
    -I$(QT_DIR)/include/Qt3DQuickExtras \
    -I$(QT_DIR)/include/Qt3DQuickRender \
    -I$(QT_DIR)/include/Qt3DQuickScene2D \
    -I$(QT_DIR)/include/Qt3DQuickInput \
    -I$(QT_DIR)/include/Qt3DQuickAnimation

LDFLAGS = -L"C:/msys64/mingw64/lib" `pkg-config --libs Qt5Widgets` \
    -L/mingw64/lib \
    -lboost_graph-mt

LIBS = -lQt53DCore -lQt53DExtras -lQt53DRender

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
