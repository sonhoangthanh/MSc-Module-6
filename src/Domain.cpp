#include <cmath>
#include <iostream>

#include "Domain.h"

// Default constructor of computational domain class which initializes the member variables
Domain::Domain(int num_processes, int domain_size_rows, int domain_size_cols, std::string boundary, std::string boundary_condition): num_processes(num_processes),
domain_size_rows(domain_size_rows), domain_size_cols(domain_size_cols), boundary(boundary), boundary_condition(boundary_condition)
{	
	// Find the optimal dimensions
	int min_gap = this->num_processes;
	int top = sqrt(this->num_processes) + 1;
	for (int i = 1; i <= top; i++)
	{
		if (this->num_processes % i == 0)
		{
			int gap = abs(this->num_processes / i - i);

			if (gap < min_gap)
			{
				min_gap = gap;

				// write to member variables the most optimal rows x columns configuration
				this->topology_rows = i;
				this->topology_cols = this->num_processes / i;
			}
		}
	}
}

// Destructor
Domain::~Domain()
{

}

// Function which translates the process id to the topological grid index positions.
void Domain::id_to_index(int pid, int &topology_row_position, int &topology_col_position)
{
	topology_col_position = pid % this->topology_cols;
	// this is a casting the float into int trick, where if the result of the operation in float
	// then this will round to the lower nearest integer
	topology_row_position =  pid / this->topology_cols;
}

// Function which translates the topological grid index position on the computational domain into the process id.
int Domain::index_to_id(std::string boundary, int& topology_rows, int& topology_cols, int topology_row_position, int topology_col_position)
{	
	if (boundary == "fixed" || boundary == "Fixed")
	{
		// The processes which exceeed the topology boundaries are detected by return -1
		if (topology_row_position >= topology_rows || topology_row_position<0)
			return -1;
		if (topology_col_position >= topology_cols || topology_col_position<0)
			return -1;

		return topology_row_position*topology_cols + topology_col_position;
	}
	else if (boundary == "periodic" || boundary == "Periodic")
	{
		// 
		topology_row_position = (topology_row_position+topology_rows) % topology_rows;
		topology_col_position = (topology_col_position+topology_cols) % topology_cols;

		return topology_row_position*topology_cols + topology_col_position;
	}
	else
	{
		std::cout << "Invalid boundary! \n";
		exit(1);
	}
}