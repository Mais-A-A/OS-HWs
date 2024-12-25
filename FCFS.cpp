#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
using namespace std;

struct PCB {
    int id;
    int arrivalTime;
    int CPUBurst;
    int finishTime;
    int waitingTime;
    int turnAroundTime;
    int lastTimeInReady;
    int remainingBurst;
};

void calculateTimes(vector<PCB>& processes) {
    processes[0].waitingTime = 0;
    processes[0].finishTime = processes[0].arrivalTime + processes[0].CPUBurst;
    processes[0].turnAroundTime = processes[0].waitingTime + processes[0].CPUBurst;

    for (int i = 1; i < processes.size(); i++) {
        processes[i].waitingTime = max(0, processes[i - 1].finishTime - processes[i].arrivalTime);
        processes[i].turnAroundTime = processes[i].waitingTime + processes[i].CPUBurst;
        processes[i].finishTime = processes[i].arrivalTime + processes[i].turnAroundTime;
    }
}

void calculateAverages(vector<PCB>& processes) {
    double avgWaitingTime = 0;
    double avgTurnaroundTime = 0;

    for (auto& p : processes) {
        avgWaitingTime += p.waitingTime;
        avgTurnaroundTime += p.turnAroundTime;
    }

    avgWaitingTime = avgWaitingTime / processes.size();
    avgTurnaroundTime = avgTurnaroundTime / processes.size();

    cout << "Average Waiting Time: " << avgWaitingTime << endl;
    cout << "Average Turnaround Time: " << avgTurnaroundTime << endl;
}

void calculateCPUUtilization(vector<PCB>& processes) {
    int totalBurstTime = 0;
    int totalIdleTime = 0;

    for (auto& p : processes) {
        totalBurstTime += p.CPUBurst;
    }
    totalIdleTime = processes.back().finishTime - totalBurstTime;

    double cpuUtilization = ((double)totalBurstTime / (totalBurstTime + totalIdleTime)) * 100;
    cout << "CPU Utilization: " << cpuUtilization << "%" << endl;
}

void printGanttChart(vector<PCB>& processes) {
    cout << "\nGantt Chart:" << endl;
    int currentTime = 0;

    if (processes[0].arrivalTime > currentTime) {
        cout << "| Idle ";
        currentTime = processes[0].arrivalTime;
    }

    for (size_t i = 0; i < processes.size(); i++) {
        if (currentTime < processes[i].arrivalTime) {
            cout << "| Idle ";
            currentTime = processes[i].arrivalTime;
        }
        cout << "|  P" << processes[i].id << "  ";
        currentTime = processes[i].finishTime;
    }
    cout << "|" << endl;

    currentTime = 0;
    if (processes[0].arrivalTime > currentTime) {
        cout << currentTime << "      ";
        currentTime = processes[0].arrivalTime;
    }

    for (size_t i = 0; i < processes.size(); i++) {
        if (currentTime < processes[i].arrivalTime) {
            cout << currentTime << "     ";
            currentTime = processes[i].arrivalTime;
        }
        cout << currentTime << "      ";
        currentTime = processes[i].finishTime;
    }
    cout << processes.back().finishTime << "\n";
}

void FCFS(vector<PCB>& processes) {
    sort(processes.begin(), processes.end(), [](PCB a, PCB b) {
        return a.arrivalTime < b.arrivalTime;
    });

    int currentTime = 0;
    for (size_t i = 0; i < processes.size(); i++) {
        if (processes[i].arrivalTime > currentTime) {
            currentTime = processes[i].arrivalTime;
        }
        processes[i].waitingTime = currentTime - processes[i].arrivalTime;
        processes[i].turnAroundTime = processes[i].waitingTime + processes[i].CPUBurst;
        processes[i].finishTime = currentTime + processes[i].CPUBurst;
        currentTime = processes[i].finishTime;
    }
}

int main() {
    ifstream inputFile("FCFS.txt"); // Open the file
    if (!inputFile) {
        cerr << "Unable to open file." << endl;
        return 1; // Exit if file can't be opened
    }

    int n;
    inputFile >> n; // # of procces
    vector<PCB> processes(n);

    // taje (id, arrival time, burst time)
    for (int i = 0; i < n; i++) {
        inputFile >> processes[i].id >> processes[i].arrivalTime >> processes[i].CPUBurst;
    }
    inputFile.close();

    // fcfs schedulng
    FCFS(processes);
    calculateTimes(processes);
    cout << "*** FCFS Algorithm (First Come First Serve) ***\n";
    printGanttChart(processes);
    // proccess info wt, tat
    cout << "\nProcess Details\nID \tWT \tTAT\n" << "-----------------------\n";
    for (auto& p : processes) {
        cout << "P" << p.id << "\t" << p.waitingTime
             << "\t" << p.turnAroundTime << "\n";
    }
    cout << "\n";
    // avgs and cpu utili
    calculateAverages(processes);
    calculateCPUUtilization(processes);

    return 0;
}
