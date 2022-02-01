#include "Process.h"
#include "Communications.h"

#include <cmath>
#include <iostream>
#include <sstream>
#include <fstream>


// Initilization constructor
// This will populate all the class variables at initialisation, initialise needed variables using initialization list
Process::Process(int pid, Domain& domain):
pid(pid)
{   
    // Copy the boundary information from domain object
    this->boundary = domain.boundary;
    this->boundary = domain.boundary_condition;


    // Get the topology position of the process
    domain.id_to_index(this->pid, this->topology_row_position, this->topology_col_position);  // error on this line

    // Get process grid sizes
    this->grid_row_size = this->get_dimension_size(this->topology_row_position, domain.domain_size_rows, domain.topology_rows);
    this->grid_col_size = this->get_dimension_size(this->topology_col_position, domain.domain_size_cols, domain.topology_cols);
    
    // The routine bellow could be encapsulated using functions, but as they are just called once, we can hard code them here.
    // Loop to get all 4 neighbours, we only need 4 so we can do this by hand for each 
    
    // Top neighbour
    this->neighbours[0] =  domain.index_to_id(domain.boundary, domain.topology_rows, domain.topology_cols, this->topology_row_position-1, topology_col_position);

    // Bottom neighbour
    this->neighbours[1] =  domain.index_to_id(domain.boundary, domain.topology_rows, domain.topology_cols, this->topology_row_position+1, topology_col_position);

    // Left neighbour
    this->neighbours[2] =  domain.index_to_id(domain.boundary, domain.topology_rows, domain.topology_cols, this->topology_row_position, topology_col_position-1);

    // Right neighbour
    this->neighbours[3] =  domain.index_to_id(domain.boundary, domain.topology_rows, domain.topology_cols, this->topology_row_position, topology_col_position+1);

    // Get the number of neighbours, and consequenly the indices of the fixed boundaries
    // Horizontal neighbours
    for (int i = 0; i < 2; ++i)
    {   
        if (neighbours[i] != -1)
            this->num_neighbours += 1;
    }
    // Vertical neighbours
    for (int i = 2; i < 4; ++i)
    {
        if (neighbours[i] != -1)
            this->num_neighbours += 1;
    }
 
}

// // Default destructor
Process::~Process()
{
    
}

// Function which returns the integer size along a topology_dimension (like row or column) of the process by distributing according to the id the process 
int Process::get_dimension_size (int process_topology_index, int domain_size, int topology_dimension_size)
{	

	int residue = domain_size % (topology_dimension_size);  // if this is zero then we have perfect division, if not we have to allocate some more work to some processes
    int dist_number = (domain_size - residue) / (topology_dimension_size);  // calculate the base size of the process without the residue

	/* Now we need to distribute the size to the processes. If the residue is non zero, lets say residue = 3, then we have to distribute
	the size for 3 processes lets say first 3 processes (indices from 0-topology_dimension_size) get dist_number + 1  */
	if (process_topology_index < residue)
		return dist_number+1;
	else
		return dist_number;
}

// Function to calculate if the initial condition with global indices match and what the internal position would that be if true 
void Process::read_initial_condition(int& pid_initial, int& initial_i_pos, int& initial_j_pos, double& initial_value, double**& old_grid2D, double**& grid2D)
{
    if (pid_initial == this->pid)
    {   
        // Set the initial position
        old_grid2D[initial_i_pos][initial_j_pos] = initial_value;
        grid2D[initial_i_pos][initial_j_pos] = initial_value;
    }
}

// The function which iterates over the inner grid of the process, that is the portion of the grid without its edge layers.
void Process::iterate_inner_domain(double**& old_grid2D, double**& grid2D, double**& new_grid2D, double& c, double& t,
double& dt, double& dx, double& dy)
{   
    // While the sending and receiving is carried out, do the work on the inner domain without entries at the boundaries 
	for (int i = 2; i <= this->grid_row_size-1; ++i)
    {
        for (int j = 2; j <= this->grid_col_size-1; ++j)
        {   
            // This is the PDE stencil
            new_grid2D[i][j] = pow(dt * c, 2.0) * ((grid2D[i + 1][j] - 2.0 * grid2D[i][j] + grid2D[i - 1][j]) / pow(dx, 2.0)
            + (grid2D[i][j + 1] - 2.0 * grid2D[i][j] + grid2D[i][j - 1]) / pow(dy, 2.0)) + 2.0 * grid2D[i][j] - old_grid2D[i][j];
        }
    } 
}


