#include <iostream>
#include <vector>
#include "Epetra_BlockMap.h"            // for Epetra_BlockMap
#include "Epetra_CombineMode.h"         // for Epetra_CombineMode::Insert
#include "Epetra_Export.h"              // for Epetra_Export
#include "Epetra_MpiComm.h"             // for Epetra_MpiComm
#include "Epetra_Vector.h"              // for Epetra_Vectors
#include "mpi.h"                        // for MPI

/*
 *  The goal of this example is to demonstrate the mapping necessary
 *  to construct portions of a solution that must then be
 *  transferred to a unique entry . 
 */

int main(int argc, char *argv[]) {
  MPI_Init(&argc,&argv);

  // Get the number of processes
  int world_size;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);

  // Check that enough processors are available
  if (world_size != 8) {
    MPI_Finalize();
    return -1;
  };
  
  // Get the rank of the process
  int my_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

  // For isLow processors, the numbers 0..14 are split over 5 processors as
  // [1,0], [3,2,1], [6,5,4,3], [10,9,8,7,6], [14,13,12,11,10]

  // For !isLow processors, the numbers 0..14 are split over 3 processors as
  // [0,1,2,3,4], [5,6,7,8,9], [10,11,12,13,14]
  
  bool isLow;
  std::vector<int> local_values;
  std::vector<int> peer_values;
  if (my_rank < 5) {
    int entry_size = my_rank + 2;
    if (my_rank == 4) entry_size = my_rank + 1;  
    isLow = true;
    local_values.resize(entry_size);
    int offset = my_rank*(my_rank+1)/2;
    for (unsigned int i = 0; i < entry_size; i++) {
      local_values[i] = (my_rank+(my_rank<4) - (i-1)) + (offset-1);
      // if (my_rank==4) 
      //   std::cout << local_values[i] << std::endl;
    }
  } else {
    isLow = false;
    local_values.resize(5);
    peer_values.resize(5);
    for (unsigned int i = 0; i < 5; i++) {
      local_values[i] = 5*(my_rank-5) + i;
      //std::cout << local_values[i] << std::endl;
      peer_values[i] = local_values[i] + 15;
      //std::cout << peer_values[i] << std::endl;
    }
  }

  // Beginning of Epetra materials
  Epetra_MpiComm eComm(MPI_COMM_WORLD);

  Epetra_BlockMap * local_map;
  Epetra_BlockMap * peer_map;
  Epetra_Vector * local_vec;
  Epetra_Vector * peer_vec;
  Epetra_Export * exporter;
  
  int blocksize = 1;
  
  if (isLow) {
    local_map = new Epetra_BlockMap (-1,
  				     local_values.size(),
  				     &local_values[0],
  				     blocksize,
  				     0,
  				     eComm);
    local_vec = new Epetra_Vector   (*local_map, true);

    // Shouldn't matter what goes here after the export
    peer_map = new Epetra_BlockMap (-1,
  				    0,
  				    NULL,
  				    blocksize,
  				    0,
  				    eComm);
    peer_vec = new Epetra_Vector   (*peer_map, false);
    exporter = new Epetra_Export   (*local_map, *peer_map);
  } else {
    local_map = new Epetra_BlockMap (-1,
  				     0,
  				     NULL,
  				     blocksize,
  				     0,
  				     eComm);
    local_vec = new Epetra_Vector   (*local_map, false);

    // Shouldn't matter what goes here after the export
    peer_map = new Epetra_BlockMap (-1,
  				    local_values.size(),
  				    &local_values[0],
  				    blocksize,
  				    0,
  				    eComm);
    peer_vec = new Epetra_Vector   (*peer_map, true);
    exporter = new Epetra_Export   (*local_map, *peer_map);
  }
			    
  Epetra_Vector& write_into_vec = *local_vec;

  for (unsigned int i=0; i<local_map->NumMyElements(); i++) {
    write_into_vec[i] = local_values[i] + 50;
    // if (my_rank==0) std::cout << write_into_vec[i] << std::endl;
  }

  peer_vec->Export(*local_vec, *exporter, Add);

  Epetra_Vector& transferred_vec = *peer_vec;
  if (my_rank==5) {
    for (unsigned int i=0; i<peer_map->NumMyElements(); i++) {
      std::cout << transferred_vec[i] << std::endl;
    }
  }
	
  delete exporter;  
  delete local_vec;
  delete peer_vec;
  delete local_map;
  delete peer_map;
  
  // Finalize the MPI environment.
  MPI_Finalize();
  return 0;
}
