#include <iostream>
#include <vector>
#include <algorithm>
#include <climits>
#include <fstream>  // Include this for file input

using namespace std;

struct Process {
//int id, at, bt, rt, ct, tat, wt;
    int id;
    int arrivalTime;
    int CPUBurst;
    int finishTime;
    int waitingTime;
    int turnAroundTime;
    int lastTimeInReady;
    int remainingBurst;
    
};
void calculateAverages(vector<Process>& processes) {
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
void calculateCPUUtilization(vector<Process>& processes) {
    int totalBurstTime = 0;
    int totalTime = 0;
    for (auto& p : processes) { //calc tot burst time
        totalBurstTime += p.CPUBurst;
    }
    /// this is gthe last number in the grant chart
    int lastNum = 0;
    for (auto& p : processes) {
        lastNum = max(lastNum, p.finishTime);
    }
    totalTime = lastNum - processes.front().arrivalTime;
    if (totalTime == 0) {
        cout << "CPU Utilization: 100%" << endl;
        return;
    }
    double cpuUtilization = ((double)totalBurstTime / totalTime) * 100;
    cout << "CPU Utilization: " << cpuUtilization << "%" << endl;
}
bool arrivalTimeSort(Process p1, Process p2) {
    return p1.arrivalTime < p2.arrivalTime;
}

void SJF(vector<Process>& processes, int n, vector<pair<int, int>>& executionOrder) {
    
    int currentTime = 0;
    int completed = 0;
    vector<bool> isCompleted(n, false);

    while (completed < n) {
        int idx = -1;
        int minRemainingTime = INT_MAX;
        for (int i = 0; i < n; i++) {
            if (!isCompleted[i] && processes[i].arrivalTime <= currentTime && processes[i].remainingBurst < minRemainingTime) {
                minRemainingTime = processes[i].remainingBurst;
                idx = i;
            }
        }
        if (idx == -1) {
            currentTime++;
        } else {
            processes[idx].remainingBurst--;
            currentTime++;
            
            if (executionOrder.empty() || executionOrder.back().first != processes[idx].id) {
                executionOrder.push_back({processes[idx].id,currentTime - 1});
            }

            // if process completed calculate its tat & wt
            if (processes[idx].remainingBurst == 0) {
                processes[idx].finishTime = currentTime;
                processes[idx].turnAroundTime = processes[idx].finishTime - processes[idx].arrivalTime;
                processes[idx].waitingTime = processes[idx].turnAroundTime - processes[idx].CPUBurst;
                isCompleted[idx] = true;
                completed++;
            }
        }
    }
}

void ganttChart(const vector<pair<int, int>>& executionOrder,vector<Process>& processes) {
    cout << "\nGantt Chart:\n";

    int startTime = 0;
    vector<int> startTimes;
    
    for (auto& segment : executionOrder) {
        int processID = segment.first;
        int endTime = segment.second;
        cout << "| P" << processID << " ";
        startTime = endTime;
        startTimes.push_back(startTime);
    }
    cout << "|\n";
    for (int i = 0; i < startTimes.size(); i++) {
        cout << startTimes[i] << "    ";
    }
    
    int maxCompletionTime = 0;
    for (auto& p : processes) {
        maxCompletionTime = max(maxCompletionTime, p.finishTime);
    }
    cout<<maxCompletionTime;
    cout << "\n";
}

int main() {
    ifstream inputFile("SJF.txt"); 

    if (!inputFile) {
        cerr << "Error opening the file!" << endl;
        return 1; 
    }
    int n;
    inputFile >> n;

    vector<Process> processes(n);
    for (int i = 0; i < n; i++) {
        inputFile >> processes[i].id;
        inputFile >> processes[i].arrivalTime;
        inputFile >> processes[i].CPUBurst;
        processes[i].remainingBurst = processes[i].CPUBurst;
    }

    inputFile.close(); 
    sort(processes.begin(), processes.end(), arrivalTimeSort);
    vector<pair<int, int>> executionOrder;
    SJF(processes, n, executionOrder);
    ganttChart(executionOrder, processes);

    cout << "\nProcess Details\nID \tWT \tTAT\n" << "-----------------------\n";    
    for (int i = 0; i < n; i++) {
        cout << processes[i].id << "\t" << processes[i].waitingTime << "\t" 
             << processes[i].turnAroundTime << "\n";
    }
    cout << "\n";
    calculateAverages(processes);
    calculateCPUUtilization(processes);
    
    return 0;
}
