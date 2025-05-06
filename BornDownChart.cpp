#include <iostream>
#include <vector>
#include <iomanip>
#include <cstdlib>
#include <ctime>

// 模拟每日燃尽任务数（随机或线性）
std::vector<int> simulateBurndown(int totalTasks, int totalDays, bool useRandom = false) {
    std::vector<int> remainingTasks(totalDays + 1);
    remainingTasks[0] = totalTasks;

    int remaining = totalTasks;
    srand(static_cast<unsigned>(time(0)));

    for (int day = 1; day <= totalDays; ++day) {
        int burn = useRandom
                   ? rand() % (totalTasks / totalDays + 2) // 随机波动燃尽
                   : totalTasks / totalDays;               // 均匀燃尽

        burn = std::min(burn, remaining);
        remaining -= burn;
        remainingTasks[day] = remaining;
    }

    return remainingTasks;
}

// 打印燃尽图（文本图表）
void printBurndownChart(const std::vector<int>& remainingTasks) {
    std::cout << "\nBurndown Chart:\n";
    for (size_t day = 0; day < remainingTasks.size(); ++day) {
        std::cout << "Day " << std::setw(2) << day << " ["
                  << std::string(remainingTasks[day] / 2, '#') << "] "
                  << remainingTasks[day] << " tasks\n";
    }
}