#include <iostream>
#include <vector>
#include "Epetra_BlockMap.h"            // for Epetra_BlockMap
#include "Epetra_CombineMode.h"         // for Epetra_CombineMode::Insert
#include "Epetra_Import.h"              // for Epetra_Import
#include "Epetra_MpiComm.h"             // for Epetra_MpiComm
#include "Epetra_Vector.h"              // for Epetra_Vectorxs
#include "mpi.h"                        // for MPI


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
  // [0], [2,1], [5,4,3], [9,8,7,6], [14,13,12,11,10]

  // For !isLow processors, the numbers 0..14 are split over 3 processors as
  // [0,1,2,3,4], [5,6,7,8,9], [10,11,12,13,14]
  
  bool isLow;
  std::vector<int> local_values;
  std::vector<int> peer_values;
  if (my_rank < 5) {
    isLow = true;
    local_values.resize(my_rank+1);
    int offset = my_rank*(my_rank+1)/2;
    for (unsigned int i = 0; i < (my_rank + 1); i++) {
      local_values[i] = (my_rank - (i-1)) + (offset-1);
      //if (my_rank==4) 
      //std::cout << local_values[i] << std::endl;
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
  Epetra_Import * importer;
  
  int blocksize = 1;
  
  if (isLow) {
    local_map = new Epetra_BlockMap (-1,
  				     local_values.size(),
  				     &local_values[0],
  				     blocksize,
  				     0,
  				     eComm);
    local_vec = new Epetra_Vector   (*local_map, false);

    // Shouldn't matter what goes here after the import
    peer_map = new Epetra_BlockMap (-1,
  				    local_values.size(),
  				    &local_values[0],
  				    blocksize,
  				    0,
  				    eComm);
    peer_vec = new Epetra_Vector   (*peer_map, false);
    importer = new Epetra_Import   (*peer_map, *local_map);
  } else {
    local_map = new Epetra_BlockMap (-1,
  				     peer_values.size(),
  				     &peer_values[0],
  				     blocksize,
  				     0,
  				     eComm);
    local_vec = new Epetra_Vector   (*local_map, false);

    // Shouldn't matter what goes here after the import
    peer_map = new Epetra_BlockMap (-1,
  				    local_values.size(),
  				    &local_values[0],
  				    blocksize,
  				    0,
  				    eComm);
    peer_vec = new Epetra_Vector   (*peer_map, false);
    importer = new Epetra_Import   (*peer_map, *local_map);
  }
			    
  Epetra_Vector& write_into_vec = *local_vec;

  for (unsigned int i=0; i<local_values.size(); i++) {
    write_into_vec[i] = local_values[i] + 50;
  }

  peer_vec->Import(*local_vec, *importer, Insert);

  Epetra_Vector& transferred_vec = *peer_vec;
  if (my_rank==6) {
    for (unsigned int i=0; i<local_values.size(); i++) {
      std::cout << transferred_vec[i] << std::endl;
    }
  }
	
  delete importer;  
  delete local_vec;
  delete peer_vec;
  delete local_map;
  delete peer_map;
  
  // Finalize the MPI environment.
  MPI_Finalize();
  return 0;
}
