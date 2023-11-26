CXX := g++
FLAGS = -std=c++11 -Wall -Wextra
RELEASE_FLAGS := -O2
DEBUG_FLAGS := -g3 -DDEBUG

FLAGS += $(RELEASE_FLAGS)

BIN_DIR := bin
BUILD_DIR := build
MODULES_DIR := modules
SRC_DIR := src

LSH := $(BIN_DIR)/lsh_main
CUBE := $(BIN_DIR)/cube_main
GRAPH := $(BIN_DIR)/graph_search

LSH_OBJ := $(BUILD_DIR)/lsh_main.o
CUBE_OBJ := $(BUILD_DIR)/cube_main.o
GRAPH_OBJ := $(BUILD_DIR)/graph_search.o

SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
LIBS := $(shell find $(MODULES_DIR) -name '*.hpp')

LSH_MODULES := $(shell find $(MODULES_DIR)/Lsh -name '*.cpp')
CUBE_MODULES := $(shell find $(MODULES_DIR)/Cube -name '*.cpp')
GRAPH_MODULES := $(shell find $(MODULES_DIR)/Graphs -name '*.cpp')
COMMON_MODULES := $(shell find $(MODULES_DIR)/Common -name '*.cpp')
ALL_MODULES := $(shell find $(MODULES_DIR) -name '*.cpp')

LSH_OBJ_MODULES := $(LSH_MODULES:$(MODULES_DIR)/Lsh/%.cpp=$(BUILD_DIR)/Lsh/%.o)
CUBE_OBJ_MODULES := $(CUBE_MODULES:$(MODULES_DIR)/Cube/%.cpp=$(BUILD_DIR)/Cube/%.o)
GRAPH_OBJ_MODULES := $(GRAPH_MODULES:$(MODULES_DIR)/Graphs/%.cpp=$(BUILD_DIR)/Graphs/%.o)
COMMON_OBJ_MODULES := $(COMMON_MODULES:$(MODULES_DIR)/Common/%.cpp=$(BUILD_DIR)/Common/%.o)
ALL_OBJ_MODULES := $(ALL_MODULES:$(MODULES_DIR)/%.cpp=$(BUILD_DIR)/%.o)

OBJ_MODULES_DEBUG := $(ALL_MODULES:$(MODULES_DIR)/%.cpp=$(BUILD_DIR)/%-deb.o)

EXEC_FILES := $(SRC_FILES:$(SRC_DIR)/%.cpp=$(BIN_DIR)/%)
INCLUDE_DIRS := $(shell find $(MODULES_DIR) -type d)
INCLUDE_FLAGS := $(addprefix -I, $(INCLUDE_DIRS) $(MODULES_DIR))

MAKEFLAGS += -j8

all: $(EXEC_FILES)

