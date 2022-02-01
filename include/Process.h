#pragma once
#include "util_functions.h"
#include "Domain.h"
#include <string>
#include "mpi.h"



/* 
Header which includes the process specific class which will contain all the information used by the process.
*/

class Process
{
public:

    // process information 
    int pid;
    int grid_row_size, grid_col_size;
    
    int topology_row_position, topology_col_position;

    // Strings which tell which type of boundary and boundary condition to impost
    std::string boundary;
    std::string boundary_condition;

    // neighbours containing array which will be for indexing: 0 - top, 1 - bottom, 2 - left, 3 - right
    int neighbours[4]; 
    int num_neighbours = 0;
    int neighbour_distribution[2];  // array which tells what the distribution of the neighbours is based on how many horizontal and vertical neightbors there are

    // Constructor with initialization of member variables
    Process(int pid, Domain& domain);

    // Destructor
    ~Process();

    // Member functions

    int get_dimension_size (int process_topology_index, int domain_size, int topology_dimension_size);

    void read_initial_condition(int& pid_initial, int& initial_i_pos, int& initial_j_pos, double& initial_value, double**& old_grid2D, double**& grid2D);

    void iterate_inner_domain(double**& old_grid2D, double**& grid2D, double**& new_grid2D, double& c, double& t,
    double& dt, double& dx, double& dy);

    void iterate_edges(double**& old_grid2D, double**& grid2D, double**& new_grid2D, double& c, double& t,
    double& dt, double& dx, double& dy);

    void set_neumann_boundaries(double**& new_grid2D);

    void grid_to_file(int out, double** grid2D);

};