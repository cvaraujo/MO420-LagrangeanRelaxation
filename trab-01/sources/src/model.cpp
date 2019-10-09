#include "../inc/model.h"

Model::Model(Graph *Graph) {
    if (graph != nullptr) {
        this->graph = graph;
        mst = MST_Kruskal(graph);
    }
    else exit(0);
}

bool Model::solve() {
    mst.solve();
    return true;
}

void Model::getGradient(vector<double> &gradient) {
    int i, j;
//    for (int k: data->terminals) {
//        gradientJK[k] = -data->paramJitter;
//        for (auto *arc : data->arcs) {
//            i = arc->getO(), j = arc->getD();
//            if (this->cplex.getValue(this->f[i][j][k]) >= 0.5) {
//                gradientJK[k] += arc->getJitter();
//            }
//        }
//        if (gradientJK[k] > 0) {
//            this->feasible = false;
//        }
//    }
}

double Model::getNorm(vector<double> gradient) {
    double sum = 0;
    for (int k = 0; k < graph->n; k++)
        sum += pow(gradient[k], 2);
    return sqrt(sum);
}

double Model::getObjValue() {
    return solution.value;
}

double Model::originalObjectiveValue() {
//    int i, j;
//    double objectiveValue = 0;
//    for (auto arc : data->nonDirectedArcs) {
//        i = arc->getO(), j = arc->getD();
//        if (cplex.getValue(x[i][j]) >= 0.5) {
//            objectiveValue +=
//                    (10e5 * thetaC) + (arc->getDelay() * (10e5 * thetaP)) - arc->getEstimateLinkDuration() * thetaD;
//        }
//    }

    return 0;
}

bool Model::isFeasible() {
    if (feasible) return true;
    feasible = true;
    return false;
}

double Model::lagrangean() {
    int progress = 0, iter = 0;
    double theta, originalObjectiveFunction, objectiveFunctionPPL;
    vector<double> gradient, nextMultipliers = vector<double>(graph->n);

    lambda = 1.5;
    max_iter = 30;
    B = 3;
    UB = 50; // Create a constructive heuristic
    LB = 0;


    while (iter < max_iter) {
        if (solve()){
            getGradient(gradient);

            objectiveFunctionPPL = getObjValue();
            theta = lambda * (UB - objectiveFunctionPPL) / pow(getNorm(gradient), 2);

            nextMultipliers = vector<double>(graph->n);

            for (int i = 0; i < graph->n; i++) {
                nextMultipliers[i] = max(0.0, multipliers[i] + gradient[i] * theta);
                multipliers[i] = nextMultipliers[i];
            }

            if (objectiveFunctionPPL > LB) {
                LB = objectiveFunctionPPL, progress = 0;
            } else progress++;

            if (progress == B) lambda /= 2.0;

            originalObjectiveFunction = originalObjectiveValue();

            if (isFeasible() && originalObjectiveFunction < UB) {
                UB = originalObjectiveFunction;
                if ((UB - LB) / LB <= 0.001) return LB;
            }

            cout << "(Feasible) Upper Bound = " << UB << ", (Relaxed) Lower Bound = " << LB << endl;
            iter++;
        }
    }
    return LB;
}

//
//void Model::showSolution(const char *input, const char *outputFile, double thetaC, double thetaP, double thetaD,
//                         double endTime) {
//    try {
//
//        double fo = 0;
//        int i, j;
//        for (auto *arc : data->nonDirectedArcs) {
//            i = arc->getO(), j = arc->getD();
//            if (cplex.getValue(this->x[i][j]) >= 0.5) {
//                //printf("[%d, %d], ", i, j);
//                fo += (10e5 * thetaC) + (arc->getDelay() * (10e5 * thetaP)) -
//                      arc->getEstimateLinkDuration() * thetaD;
//            }
//        }
//
//        FILE *output;
//        output = fopen(outputFile, "a");
//        fprintf(output, "%s: ", input);
//
//        fprintf(output,
//                " ---- CPLEX_FO = %g ---- FO = %lf ---- Time = %g ---- GAP = %g ---- LB = %g\n",
//                UB, fo, endTime, (UB - LB) / UB, LB);
//        fclose(output);
//    } catch (IloException &exception) {
//        FILE *output;
//        output = fopen(outputFile, "a");
//        fprintf(output, "%s: ", input);
//
//        fprintf(output,
//                " ---- FO = %lf ---- GAP = %g ---- LB = %g\n",
//                UB, (UB - LB) / UB, LB);
//        fclose(output);
//        cout << "Write: " << exception.getMessage() << endl;
//    }
//}