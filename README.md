# MPI Wave Equation Domain Decomposion
This repository is for the individual coursework for the ACSE-6 module. The repository holds the Message Passing Interface (MPI) version of the wave-equation solver program on the 2D computational grid using domain decomposition method.

# Dependencies Installation
The program was made in mind with building and deployment on Linux OS / HPC platforms. Therefore the default building system employs the use of *make* and a Makefile which is contained within this directory. The program requires the use of OpenMPI implementation of MPI, however the Makefile can be altered to include other implementations and compilers.

To install OpenMPI on Linux platform:

Ubuntu/Debian : sudo apt install libopenmpi-dev

Centos/Redhat: sudo yum  install openmpi-devel

# Building
As mentioned above, the program uses *make* and Makefiles for code compilation. Within the makefile, several building options are provided:

- `make` or `make all`: Compiles the default MPI implementation of the wave equation program.

- `make release`: Compilies the standard optimized (with -O3 flag) MPI implementation of the wave equation program.

- `make serial`: Compiles the serial (single-core) implementation of the wave equation program.

- `make serial_release`: Compiles the standard optimized (with -O3 flag) serial implementation of the wave equation program.

In addition to compiling the code, make also creates the build (or serial_build) and output (or output_serial) directories, which will hold the compiled program executable and will hold the output files after running such program.

The user can clean the contents of the build and output directories using either *make clean* , *make clean_mpi* or *make clean_serial*.

# Executing the program
After compilation, user can execute the MPI or serial implementations within the build directories. To run the program, type:

(For MPI) `mpiexec -n [number of process] ./build/MPI_Wave_Equation [options]`

(For serial code) `./serial_build/Serial_Wave_Equation`

For release builds, the analogous methos is used with MPI_Wave_Equation_Optimized and Serial_Wave_Equation_Optimized executables.

After execution, the file output will be generated either in the output or output_serial directories. Also the configuration file containing the program parameters will be created, which can be used to combine seperate decomposition process files using post-processing scripts. 

# Execution Options
The program parameters for solving the wave equation can both be specified using the configuration file and the command-line flag arguments.

## Configuration File
All program parameters, including the domain information and wave-equation, physics parameters can be changed and edited within the provided config.txt file. The program will parse the file at the beginning to obtain the paramters. Please don't change the layout and the position of the entries, just change calye after the '=' sign. For the example of correctly formatted config.txt file, see the attached image.

### Config.txt Options

**Domain information**
-boundary: fixed or periodic

-boundary_condition: Dirichlet or Neumann

-grid_imax: size of domain in rows of grid/matrix.

-grid_jmax: size of domain in columns of grid/matrix.

**Physical Parameters**
-t_max: the end time of the program in seconds

-t: initial time of the program in seconds

-t_out: output time of the program in seconds

-dt_out: time step for with to output the domain to grid files

-x_max: physical size of the domain in metres [row length]

-y_max: physical size of the domain in metres [column length]

-c: speed of wave propagation in metres per second

**Initial Condition Parameters**
-pid: The process id where initial position is to be placed

-i_pos: The internal process grid i (row) position for initial condition

-j_pos: The internal process grid j (column) position for initial condition

-value: The value of the source displacement

## CL Usage
- **MPI_Wave_Equation** [options]

### Options
- **-i** : Set the imax (row) size of the computational grid matrix/domain. Default = 100
- **-j** : Set the jmax (column) size of the computational grid matrix/domain. Default  100
- **-b** : Set the type of boundary on the computational domain i.e. fixed or periodic boundaries. Default = fixed
- **-c** : Set the type of boundary condition if the fixed edges where set for the computational domain i.e. Dirichlet or Neumann Default = Neumann

# Post-processing and Visualization
The repository contains the post-processing scripts written in Python located in *post-processing* directory. The script contain two function and a routine which can be combined to assemble the data from multiple processor grid data (which can be then plotted if desired but this have to be later included in the source code and requires firther development) and by default assembles the data and generates an animation (animate.gif) for visualization of the solutions in the current directory. Note however that the animation can take increasingly long time for larger grid dimensions and number of iterations.

To execute the scripts type from within the base repository path: `python3 ./post-processing/postprocess_mpi.py`

The script reads in the mpi_config.txt file generated by the main program which contains the information needed for the post processing such as the topology distribution of processes over the computational domain as well as the number of output files.
# Debugging and Profiling

The program can be compiled with debug symbols and passed into the profiling tool like callgrind. To compile the MPI program with debug symbols, type:

- `make debug`

To profile the mpi code using the valgrind-callgrind option, type:

- `valgrind --tool=callgrind ./build/MPI_Wave_Equation_debug`

An example callgrind output file created with the base config file in the repository can be found in the valgrid.

# Job Submission File and Testing on HPC Distributed System
An example bash job-submission script for DUG HPC scheduler rjs is contained in the main repository under the name coursework.job . Different variations of this script has been tested and used for the collection of code timings, with the loop over number of processes or the desired sizes of the domain. The output of these test on DUG can be seen in the */logs* directory. The timings were taken with command line function `time mpiexec [...]` which returns the wall clock time of binary execution, and user / system time taken.

# Further Documentation
For further documentation and code explaination please refer to the *.pdf* report and to the comments within the header and source code.
