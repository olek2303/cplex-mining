#include <iostream>
#include "opl_solution.h"
#include "solution.h"

using namespace std;

int main() {
	cout << endl << "OPL SOLUTION: " << endl;
	opl_solution();

	cout << endl << "CPLEX SOLUTION: " << endl;
	// cplex_solution();

	mining_solution();

	return 0;
}