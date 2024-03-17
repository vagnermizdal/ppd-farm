/* ***************************************************************************
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 *  As a special exception, you may use this file as part of a free software
 *  library without restriction.  Specifically, if other files instantiate
 *  templates or use macros or inline functions from this file, or you compile
 *  this file and link it with other files to produce an executable, this
 *  file does not by itself cause the resulting executable to be covered by
 *  the GNU General Public License.  This exception does not however
 *  invalidate any other reasons why the executable file might be covered by
 *  the GNU General Public License.
 *
 ****************************************************************************
 *  Authors: Dalvan Griebler <dalvangriebler@gmail.com>
 *
 *  Copyright: GNU General Public License
 *  Description: This is a simple prime calculator with MPI
 *  File Name: prime.cpp
 *  Version: 1.0 (02/06/2018)
 *  Compilation Command: mpic++ -O3 -std=c++1y -Wall /home/mpihpc/ppd-farm/prime.cpp -o /home/mpihpc/shared/exe.out
 *	Exacution Command: mpirun -np 4 ./exe
 */
#include <iostream>
#include <mpi.h>
#include <cmath>
#include <vector>
#include <chrono>

bool is_prime(int n)
{
  if (n <= 1)
    return false;
  if (n <= 3)
    return true;
  if (n % 2 == 0 || n % 3 == 0)
    return false;
  for (int i = 5; i * i <= n; i += 6)
    if (n % i == 0 || n % (i + 2) == 0)
      return false;
  return true;
}

const int EMITTER=0;
const int COLLECTOR=1;

int main(int argc, char *argv[])
{
  MPI_Init(&argc, &argv);

  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  
  int N = atoi(argv[1]);
  double start_time = MPI_Wtime();

  if (rank == EMITTER)
  {
    const int chunk_size = N / (size - 1); // Tamanho do intervalo para cada worker

    MPI_Send(&start_time, 1, MPI_DOUBLE, COLLECTOR, 0, MPI_COMM_WORLD);
    // Emissor
    for (int dest = 2; dest < size; ++dest)
    {
      int start = (dest - 2) * chunk_size + 1;
      int end = (dest == size - 1) ? N : dest * chunk_size;
      MPI_Send(&start, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
      MPI_Send(&end, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
    }
  }
  else if (rank == COLLECTOR)
  {
    // Coletor
    MPI_Recv(&start_time, 1, MPI_DOUBLE, EMITTER, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    int global_count = 0;

    for (int src = 2; src < size; ++src)
    {
      int local_count;
      MPI_Recv(&local_count, 1, MPI_INT, src, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      global_count += local_count;
    }

    double end_time = MPI_Wtime();
    std::cout << "O número total de primos até " << N << " é: " << global_count << std::endl;
    std::cout << "Execution time(s): " << (end_time - start_time) << std::endl;
  }
  else
  {
    // Trabalhadores
    int start, end;
    MPI_Recv(&start, 1, MPI_INT, EMITTER, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Recv(&end, 1, MPI_INT, EMITTER, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    int local_count = 0;
    for (int i = start; i <= end; ++i)
    {
      if (is_prime(i))
      {
        ++local_count;
      }
    }

    MPI_Send(&local_count, 1, MPI_INT, COLLECTOR, 0, MPI_COMM_WORLD); // Envia contagem local para o coletor
  }

  MPI_Finalize();
  return 0;
}
