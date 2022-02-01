#pragma once
#include "mpi.h"

// This header contains the functions to setup the boundary types for the communication between the processes

void build_process_boundary_types(double**& grid2D, int row_size, int col_size, MPI_Datatype& MPI_top, 
MPI_Datatype& MPI_bottom, MPI_Datatype& MPI_left, MPI_Datatype& MPI_right, MPI_Datatype& ghost_top, 
MPI_Datatype& ghost_bottom, MPI_Datatype& ghost_left, MPI_Datatype& ghost_right);

void send_boundaries(int& pid, double**& grid2D, int* neighbours, MPI_Datatype*& send_datatypes, MPI_Request*& send_requests);

void recv_boundaries(int& pid, double**& grid2D, int* neighbours, MPI_Datatype*& recv_datatypes, MPI_Request*& recv_requests);

void free_types(MPI_Datatype*& types_array);