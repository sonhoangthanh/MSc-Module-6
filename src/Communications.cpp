#include "Communications.h"


// Function which builds all 8 MPI Datatypes that we use from the base grid2D array and other parameters from Process
void build_process_boundary_types(double**& grid2D, int row_size, int col_size, MPI_Datatype& grid_top, 
MPI_Datatype& grid_bottom, MPI_Datatype& grid_left, MPI_Datatype& grid_right, MPI_Datatype& ghost_top, 
MPI_Datatype& ghost_bottom, MPI_Datatype& ghost_left, MPI_Datatype& ghost_right)
{   
    // Horizontal type e.g. top and bottom boundaries
    int block_length_x[col_size];
	MPI_Datatype typelist_x[col_size];
	MPI_Aint addresses_x[col_size], addresses_x_ghost[col_size];
    MPI_Aint displacement_x[col_size], displacement_x_ghost[col_size];

    // Vertical type e.g. left and fight boundaries
    int block_length_y[row_size];
	MPI_Datatype typelist_y[row_size];
	MPI_Aint addresses_y[row_size], addresses_y_ghost[row_size];
    MPI_Aint displacement_y[row_size], displacement_y_ghost[row_size];
	
    MPI_Aint add_start;

    MPI_Get_address(&(grid2D[0][0]), &add_start);

    // Top boundary

    for (int i = 0; i < col_size; ++i)
    {
        block_length_x[i] = 1;
        typelist_x[i] = MPI_DOUBLE;
        MPI_Get_address( &(grid2D[1][i+1]), &addresses_x[i]);
        displacement_x[i] = addresses_x[i] - add_start;

        // ghost top nodes
        MPI_Get_address(&(grid2D[0][i+1]), &addresses_x_ghost[i]);
        displacement_x_ghost[i] = addresses_x_ghost[i] - add_start;
    }
    
	MPI_Type_create_struct(col_size, block_length_x, displacement_x, typelist_x, &grid_top);
    MPI_Type_create_struct(col_size, block_length_x, displacement_x_ghost, typelist_x, &ghost_top);
	MPI_Type_commit(&grid_top);
	MPI_Type_commit(&ghost_top);
    
    // Bottom boundaries

    for (int i = 0; i < col_size; ++i)
    {
        block_length_x[i] = 1;
        typelist_x[i] = MPI_DOUBLE;
        MPI_Get_address( &(grid2D[row_size][i+1]), &addresses_x[i]);
        displacement_x[i] = addresses_x[i] - add_start;

        // ghost bottom nodes
        MPI_Get_address(&(grid2D[row_size+1][i+1]), &addresses_x_ghost[i]);
        displacement_x_ghost[i] = addresses_x_ghost[i] - add_start;
    }
    
	MPI_Type_create_struct(col_size, block_length_x, displacement_x, typelist_x, &grid_bottom);
    MPI_Type_create_struct(col_size, block_length_x, displacement_x_ghost, typelist_x, &ghost_bottom);
	MPI_Type_commit(&grid_bottom);
    MPI_Type_commit(&ghost_bottom);
    
    // Left boundary

    for (int i = 0; i < row_size; ++i)
    {
        block_length_y[i] = 1;
        typelist_y[i] = MPI_DOUBLE;
        MPI_Get_address( &(grid2D[i+1][1]), &addresses_y[i]);
        displacement_y[i] = addresses_y[i] - add_start;

        // ghost left nodes
        MPI_Get_address(&(grid2D[i+1][0]), &addresses_y_ghost[i]);
        displacement_y_ghost[i] = addresses_y_ghost[i] - add_start;
    }
    
	MPI_Type_create_struct(row_size, block_length_y, displacement_y, typelist_y, &grid_left);
    MPI_Type_create_struct(row_size, block_length_y, displacement_y_ghost, typelist_y, &ghost_left);
	MPI_Type_commit(&grid_left);
	MPI_Type_commit(&ghost_left);

    // Right boundary

    for (int i = 0; i < row_size; ++i)
    {
        block_length_y[i] = 1;
        typelist_y[i] = MPI_DOUBLE;
        MPI_Get_address( &(grid2D[i+1][col_size]), &addresses_y[i]);
        displacement_y[i] = addresses_y[i] - add_start;

        // ghost right nodes
        MPI_Get_address(&(grid2D[i+1][col_size+1]), &addresses_y_ghost[i]);
        displacement_y_ghost[i] = addresses_y_ghost[i] - add_start;
    }
    
	MPI_Type_create_struct(row_size, block_length_y, displacement_y, typelist_y, &grid_right);
	MPI_Type_create_struct(row_size, block_length_y, displacement_y_ghost, typelist_y, &ghost_right);
	MPI_Type_commit(&grid_right);
	MPI_Type_commit(&ghost_right);

}



// Function to send the boundaries in order of (top, bottom, left, right)
void send_boundaries(int& pid, double**& grid2D, int* neighbours, MPI_Datatype*& send_datatypes, MPI_Request*& send_requests)
{
	for (int i = 0; i < 4; ++i)
	{
		// If the neighbour to send is the process itself, just copy the data from top boundary to bottom boundary
		if (neighbours[i] == pid)
		{	
			// Send to itself
			MPI_Isend(&grid2D[0][0], 1, send_datatypes[i], neighbours[i], i, MPI_COMM_WORLD, &send_requests[i]);
		}
		else if (neighbours[i] == -1)
		{
			// Send to no process using MPI_PROC_NULL
			MPI_Isend(&grid2D[0][0], 1, send_datatypes[i], MPI_PROC_NULL, i, MPI_COMM_WORLD, &send_requests[i]);
		}
		else
		{
			// Send the boundaries to neighbours
			MPI_Isend(&grid2D[0][0], 1, send_datatypes[i], neighbours[i], i, MPI_COMM_WORLD, &send_requests[i]);
		}
	}
}

/* This function has reverse recv_datatype_array compared to send_datatype_array, that is they receive the ghost nodes that are
(ghost_bottom, ghost_top, ghost_right, ghost_left). This way is used to use simillar loops to sending function.
 */
void recv_boundaries(int& pid, double**& grid2D, int* neighbours, MPI_Datatype*& recv_datatypes, MPI_Request*& recv_requests)
{	
	// Manually schedule tags to get good boundaries into correct ghost nodes
	int recv_tags[4]{1, 0, 3, 2};

	for (int i = 0; i < 4; ++i)
	{
		// If the neighbour to send is the process itself, just copy the data from top boundary to bottom boundary
		if (neighbours[i] == pid)
		{	
			// Send to itself
			MPI_Irecv(&grid2D[0][0], 1, recv_datatypes[i], pid, recv_tags[i], MPI_COMM_WORLD, &recv_requests[i]);
		}
		else if (neighbours[i] == -1)
		{
			// Send to no process using MPI_PROC_NULL
			MPI_Irecv(&grid2D[0][0], 1, recv_datatypes[i], MPI_PROC_NULL, recv_tags[i], MPI_COMM_WORLD, &recv_requests[i]);
		}
		else
		{
			// Send the boundaries to neighbours
			MPI_Irecv(&grid2D[0][0], 1, recv_datatypes[i], neighbours[i], recv_tags[i], MPI_COMM_WORLD, &recv_requests[i]);
		}
	}
}

// Function which frees all the MPI Datytypes at once
void free_types(MPI_Datatype*& types_array)
{
    for (int i = 0; i < 4; ++i)
    {
        MPI_Type_free(&types_array[i]);
    }
}

	
