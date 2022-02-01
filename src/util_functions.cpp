#include "util_functions.h"
#include "Process.h"

#include <iostream>
#include <math.h>
#include <fstream>
#include <sstream>
#include <string>
#include<vector>


/* 
This is the file containing all the supporting functions for the mpi wave equation implementation which are not associated with the individual process
*/

// Function to read in the configuration file name config.txt
// Very crude and inelegant, can be made better!
void read_config(std::string& boundary, std::string& boundary_condition, int& grid_imax, int& grid_jmax, double& tmax,
double& t, double& t_out, double& dt_out, double& x_max, double& y_max, double& c, int& initial_pid, int& initial_i_pos, int& initial_j_pos, double& initial_value)
{   
    
    //Reads in the CSV style file with format [matrix_values, b_values]
    // Arrays of different type to read in the parameters
    std::vector<std::string> string_parameters;
    std::vector<int> int_parameters;
    std::vector<double> double_parameters;
    std::vector<int> initial_parameters;
    std::vector<double> initial_value_param;
    

    std::ifstream input("./config.txt", std::fstream::in);
    if (!input.is_open()) 
    {
        std::cerr << "Unable to open config.txt file for reading!" << std::endl;
        std::cerr << "The file might not be there, or may not exist. Skipping!!" << std::endl;
        return;
    }
    
    std::string line;  // string to store out data segment delimitted with "="

    getline(input, line); // skip first line header
    getline(input, line); // skip second line
    getline(input, line); // Ccursor on the third line

    // Read in the string parameters
    for (int i = 0; i < 2; ++i)
    {
        std::stringstream mystream(line);  // get stream of string and store it in variable line
        
        bool flag = false;  // need this flag to read second substring after =

        while (mystream.good())
        {  
            std::string substr;
            getline(mystream, substr, '=');  // get substring delimitted by =

            // If its the second line, push back the vector
            if (flag)
            {
                string_parameters.push_back(substr.c_str());
            }
 
            flag = !flag;  // change the flag to negation to get every other value
        }

        getline(input, line);  // skip to next line and repeat
    }

    // Set string variables
    boundary = string_parameters[0]; boundary_condition = string_parameters[1];

    // Read in the integer parameters
    for (int i = 0; i < 2; ++i)
    {
        std::stringstream mystream(line);  // get stream of string and store it in variable line
        
        bool flag = false;  // need this flag to read second substring after =

        while (mystream.good())
        {  
            std::string substr;
            getline(mystream, substr, '=');  // get substring delimitted by =

            // If its the second line, push back the vector and convert to int
            if (flag)
            {
                int_parameters.push_back(atoi(substr.c_str()));
            }
 
            flag = !flag;  // change the flag to negation to get other value
        }

        getline(input, line);  // skip to next line and repeat
    }

    // Set integer variables
    grid_imax = int_parameters[0]; grid_jmax = int_parameters[1];

    // Skip another line
    getline(input, line);

    // Read in the rest float parameters
    for (int i = 0; i < 7; ++i)
    {
        std::stringstream mystream(line);  // get stream of string and store it in variable line
        
        bool flag = false;  // need this flag to read second substring after =

        while (mystream.good())
        {  
            std::string substr;
            getline(mystream, substr, '=');  // get substring delimitted by =

            // If its the second line, push back the vector and convert to float
            if (flag)
            {
                double_parameters.push_back(atof(substr.c_str()));
            }
 
            flag = !flag;  // change the flag to negation to get other value
        }

        getline(input, line);  // skip to next line and repeat
    }

    // Skip another line to initial conditions
    getline(input, line);

    // Read in the initial condition integer parameters
    for (int i = 0; i < 3; ++i)
    {
        std::stringstream mystream(line);  // get stream of string and store it in variable line
        
        bool flag = false;  // need this flag to read second substring after =

        while (mystream.good())
        {  
            std::string substr;
            getline(mystream, substr, '=');  // get substring delimitted by =

            // If its the second line, push back the vector and convert to float
            if (flag)
            {
                initial_parameters.push_back(atoi(substr.c_str()));
            }
 
            flag = !flag;  // change the flag to negation to get other value
        }

        getline(input, line);  // skip to next line and repeat
    }

    // Read in the remaining initial value parameter
    for (int i = 0; i < 1; ++i)
    {
        std::stringstream mystream(line);  // get stream of string and store it in variable line
        
        bool flag = false;  // need this flag to read second substring after =

        while (mystream.good())
        {  
            std::string substr;
            getline(mystream, substr, '=');  // get substring delimitted by =

            // If its the second line, push back the vector and convert to float
            if (flag)
            {
                initial_value_param.push_back(atof(substr.c_str()));
            }
 
            flag = !flag;  // change the flag to negation to get other value
        }

        getline(input, line);  // skip to next line and repeat
    }

    // Set the float parameters to variables
    tmax = double_parameters[0];
    t = double_parameters[1];
    t_out = double_parameters[2];
    dt_out = double_parameters[3];
    x_max = double_parameters[4];
    y_max = double_parameters[5];
    c = double_parameters[6];
    initial_pid = initial_parameters[0];
    initial_i_pos = initial_parameters[1];
    initial_j_pos = initial_parameters[2];
    initial_value = initial_value_param[0];

    // Close the file
    input.close();
}

// Function which takes pointers and allocats 1D and 2D matrices. 2D matrix is just a 1D matrix which can be refered to like 2D matrix
void allocate_grids(double*& grid1D, double**& grid2D, int row_size, int col_size)
{
	grid1D = new double[(row_size+2)*(col_size+2)]{0};
    grid2D = new double*[row_size+2];  // 2D array of 1D array

	// Initialise the grid2D as a pointer array to grid1D
    for (int i = 0; i < row_size+2; i++)
    {
        grid2D[i] = &grid1D[i*(col_size+2)];
    }
}

// Function which deletes the specified 1d and 2d grids
void delete_grids(double*& grid1D, double**& grid2D)
{
	delete[] grid2D;
	delete[] grid1D;
}

// Function to swap the pointers between the two grids
void swap_grid_pointers(double**& grid1, double**& grid2)
{
    double** temp;
    // Copy the pointer of first grid to temp
    temp = grid1;
    // Change pointers between the grids
    grid1 = grid2;
    // Copy back the pointer from temp
    grid2 = temp;

}

// Function to write to file the program parameters into a configuration file
void output_config(int& num_outputs, int& topology_rows, int& topology_cols, double& dt)
{
    std::fstream output_config;
    output_config.open("./output/mpi_config.txt", std::ios_base::out);

    // Push data to output file stream
    output_config << "Number of output files = " << num_outputs << std::endl;
    output_config << "Topology_rows = " << topology_rows << std::endl;
    output_config << "Topology_columns = " << topology_cols << std::endl;
    output_config << "Dt_out = " << dt << std::endl;

    output_config.close();
}