$(BUILD_DIR)/%.o: $(MODULES_DIR)/%.cpp $(LIBS)
	@mkdir -p $(@D)
	$(CXX) -c $(filter-out %.hpp, $<) -o $@ $(INCLUDE_FLAGS) $(FLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp $(LIBS)
	$(CXX) -c $(filter-out %.hpp, $<) -o $@ $(INCLUDE_FLAGS) $(FLAGS)

$(LSH): $(LSH_OBJ) $(LSH_OBJ_MODULES) $(COMMON_OBJ_MODULES)
	$(CXX) $^ -o $@ $(INCLUDE_FLAGS)

$(CUBE): $(CUBE_OBJ) $(CUBE_OBJ_MODULES) $(COMMON_OBJ_MODULES)
	$(CXX) $^ -o $@ $(INCLUDE_FLAGS)

$(GRAPH): $(GRAPH_OBJ) $(ALL_OBJ_MODULES)
	$(CXX) $^ -o $@ $(INCLUDE_FLAGS)

.PHONY: all clean lsh cube graph run-lsh run-cube run-graph valgrind-lsh valgrind-cube valgrind-graph \
 tests test-lsh test-cube test-graph lsh-test cube-test graph-test deb-lsh deb-cube deb-graph

clean:
	rm -rf $(BIN_DIR)/* $(BUILD_DIR)/*

lsh: $(LSH)

cube: $(CUBE)

graph: $(GRAPH)

ARGS_LSH := -d datasets/train-images.idx3-ubyte -q datasets/t10k-images.idx3-ubyte -k 4 -L 5 -o output_lsh.txt -N 5 -R 10000

ARGS_CUBE := -d datasets/train-images.idx3-ubyte -q datasets/t10k-images.idx3-ubyte -k 14 -M 6000 -probes 15 -o output_cube.txt -N 5 -R 10000

ARGS_GRAPH := -d datasets/train-images.idx3-ubyte -q datasets/t10k-images.idx3-ubyte -k 40 -E 30 -R 15 -N 3 -l 10 -m 2 -o output_graph.txt

run-lsh: lsh
	./$(LSH) $(ARGS_LSH)

run-cube: cube
	./$(CUBE) $(ARGS_CUBE)

run-graph: graph
	./$(GRAPH) $(ARGS_GRAPH)


# Valgrind targets

VALGRIND_ARGS := --leak-check=full --show-leak-kinds=all --track-origins=yes -s

valgrind-lsh: $(LSH)
	valgrind $(VALGRIND_ARGS) ./$(LSH) $(ARGS_LSH)

valgrind-cube: $(CUBE)
	valgrind $(VALGRIND_ARGS) ./$(CUBE) $(ARGS_CUBE)

valgrind-graph: $(GRAPH)
	valgrind $(VALGRIND_ARGS) ./$(GRAPH) $(ARGS_GRAPH)


# Test targets

TEST_DIR := tests

TEST_FILES := $(wildcard $(TEST_DIR)/*.cpp)

LSH_TEST := $(BIN_DIR)/lsh_test
CUBE_TEST := $(BIN_DIR)/cube_test
GRAPH_TEST := $(BIN_DIR)/graph_test

LSH_TEST_OBJ := $(BUILD_DIR)/lsh_test.o
CUBE_TEST_OBJ := $(BUILD_DIR)/cube_test.o
GRAPH_TEST_OBJ := $(BUILD_DIR)/graph_test.o

TEST_EXEC_FILES := $(TEST_FILES:$(TEST_DIR)/%.cpp=$(BIN_DIR)/%)

tests: $(TEST_EXEC_FILES)

$(BUILD_DIR)/%.o: $(TEST_DIR)/%.cpp $(LIBS)
	$(CXX) -c $(filter-out %.hpp, $<) -o $@ $(INCLUDE_FLAGS) $(FLAGS)

$(LSH_TEST): $(LSH_TEST_OBJ) $(LSH_OBJ_MODULES) $(COMMON_OBJ_MODULES)
	$(CXX) $^ -o $@ $(INCLUDE_FLAGS)

$(CUBE_TEST): $(CUBE_TEST_OBJ) $(CUBE_OBJ_MODULES) $(COMMON_OBJ_MODULES)
	$(CXX) $^ -o $@ $(INCLUDE_FLAGS)

$(GRAPH_TEST): $(GRAPH_TEST_OBJ) $(ALL_OBJ_MODULES)
	$(CXX) $^ -o $@ $(INCLUDE_FLAGS)

lsh-test: $(LSH_TEST)

cube-test: $(CUBE_TEST)

graph-test: $(GRAPH_TEST)

test-lsh: lsh-test
	./$(LSH_TEST) $(ARGS_LSH)

test-cube: cube-test
	./$(CUBE_TEST) $(ARGS_CUBE)

test-graph: graph-test
	./$(GRAPH_TEST) $(ARGS_GRAPH)


# Debug targets

$(BUILD_DIR)/%-deb.o: $(MODULES_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) -c $< -o $@ $(INCLUDE_FLAGS) $(DEBUG_FLAGS)

$(BUILD_DIR)/%-deb.o: $(SRC_DIR)/%.cpp
	$(CXX) -c $< -o $@ $(INCLUDE_FLAGS) $(DEBUG_FLAGS)

$(BIN_DIR)/%-deb: $(BUILD_DIR)/%-deb.o $(OBJ_MODULES_DEBUG)
	$(CXX) $^ -o $@ $(INCLUDE_FLAGS) $(DEBUG_FLAGS)

deb-lsh: $(LSH)-deb

deb-cube: $(CUBE)-deb

deb-graph: $(GRAPH)-deb
