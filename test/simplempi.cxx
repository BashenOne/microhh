#include <cstdio>
#include <mpi.h>

int main()
{
  // hard set processors
  const int npx = 4;
  const int npy = 4;

  // variables from header
  MPI_Comm commxy;
  MPI_Comm commx;
  MPI_Comm commy;

  int nprocs;
  int mpiid;
  int mpicoordx;
  int mpicoordy;

  // initialization function
  char err_buffer[MPI_MAX_ERROR_STRING];
  int  n, resultlen;

  n = MPI_Init(NULL, NULL);
  if(n != MPI_SUCCESS)
  {
    MPI_Error_string(n, err_buffer, &resultlen);
    std::printf("MPI ERROR: %s\n", err_buffer);
  }

  n = MPI_Comm_rank(MPI_COMM_WORLD, &mpiid);
  if(n != MPI_SUCCESS)
  {
    MPI_Error_string(n, err_buffer, &resultlen);
    std::printf("MPI ERROR: %s\n", err_buffer);
  }

  n = MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
  if(n != MPI_SUCCESS)
  {
    MPI_Error_string(n, err_buffer, &resultlen);
    std::printf("MPI ERROR: %s\n", err_buffer);
  }

  int dims    [2] = {npy, npx};
  int periodic[2] = {true, true};

  n = MPI_Dims_create(nprocs, 2, dims);
  if(n != MPI_SUCCESS)
  {
    MPI_Error_string(n, err_buffer, &resultlen);
    std::printf("MPI ERROR: %s\n", err_buffer);
  }

  n = MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periodic, true, &commxy);
  if(n != MPI_SUCCESS)
  {
    MPI_Error_string(n, err_buffer, &resultlen);
    std::printf("MPI ERROR: %s\n", err_buffer);
  }

  n = MPI_Comm_rank(commxy, &mpiid);
  if(n != MPI_SUCCESS)
  {
    MPI_Error_string(n, err_buffer, &resultlen);
    std::printf("MPI ERROR: %s\n", err_buffer);
  }

  int mpicoords[2];
  n = MPI_Cart_coords(commxy, mpiid, 2, mpicoords);
  if(n != MPI_SUCCESS)
  {
    MPI_Error_string(n, err_buffer, &resultlen);
    std::printf("MPI ERROR: %s\n", err_buffer);
  }

  mpicoordx = mpicoords[1];
  mpicoordy = mpicoords[0];

  int dimx[2] = {false, true };
  int dimy[2] = {true , false};

  n = MPI_Cart_sub(commxy, dimx, &commx);
  if(n != MPI_SUCCESS)
  {
    MPI_Error_string(n, err_buffer, &resultlen);
    std::printf("MPI ERROR: %s\n", err_buffer);
  }

  n = MPI_Cart_sub(commxy, dimy, &commy);
  if(n != MPI_SUCCESS)
  {
    MPI_Error_string(n, err_buffer, &resultlen);
    std::printf("MPI ERROR: %s\n", err_buffer);
  }

  return 0;
}
