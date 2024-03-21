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
 *  Authors: Vagner Rigon Mizdal <vagner.mizdal@gmail.com>
 *
 *  Copyright: GNU General Public License
 *  Description: This is a simple prime calculator with MPI
 *  File Name: parallel.cpp
 *  Version: 1.0 (02/06/2018)
 *  Compilation Command: mpic++ -O3 -std=c++1y -Wall ~/ppd-farm/parallel.cpp -o ~/shared/parallel.out
 *	Exacution Command: mpirun -np 6 --machinefile ~/.cluster_hostfile ~/shared/parallel.out
 */
#include <iostream>
#include <mpi.h>
#include <cmath>
#include <vector>
#include <chrono>
#include <string>

int prime_number(int start, int end)
{
    int total = 0;
    for (int i = start; i <= end; i++)
    {
        int prime = 1;
        for (int j = 2; j < i; j++)
        {
            if (i % j == 0)
            {
                prime = 0;
                break;
            }
        }
        total = total + prime;
    }

    return total;
}

const std::string DELIMITER = "|";

std::string serialize(int from, int to)
{
    return std::to_string(from) + DELIMITER + std::to_string(to);
}

int deserializeFrom(std::string s)
{
    return std::stoi(s.substr(0, s.find(DELIMITER)));
}

int deserializeTo(std::string s)
{
    return std::stoi(s.substr(s.find(DELIMITER) + 1, s.length() - 1));
}

const int EMITTER = 0;
const int COLLECTOR = 1;

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size < 3)
    {
        std::cout << "This program needs at least 3 processors" << std::endl;
        return 1;
    }

    int N = atoi(argv[1]);
    auto t_start = std::chrono::high_resolution_clock::now();

    // MPI_Bcast(&t_start, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    if (rank == EMITTER)
    {
        const int batch_size = N / (size - 2);

        MPI_Send(&t_start, 1, MPI_DOUBLE, COLLECTOR, 0, MPI_COMM_WORLD);

        for (int rank = 2; rank < size; ++rank)
        {
            int start = (rank - 2) * batch_size + 1;
            int end = (rank == size - 1) ? N : start + batch_size - 1;

            auto content = serialize(start, end);
            
            MPI_Send(&content[0], content.length(), MPI_CHAR, rank, 0, MPI_COMM_WORLD);
        }
    }
    else if (rank == COLLECTOR)
    {
        int worker_result,
            global_count = 0;

        MPI_Recv(&t_start, 1, MPI_DOUBLE, EMITTER, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        for (int src = 2; src < size; ++src)
        {
            MPI_Recv(&worker_result, 1, MPI_INT, src, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            global_count += worker_result;
        }

	    auto t_end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed_seconds = t_end - t_start;
        std::cout << "O número total de primos até " << N << " é: " << global_count << std::endl;
        std::cout << "Execution time(s): " << elapsed_seconds.count() << std::endl;
    }
    else // Workers
    {
        MPI_Status status;
        MPI_Probe(EMITTER, 0, MPI_COMM_WORLD, &status);
        int count;
        MPI_Get_count(&status, MPI_CHAR, &count);
        char buf[count];
        MPI_Recv(&buf, count, MPI_CHAR, EMITTER, 0, MPI_COMM_WORLD, &status);
        std::string content = buf;

        int start = deserializeFrom(content);
        int end = deserializeTo(content);

        int worker_result = prime_number(start, end);

        MPI_Send(&worker_result, 1, MPI_INT, COLLECTOR, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
