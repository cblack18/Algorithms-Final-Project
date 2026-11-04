// Checkpoint 2 Initial Code
// Authors: Owen Dzurko & Cameron Black

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

const int SALARY_CAP = 40000;

struct Player {
    int id;
    std::string name;
    std::string pos;
    int salary;
    double points;
};

int main() {
    std::ifstream inFile("baseline_data.csv");
    if (!inFile.is_open()) {
        std::cout << "ERROR: COULD NOT OPEN FILE\n";
        return 1;
    }

    std::vector<Player> qbs;
    std::vector<Player> others;

    std::string line;

    // 1. Skip the header row so stoi() doesn't try to parse column names
    getline(inFile, line);

    while (getline(inFile, line)) {
        if (line.empty()) { continue; }

        Player p;
        std::string temp = "";
        int field = 0;

        for (int i = 0; i <= line.length(); i++) {
            if (i == line.length() || line[i] == ',') {
                // Strip Windows carriage return '\r' if present
                if (!temp.empty() && temp.back() == '\r') {
                    temp.pop_back();
                }

                if (field == 0) {
                    p.id = stoi(temp);
                }
                else if (field == 1) {
                    p.name = temp;
                }
                else if (field == 2) {
                    p.pos = temp;
                }
                else if (field == 3) {
                    if (!temp.empty() && temp[0] == '$') temp = temp.substr(1);
                    p.salary = stoi(temp);
                }
                else if (field == 4) {
                    p.points = stod(temp);
                }

                temp = "";
                field++;
            }
            else {
                temp += line[i];
            }
        }

        if (p.pos == "QB") {
            qbs.push_back(p);
        }
        else if (p.pos == "RB" || p.pos == "WR" || p.pos == "TE") {
            others.push_back(p);
        }
    }
    inFile.close();

    if (qbs.empty() || others.size() < 6) {
        std::cout << "ERROR: NOT ENOUGH PLAYERS\n";
        return 1;
    }

    std::vector<Player> bestLineup;
    double bestPoints = -1.0;
    int bestSalary = 0;

    int totalOthers = others.size();

    // Check every QB paired with every combination of 6 skill players
    for (int q = 0; q < qbs.size(); q++) {
        for (int i = 0; i < totalOthers; i++) {
            for (int j = i + 1; j < totalOthers; j++) {
                for (int k = j + 1; k < totalOthers; k++) {
                    for (int l = k + 1; l < totalOthers; l++) {
                        for (int m = l + 1; m < totalOthers; m++) {
                            for (int n = m + 1; n < totalOthers; n++) {

                                Player p1 = others[i];
                                Player p2 = others[j];
                                Player p3 = others[k];
                                Player p4 = others[l];
                                Player p5 = others[m];
                                Player p6 = others[n];

                                // Count roster positions
                                int rbs = 0, wrs = 0, tes = 0;
                                std::vector<Player> group = { p1, p2, p3, p4, p5, p6 };

                                for (int idx = 0; idx < 6; idx++) {
                                    if (group[idx].pos == "RB") rbs++;
                                    else if (group[idx].pos == "WR") wrs++;
                                    else if (group[idx].pos == "TE") tes++;
                                }

                                // Must have at least 2 RBs, 2 WRs, and 1 TE
                                if (rbs < 2 || wrs < 2 || tes < 1) {
                                    continue;
                                }

                                int currentSalary = qbs[q].salary + p1.salary + p2.salary +
                                    p3.salary + p4.salary + p5.salary + p6.salary;

                                if (currentSalary > SALARY_CAP) {
                                    continue;
                                }

                                double currentPoints = qbs[q].points + p1.points + p2.points +
                                    p3.points + p4.points + p5.points + p6.points;

                                // Tie-breaker: higher points, or lower salary if tied
                                if (currentPoints > bestPoints ||
                                    (currentPoints == bestPoints && currentSalary < bestSalary)) {
                                    bestPoints = currentPoints;
                                    bestSalary = currentSalary;
                                    bestLineup = { qbs[q], p1, p2, p3, p4, p5, p6 };
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    if (bestLineup.empty()) {
        std::cout << "No valid lineup found under the salary cap.\n";
        return 0;
    }

    // Output lineup
    std::cout << "Optimal Lineup:\n";
    for (int i = 0; i < bestLineup.size(); i++) {
        std::cout << "ID: " << bestLineup[i].id
            << " | " << bestLineup[i].pos
            << " | " << bestLineup[i].name
            << " | Salary: $" << bestLineup[i].salary
            << " | Points: " << bestLineup[i].points << "\n";
    }

    std::cout << "\nTotal Salary: $" << bestSalary;
    std::cout << "\nRemaining Cap: $" << (SALARY_CAP - bestSalary);
    std::cout << "\nTotal Projected Points: " << bestPoints << "\n";

    return 0;
}