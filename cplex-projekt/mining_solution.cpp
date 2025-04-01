#include "solution.h"

using namespace std;

ILOSTLBEGIN

int mining_solution() {
    try {
        IloEnv env;
        IloModel model(env);

        vector<string> mines= { "MINE1", "MINE2", "MINE3", "MINE4", };
        vector<string> years= { "YEAR1", "YEAR2", "YEAR3", "YEAR4", "YEAR5"};

        map<std::string, std::string> NEXT_YEAR = {
            {"YEAR1", "YEAR2"},
            {"YEAR2", "YEAR3"},
            {"YEAR3", "YEAR4"},
            {"YEAR4", "YEAR5"}
        };

    	map<std::string, double> royalties = {
            {"MINE1", 5e6}, {"MINE2", 4e6}, {"MINE3", 4e6}, {"MINE4", 5e6}
        };

        map<std::string, double> ore_limit = {
            {"MINE1", 2e6}, {"MINE2", 2.5e6}, {"MINE3", 1.3e6}, {"MINE4", 3e6}
        };

        map<std::string, double> ore_quali = {
            {"MINE1", 1.0}, {"MINE2", 0.7}, {"MINE3", 1.5}, {"MINE4", 0.5}
        };

        map<std::string, double> req_quali = {
            {"YEAR1", 0.9}, {"YEAR2", 0.8}, {"YEAR3", 1.2}, {"YEAR4", 0.6}, {"YEAR5", 1.0}
        };
        
        map<std::string, double> year_disc;
        double blend_price = 10;
        double rev_disc_rate = 0.1;
        for (size_t i = 0; i < years.size(); i++) {
            year_disc[years[i]] = pow(1 / (1 + rev_disc_rate), i);
        }

        std::cout << "Wszystkie zmienne zosta³y poprawnie zadeklarowane." << std::endl;


        IloNumVarArray make(env, years.size(), 0, IloInfinity, ILOFLOAT);
        IloNumVarArray extract(env, mines.size() * years.size(), 0, IloInfinity, ILOFLOAT);
        IloNumVarArray used(env, mines.size() * years.size(), 0, 1, ILOBOOL);
        IloNumVarArray active(env, mines.size() * years.size(), 0, 1, ILOBOOL);

        // Funkcja celu
        IloExpr obj(env);
        for (size_t i = 0; i < years.size(); i++) {
            obj += blend_price * year_disc[years[i]] * make[i];
            for (size_t j = 0; j < mines.size(); j++) {
                obj -= royalties[mines[j]] * year_disc[years[i]] * active[j * years.size() + i];
            }
        }
        model.add(IloMaximize(env, obj));
        obj.end();

        // Ograniczenia
        for (size_t i = 0; i < years.size(); i++) {
            IloExpr sum_used(env);
            for (size_t j = 0; j < mines.size(); j++) {
                sum_used += used[j * years.size() + i];
            }
            model.add(sum_used <= 3);
            sum_used.end();
        }

        for (size_t i = 0; i < years.size(); i++) {
            IloExpr sum_ore(env);
            IloExpr sum_make(env);
            for (size_t j = 0; j < mines.size(); j++) {
                model.add(extract[j * years.size() + i] <= ore_limit[mines[j]] * used[j * years.size() + i]);
                model.add(used[j * years.size() + i] <= active[j * years.size() + i]);
                sum_ore += ore_quali[mines[j]] * extract[j * years.size() + i];
                sum_make += extract[j * years.size() + i];
            }
            model.add(sum_ore == req_quali[years[i]] * make[i]);
            model.add(sum_make == make[i]);
            sum_ore.end();
            sum_make.end();
        }

        for (size_t i = 0; i < years.size() - 1; i++) {
            for (size_t j = 0; j < mines.size(); j++) {
                model.add(active[j * years.size() + (i + 1)] <= active[j * years.size() + i]);
            }
        }

        // Rozwi¹zanie
        IloCplex cplex(model);
        if (cplex.solve()) {
            std::cout << "Profit of $" << std::fixed << std::setprecision(2) << cplex.getObjValue() << std::endl;

            std::cout << "=== PRODUCING PLAN ===" << std::endl;
            for (size_t i = 0; i < years.size(); i++) {
                std::cout << years[i] << ": " << cplex.getValue(make[i]) << " tons" << std::endl;
            }

            std::cout << "\n=== EXTRACTION PLAN ===" << std::endl;
            for (size_t i = 0; i < years.size(); i++) {
                std::cout << years[i] << ": ";
                for (size_t j = 0; j < mines.size(); j++) {
                    std::cout << mines[j] << "=" << cplex.getValue(extract[j * years.size() + i]) << " ";
                }
                std::cout << std::endl;
            }
        }
        else {
            std::cerr << "Nie znaleziono optymalnego rozwi¹zania." << std::endl;
        }


    } catch (IloException& e) {
        cerr << "B³¹d CPLEX: " << e << endl;
    }
    return 0;
}