#include <bits/stdc++.h>

#include "BornChart.cpp"

int main() {

    int totalTasks = 100;
    int totalDays = 20;
    bool randomBurn = true;
    std::vector<int> burndown = simulateBurndown(totalTasks, totalDays, randomBurn);
    printBurndownChart(burndown);


    return 0;
}
