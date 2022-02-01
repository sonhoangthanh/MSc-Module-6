#pragma once
#include <string>


/* Class which contains all the variables and member functions which will deal with the computational domain in the MPI implementation.
*/
class Domain
{   
    public:
        
        // Domain information
        int num_processes;
        int domain_size_rows, domain_size_cols;  // the actual size of domain in number of rows and columns
        int topology_rows, topology_cols;  // the topological grid distribution of processes such as 2 x 2 for 4 processes etc.

        // Boundary information
        std::string boundary;  // Fixed or Periodic
        std::string boundary_condition;  // Dirichlet or Neumannn
        
        // Default constructor
        Domain(int num_processes, int domain_size_rows, int domain_size_cols, std::string boundary, std::string boundary_condition);

        // Default destructor
        ~Domain();

        // Member functions

        void id_to_index( int pid, int &id_row, int &id_column);

        int index_to_id(std::string boundary, int& topology_rows, int& topology_columns, int topology_row_position, int topology_col_position);
        

    
};