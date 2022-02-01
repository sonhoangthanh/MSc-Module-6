#pragma once
#include <string>

/* 
This header contains all the function which are not associated with each individual processes (such as finding the topology of the processes)
and are not explicitly used by processes in their pure form but act more as a support provided to create the member function of the process class 
*/
// MPI_Datatype top, bottom, left, right;
// Process process;

void read_config(std::string& boundary, std::string& boundary_condition, int& grid_imax, int& grid_jmax, double& tmax,
double& t, double& t_out, double& dt_out, double& x_max, double& y_max, double& c, int& initial_pid, int& initial_i_pos, int& initial_j_pos, double& initial_value);

void allocate_grids(double*& grid1D, double**& grid2D, int row_size, int col_size);

void delete_grids(double*& grid1D, double**& grid2D);

void swap_grid_pointers(double**& grid1, double**& grid2);

void output_config(int& iterations, int& topology_rows, int& topology_cols, double& dt);







