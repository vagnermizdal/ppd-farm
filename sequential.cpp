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
 *  Description: This is a simple prime calculator
 *  File Name: sequential.cpp
 *  Version: 1.0 (02/06/2018)
 *  Compilation Command: g++ -std=c++1y ~/ppd-farm/sequential.cpp -o ~/shared/sequential.out
 *	Exacution Command: ~/shared/sequential.out
 */
#include <iostream>
#include <cmath>
#include <vector>
#include <chrono>

// Função para contar números primos até n
int prime_number(int n)
{
    int total = 0;
    for (int i = 2; i <= n; i++)
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
        total += prime;
    }
    return total;
}

int main(int argc, char *argv[])
{
    int N = atoi(argv[1]);

	auto t_start = std::chrono::high_resolution_clock::now();

    int global_count = prime_number(N);

	auto t_end = std::chrono::high_resolution_clock::now();

    std::cout << "O número total de primos até " << N << " é: " << global_count << std::endl;
    std::cout << "Execution time(s): " << std::chrono::duration<double>(t_end-t_start).count() << std::endl;

    return 0;
}
