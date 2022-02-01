// Libraries
#include "mpi.h"
#include <iostream>
#include <math.h>
#include <string>
#include <unistd.h>
#include <fstream>

// User defined libraries
#include "util_functions.h"
#include "Process.h"
#include "Communications.h"
#include "Domain.h"

using namespace std;


int main(int argc, char* argv[])
{
    // Domain parameters
    string boundary = "fixed";
    string boundary_condition = "None";
    int grid_imax = 100;
    int grid_jmax = 100;

    // PDE and grid printing parameters
    double t_max = 5.0;
    double t = 0.0, t_out = 0.0, dt_out = 0.04, dt;
    double y_max = 10.0, x_max = 10.0, dx, dy;
    double c = 5;

    // Initial condition
    int initial_pid = 0;
    int initial_i_pos = 2;
    int initial_j_pos = 2;
    double initial_value = 9;

    // Calculate the discretization length and time using time constraints
    dx = x_max / ((double)grid_imax - 1);
	dy = y_max / ((double)grid_jmax - 1);

	dt = 0.1 * min(dx, dy) / c;

    // MPI variables
    int id, p;

    // Read in the configuration file
    read_config(boundary, boundary_condition, grid_imax, grid_jmax, t_max, t, t_out, dt_out, x_max, y_max, c, initial_pid, initial_i_pos, initial_j_pos, initial_value);

    // Create the command line parser interface and read in the parameters from CL
    int a = -1;
    while ((a = getopt(argc, argv, "hb:c:i:j:")) != -1) 
    {
        switch(a)
        {
            case 'b':  // output file
                boundary = optarg;
                break;
            case 'c': 
                boundary_condition = optarg;
                break;
            case 'i':
                grid_imax = atoi(optarg);
                break;
            case 'j':
                grid_jmax = atoi(optarg);
                break;
        }
    }

    // Sanity checks for boundary conditions
    if (boundary == "periodic" && (boundary_condition != "" && boundary_condition != "None"))
    {
        cout << "Invalid boundary choice! Can't enforce Dirichlet or Neumman boundary conditions on the periodic grid! \n";
        cout << "Invalidating the boundary choice to 'None'! \n";
    }

    // Initialize the MPI program
    MPI_Init(&argc, &argv);

    // Initialise the MPI information
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
	MPI_Comm_size( MPI_COMM_WORLD , &p);

    // Initialise the computational domain object
    Domain domain(p, grid_imax, grid_jmax, boundary, boundary_condition); 

    // Set up the process object
    Process process(id, domain);

    // Declare the 1D and 2D grids
    double* old_grid1D; double** old_grid2D;
    double* grid1D; double** grid2D;
    double* new_grid1D; double** new_grid2D;
    
    // Allocate the grids
    allocate_grids(old_grid1D, old_grid2D, process.grid_row_size, process.grid_col_size);
    allocate_grids(grid1D, grid2D, process.grid_row_size, process.grid_col_size);
    allocate_grids(new_grid1D, new_grid2D, process.grid_row_size, process.grid_col_size);


    // Insert initial condition
    process.read_initial_condition(initial_pid, initial_i_pos, initial_j_pos, initial_value, old_grid2D, grid2D);

    // counters
    int out_cnt = 0, it = 0;

    // Print initial condition to file
    process.grid_to_file(out_cnt, grid2D);

    // After printing to file, increment the counters
	out_cnt++;
	t_out += dt_out;


    // Declare the 4 grid boundary types and 4 ghost boundary types
    MPI_Datatype grid_top, grid_bottom, grid_left, grid_right;
    MPI_Datatype ghost_top, ghost_bottom, ghost_left, ghost_right;

    // Build the MPI_Datypes for all the 8 boundary types
    build_process_boundary_types(grid2D, process.grid_row_size, process.grid_col_size, grid_top, grid_bottom, grid_left, grid_right,
    ghost_top, ghost_bottom, ghost_left, ghost_right);
    
    // Declare the request array, send_datatype array and recv_datype array.
    MPI_Request* send_requests = new MPI_Request[4]; 
    MPI_Request* recv_requests = new MPI_Request[4];

    MPI_Datatype* send_datatypes = new MPI_Datatype[4]{grid_top, grid_bottom, grid_left, grid_right};
    MPI_Datatype* recv_datatypes = new MPI_Datatype[4]{ghost_top, ghost_bottom, ghost_left, ghost_right}; 

    // Loop and perform: sending neighbours, doing PDE iteration and if applicable, print to file
    while (t < t_max)
	{   
        // Set up non-blocking sends and receives of the applicable boundaries 
        recv_boundaries(id, grid2D, process.neighbours, recv_datatypes, recv_requests);
        send_boundaries(id, grid2D, process.neighbours, send_datatypes, send_requests);

        // Do the iteration on the inner domain without the boundaries
        process.iterate_inner_domain(old_grid2D, grid2D, new_grid2D, c, t, dt, dx, dy);
        
        // Wait until all the receives are finished
        MPI_Waitall(4, recv_requests, MPI_STATUSES_IGNORE);

        // This second Waitall was put here even if without it we could improve timings by few percent!. 
        // That is due to fear of starting iterations and swapping pointers before send can be done for large systems. After more testing which proves otherwise, can be removed
        MPI_Waitall(4, send_requests, MPI_STATUSES_IGNORE);
        

        // Set boundary if Neumann, leave untouched if Dirichlet
        if (boundary_condition == "Neumann")
        process.set_neumann_boundaries(new_grid2D);
        
        // Finish PDE iteration on the boundaries after receiving the ghost boundaries
        process.iterate_edges(old_grid2D, grid2D, new_grid2D, c, t, dt, dx, dy);

        // Print the grid2D
        if (t_out <= t)
		{
			// cout << "output: " << out_cnt << "\t t: " << t << "\t iteration: " << it << endl;
			process.grid_to_file(out_cnt, grid2D);
			out_cnt++;
			t_out += dt_out;
		}

        // Swap pointers
        swap_grid_pointers(old_grid2D, new_grid2D);
        swap_grid_pointers(old_grid2D, grid2D);

        t += dt;
		it++;
	}

    // Output the config of the program to mpi_config.txt in output/ directory
    output_config(out_cnt, domain.topology_rows, domain.topology_cols, dt_out);

    // Deallocate the grids
    delete_grids(old_grid1D, old_grid2D);
    delete_grids(grid1D, grid2D);
    delete_grids(new_grid1D, new_grid2D);

    // Free the types
    MPI_Type_free(send_datatypes);
    MPI_Type_free(recv_datatypes);

    // Delete the datatype and requests dynamic arrays
    delete[] send_datatypes;
    delete[] recv_datatypes;
    delete[] send_requests;
    delete[] recv_requests;

    MPI_Finalize();

    return 0;
}