// This function iterates over all the edges of the domain and applies the PDE WE stencil on the boundary entries
// The functions will check if the edge is fixed or not using neighbours[i] array entries from Process
void Process::iterate_edges(double**& old_grid2D, double**& grid2D, double**& new_grid2D, double& c, double& t,
double& dt, double& dx, double& dy)
{   
    // This function is very ugly but couldnt find the way to do it more efficiently without more overhead cost
    // Check using neighbours array if the neighbour is valid 
    
    // Top edge
    if (this->neighbours[0] != -1)
    {
        for (int j = 1; j <= this->grid_col_size; ++j)
        {   
            new_grid2D[1][j] = pow(dt * c, 2.0) * ((grid2D[2][j] - 2.0 * grid2D[1][j] + grid2D[0][j]) / pow(dx, 2.0)
            + (grid2D[1][j + 1] - 2.0 * grid2D[1][j] + grid2D[1][j - 1]) / pow(dy, 2.0)) + 2.0 * grid2D[1][j] - old_grid2D[1][j];
        }

    }

    // Bottom edge
    if (this->neighbours[1] != -1)
    {
        for (int j = 1; j <= this->grid_col_size; ++j)
        {   
            new_grid2D[this->grid_row_size][j] = pow(dt * c, 2.0) * ((grid2D[this->grid_row_size+1][j] - 2.0 * grid2D[this->grid_row_size][j]
            + grid2D[this->grid_row_size-1][j]) / pow(dx, 2.0) + (grid2D[this->grid_row_size][j + 1] - 2.0 * grid2D[this->grid_row_size][j]
            + grid2D[this->grid_row_size][j - 1]) / pow(dy, 2.0)) + 2.0 * grid2D[this->grid_row_size][j] - old_grid2D[this->grid_row_size][j];
        }

    }

    // Left edge
    if (this->neighbours[2] != -1)
    {
        for (int i = 1; i <= this->grid_row_size; ++i)
        {   
            new_grid2D[i][1] = pow(dt * c, 2.0) * ((grid2D[i + 1][1] - 2.0 * grid2D[i][1] + grid2D[i - 1][1]) / pow(dx, 2.0)
            + (grid2D[i][2] - 2.0 * grid2D[i][1] + grid2D[i][0]) / pow(dy, 2.0)) + 2.0 * grid2D[i][1] - old_grid2D[i][1];
        }
    }

    // Right edge
    if (this->neighbours[3] != -1)
    {
        for (int i = 1; i <= this->grid_row_size; ++i)
        {   
            new_grid2D[i][this->grid_col_size] = pow(dt * c, 2.0) * ((grid2D[i + 1][this->grid_col_size] - 2.0 * grid2D[i][this->grid_col_size]
            + grid2D[i - 1][this->grid_col_size]) / pow(dx, 2.0) + (grid2D[i][this->grid_col_size+1] - 2.0 * grid2D[i][this->grid_col_size]
            + grid2D[i][this->grid_col_size-1]) / pow(dy, 2.0)) + 2.0 * grid2D[i][this->grid_col_size] - old_grid2D[i][this->grid_col_size];
        }
    }
}

// Function to enforce Neumann boundary condition on the fixed edges
void Process::set_neumann_boundaries(double**& new_grid2D)
{   
    // Do each boundary one by one
    if (this->neighbours[0] == -1)
    {
        // Set the top boundary
        for (int j = 1; j <= this->grid_col_size; ++j)
        {
            new_grid2D[1][j] = new_grid2D[2][j];
        }
    }

    if (this->neighbours[1] == -1)
    {
        // Set the bottom boundary
        for (int j = 1; j <= this->grid_col_size; ++j)
        {
            new_grid2D[this->grid_row_size][j] = new_grid2D[this->grid_row_size-1][j];
        }
    }

    if (this->neighbours[2] == -1)
    {
        // Set the left boundary
        for (int i = 1; i <= this->grid_row_size; ++i)
        {
            new_grid2D[i][1] = new_grid2D[i][2];
        }
    }

    if (this->neighbours[3] == -1)
    {
        // Set the left boundary
        for (int i = 1; i <= this->grid_row_size; ++i)
        {
 		    new_grid2D[i][this->grid_col_size] = new_grid2D[i][this->grid_col_size-1];
        }
    }
}


// Function to print the process grid to file 
void Process::grid_to_file(int out, double** grid2D)
{   
    // Declare the filename and fstream object
	std::stringstream filename;
	std::fstream f1;

    // Arrange the file name based on the program and process parameters
	filename << "./output/output" << "_it" << out <<"_p"<< this->pid << ".dat";
	f1.open(filename.str().c_str(), std::ios_base::out);

    // Loop over and write to file
	for (int i = 1; i <= this->grid_row_size; ++i)
	{   
		for (int j = 1; j <= this->grid_col_size; ++j)
        {   
            f1  << grid2D[i][j] << "\t" ;
        }
			
		f1 << std::endl;
	}
	f1.close();
}
