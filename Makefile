# the c++ compiler wrapper used in OpenMPI
COMPILER = mpic++
CXX = g++

# Paths and libraries
INCLUDE_DIR = include
SOURCE_DIR = src
BUILD_DIR = build
 
# compiler flags:
#  -g     - this flag adds debugging information to the executable file
#  -Wall  - this flag is used to turn on most compiler warnings
CPPFLAGS  = -Wall -I $(INCLUDE_DIR)

# The build target 
TARGET = MPI_Wave_Equation

  
.PHONY: all clean
all: $(TARGET)

# Check if the build directory exists and makes one if not. Builds the target program within the build directory.
$(TARGET): $(SOURCE_DIR)/$(TARGET).cpp $(SOURCE_DIR)/util_functions.cpp $(SOURCE_DIR)/Process.cpp $(SOURCE_DIR)/Communications.cpp $(SOURCE_DIR)/Domain.cpp
	@if [ ! -d "./$(BUILD_DIR)" ]; then mkdir ./build; echo "Created build folder!"; fi
	@if [ ! -d "./output" ]; then mkdir ./output; echo "Created output folder"; fi
	
	@ $(COMPILER) $(CPPFLAGS) -o $(BUILD_DIR)/$@ $^
	@echo "Built Executable!"

release: MPI_Wave_Equation_Optimized
# Check if the build directory exists and makes one if not. Builds the target program within the build directory.
MPI_Wave_Equation_Optimized: $(SOURCE_DIR)/$(TARGET).cpp $(SOURCE_DIR)/util_functions.cpp $(SOURCE_DIR)/Process.cpp $(SOURCE_DIR)/Communications.cpp $(SOURCE_DIR)/Domain.cpp
	@if [ ! -d "./$(BUILD_DIR)" ]; then mkdir ./build; echo "Created build folder!"; fi
	@if [ ! -d "./output" ]; then mkdir ./output; echo "Created output folder"; fi
	
	@ $(COMPILER) $(CPPFLAGS) -O3 -o $(BUILD_DIR)/$@ $^
	@echo "Built Optimized Executable!"

debug: MPI_Wave_Equation_debug
MPI_Wave_Equation_debug: $(SOURCE_DIR)/$(TARGET).cpp $(SOURCE_DIR)/util_functions.cpp $(SOURCE_DIR)/Process.cpp $(SOURCE_DIR)/Communications.cpp $(SOURCE_DIR)/Domain.cpp
	@if [ ! -d "./$(BUILD_DIR)" ]; then mkdir ./build; echo "Created build folder!"; fi
	@if [ ! -d "./output" ]; then mkdir ./output; echo "Created output folder"; fi
	
	@ $(COMPILER) $(CPPFLAGS) -g -o $(BUILD_DIR)/$@ $^
	@echo "Built Debug Executable!"

serial: Serial_Wave_Equation
Serial_Wave_Equation: ./$(SOURCE_DIR)/Serial_Wave_Equation.cpp
	@if [ ! -d "./serial_build" ]; then mkdir ./serial_build; echo "Created serial build folder!"; fi
	@if [ ! -d "./output_serial" ]; then mkdir ./output_serial; echo "Created output folder"; fi

	@$(CXX) -o ./serial_build/$@ $^
	@echo "Built Serial Executable!"

serial_release: Serial_Wave_Equation_Optimized
Serial_Wave_Equation_Optimized: ./$(SOURCE_DIR)/Serial_Wave_Equation.cpp
	@if [ ! -d "./serial_build" ]; then mkdir ./serial_build; echo "Created serial build folder!"; fi
	@if [ ! -d "./output_serial" ]; then mkdir ./output_serial; echo "Created output folder"; fi

	@$(CXX) -O3 -o ./serial_build/$@ $^
	@echo "Built Optimized Serial Executable!"

clean:
	@if [ -d "./$(BUILD_DIR)" ] && [ -d "./output" ]; then $(RM) -rf $(BUILD_DIR) output; fi
	@if [ -d "./serial_build" ] && [ -d "./output_serial" ]; then $(RM) -rf serial_build output_serial; fi
	@echo "Cleaned!";

clean_serial:
	@if [ -d "./serial_build" ] && [ -d "./output_serial" ]; then $(RM) -rf serial_build output_serial; fi
	@echo "Cleaned!";

clean_mpi:
	@if [ -d "./$(BUILD_DIR)" ] && [ -d "./output" ]; then $(RM) -rf $(BUILD_DIR) output; fi
	@echo "Cleaned!";
	
