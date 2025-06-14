#include "opl_solution.h"

using namespace std;

int opl_solution() {

	const char* mod_filename = "./yield.mod";
	const char* dat_filename = "./yield.dat";
	int status = 127;

	IloEnv env;


	try {
		IloOplErrorHandler handler(env, cout);
		IloOplModelSource modelSource(env, mod_filename);

		IloOplSettings settings(env, handler);
		settings.setWithWarnings(IloFalse);
		IloOplModelDefinition def(modelSource, settings);
		IloCplex cplex(env);
		IloOplModel opl(def, cplex);
		IloOplDataSource dataSource(env, dat_filename);
		opl.addDataSource(dataSource);
		opl.generate();

		if (cplex.solve()) {
			cout << endl
				<< "OBJECTIVE: " << fixed << setprecision(2) << opl.getCplex().getObjValue()
				<< endl;
			opl.postProcess();
			opl.printSolution(cout);
			status = 0;
		}
		else {
			cout << "No solution!" << endl;
			status = 1;
		}

		// pobrac solution pool i wyswietlic ostatni punkt, czyli najlepszy wynik

	}
	catch (IloOplException& e) {
		cout << "### OPL exception: " << e.getMessage() << endl;
	}
	catch (IloException& e) {
		cout << "### CONCERT exception: ";
		e.print(cout);
		status = 2;
	}
	catch (...) {
		cout << "### UNEXPECTED ERROR ..." << endl;
		status = 3;
	}

	cout << endl << "--Press to exit--" << endl;
	getchar();

	return status;

}