CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -Wpedantic -O2
LDFLAGS  :=

SRC_DIR  := src
OBJ_DIR  := build
BIN_DIR  := bin

TARGET   := $(BIN_DIR)/sim

SRCS := $(SRC_DIR)/Config.cpp \
        $(SRC_DIR)/IPRange.cpp \
        $(SRC_DIR)/LoadBalancer.cpp \
        $(SRC_DIR)/LogFileStats.cpp \
        $(SRC_DIR)/Request.cpp \
        $(SRC_DIR)/RequestGenerator.cpp \
        $(SRC_DIR)/Simulate.cpp \
        $(SRC_DIR)/WebServer.cpp \
        $(SRC_DIR)/main.cpp

OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -I$(SRC_DIR) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)