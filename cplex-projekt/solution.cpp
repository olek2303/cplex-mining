#include "solution.h"

using namespace std;

ILOSTLBEGIN

int cplex_solution() {
    try {
        IloEnv env;
        IloModel model(env);

        // Definicja klas
        vector<string> Klasy = { "First", "Business", "Economy" };
        vector<double> Scenarios = { 0.1, 0.7, 0.2 };
        vector<int> Capacity = { 37, 38, 47 };

        // Dane cen
        vector<vector<vector<int>>> CurrentTable = {
            {{1200, 900, 500}, {1000, 800, 300}, {950, 600, 200}},
            {{1400, 1100, 700}, {1300, 900, 400}, {1150, 750, 350}},
            {{1500, 820, 480}, {900, 800, 470}, {850, 500, 450}}
        };

        // Prognozowane zapotrzebowanie
        vector<vector<vector<int>>> ForecastDemand = {
            {{10, 20, 45}, {15, 25, 55}, {20, 35, 60}},
            {{20, 40, 50}, {25, 42, 52}, {35, 45, 63}},
            {{45, 50, 55}, {50, 46, 56}, {60, 47, 64}},
            {{20, 42, 50}, {25, 45, 52}, {35, 46, 60}},
            {{10, 50, 60}, {40, 60, 65}, {50, 80, 90}},
            {{50, 20, 10}, {55, 30, 40}, {80, 50, 60}},
            {{30, 40, 50}, {35, 50, 60}, {40, 55, 80}},
            {{30, 10, 50}, {40, 40, 60}, {60, 45, 70}},
            {{50, 40, 60}, {70, 45, 65}, {80, 60, 70}}
        };

        // Tworzenie zmiennych decyzyjnych w CPLEX
        IloArray<IloNumVarArray> x(env, 3);
        IloArray<IloNumVarArray> y(env, 3);
        for (int i = 0; i < 3; i++) {
            x[i] = IloNumVarArray(env, 3, 0, IloInfinity, ILOINT);
            y[i] = IloNumVarArray(env, 3, 0, IloInfinity, ILOINT);
           /* x[i] = IloNumVarArray(env, 3, 0, IloInfinity, ILOFLOAT);
            y[i] = IloNumVarArray(env, 3, 0, IloInfinity, ILOFLOAT);*/
        }

        // Tworzenie funkcji celu
        IloExpr objExpr(env);
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                objExpr += Scenarios[j] * CurrentTable[i][j][i] * x[i][j];
            }
        }
        model.add(IloMaximize(env, objExpr));
        objExpr.end();

        // Ograniczenie liczby dostêpnych miejsc
        for (int i = 0; i < 3; i++) {
            IloExpr capExpr(env);
            for (int j = 0; j < 3; j++) {
                capExpr += x[i][j];
            }
            model.add(capExpr <= Capacity[i]);
            capExpr.end();
        }

        // Ca³kowity popyt podzielony na bilety sprzedane i niesprzedane
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                //model.add(x[i][j] + y[i][j] <= ForecastDemand[i][j][i]);
                model.add(x[i][j] + y[i][j] == ForecastDemand[i][j][i]);
            }
        }

        // przenoszenie miejsc miêdzy kategoriami (do 10%)
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                for (int k = 0; k < 3; k++) {
                    model.add(x[i][j] <= 1.1 * x[i][k]);
                }
            }
        }

        // Tworzenie solvera
        IloCplex cplex(model);
    	if (cplex.solve()) {
            cout << "SOLNPOOLCAP: " << cplex.SolnPoolCapacity << endl;
            cout << "X: [\n";
            for (int i = 0; i < 3; i++) {
                cout << "\t[";
                for (int j = 0; j < 3; j++) {
                    cout << cplex.getValue(x[i][j]) << " ";
                }
                cout << "]\n";
            }

            cout << "]\n";

            cout << "Y: [\n";
            for (int i = 0; i < 3; i++) {
                cout << "\t[";
                for (int j = 0; j < 3; j++) {
                    cout << cplex.getValue(y[i][j]) << " ";
                }
                cout << "]\n";
            }

            cout << "]\n";
            cout << "Optymalne rozwi¹zanie: " << cplex.getObjValue() << endl;
            cout << "getSolnPoolNsolns: " << cplex.getSolnPoolNsolns() << endl;
            cout << "Status: " << cplex.getStatus() << endl;
            cout << "getSolnPoolNreplaced: " << cplex.getSolnPoolNreplaced() << endl;

        }
        else {
            cout << "Nie znaleziono optymalnego rozwi¹zania." << endl;
        }

        env.end();
    }
    catch (IloException& e) {
        cerr << "B³¹d CPLEX: " << e << endl;
    }

    return 0;
}