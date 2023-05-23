#include <iostream>
#include <vector>
#include "mqlib/heuristics/maxcut/burer2002.h"
#include "mqlib/problem/max_cut_heuristic.h"

class Burer2002Callback : public mqlib::MaxCutCallback {
public:
    Burer2002Callback() :
            lastNewBest_(0.0) {}

    bool Report(const mqlib::MaxCutSimpleSolution &sol, bool newBest,
                double runtime) override {
        if (newBest) {
            std::cout << "New best " << sol.get_weight() << " at " << runtime <<
                      std::endl;
            lastNewBest_ = runtime;
        }
        if (runtime - lastNewBest_ >= 5.0) {
            std::cout << "Exiting at runtime " << runtime << std::endl;
            return false;
        } else {
            return true;  // Keep going
        }
    }

    bool Report(const mqlib::MaxCutSimpleSolution &sol, bool newBest,
                double runtime, int /*iter*/) override {
        return Report(sol, newBest, runtime);
    }

private:
    double lastNewBest_;  // Elapsed seconds when last new best sol. was found
};

int main(int args, char **argv) {
    mqlib::MaxCutInstance mi("../extern/MQLib/bin/sampleMaxCut.txt");

    if (args < 2) {
        std::cout << "No graph provided, using MQLib sample graph. Usage: ./pheuristic INSTANCE" << std::endl;
    } else {
        std::cout << "Using graph provided via command line." << std::endl;
        std::string graph_path = argv[1];
        mqlib::MaxCutInstance mu(graph_path);
        mi = mu;
    }

    Burer2002Callback callback;
    mqlib::Burer2002 heur(mi, 5, false, &callback);
    const mqlib::MaxCutSimpleSolution &mcSol = heur.get_best_solution();
    std::cout << "Best objective value: " << mcSol.get_weight() << std::endl;
    const std::vector<int> &solution = mcSol.get_assignments();
    for (uint64_t i = 0; i < solution.size(); ++i) {
        std::cout << "Index " << i << " : " << solution[i] << std::endl;
    }
    return 0;
}
