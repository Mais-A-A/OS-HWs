#include <iostream>
#include <vector>
#include <algorithm>
#include <deque>
#include <climits>
#include <iomanip>
#include <fstream>

using namespace std;

class Process {
public:
    int id;
    int arrivalTime;
    int CPUBurst;
    int finishTime;
    int waitingTime;
    int turnAroundTime;
    int lastTimeInReady;
    int remainingBurst;
    int responseTime;

    Process(int id, int arrivalTime, int CPUBurst) // Constructor
    {
        this->id = id;
        this->arrivalTime = arrivalTime;
        this->CPUBurst = CPUBurst;
        this->remainingBurst = CPUBurst;
        this->lastTimeInReady = arrivalTime;  
        this->finishTime = 0;
        this->waitingTime = 0;
        this->turnAroundTime = 0;
        this->responseTime = -1;
    }
};
bool compareProcesses(const Process &a, const Process &b, int time) {
    if (a.lastTimeInReady != b.lastTimeInReady) {
        return a.lastTimeInReady < b.lastTimeInReady;
    }
    return a.arrivalTime < b.arrivalTime;
}

void custom_sort(deque<Process> &d, int time) {
    sort(d.begin(), d.end(),
         [&time](const Process &a, const Process &b) {
             return compareProcesses(a, b, time);
         });
}

Process processingInRR(int time, Process &process, vector<Process> &processes, bool lastTime, int quantum) {
    int currentWaitingTime = time - min(quantum, process.remainingBurst ) - process.lastTimeInReady;
    process.waitingTime += currentWaitingTime;
    
    // Update finish time and remaining burst time
    if (lastTime) process.finishTime = time;
    process.remainingBurst -= quantum;
    process.lastTimeInReady = time;
    process.turnAroundTime = process.finishTime - process.arrivalTime;
    
    return process;
}

void processDet(vector<Process> &processes) {
    sort(begin(processes), end(processes),
         [](Process a, Process b) { return a.id <= b.id; });

    cout << "\nProcess Details\nID \tWT \tTAT\n" << "-----------------------\n";
    for (auto& p : processes) {
        cout << "P" << p.id << "\t" << p.waitingTime
             << "\t" << p.turnAroundTime << "\n";
    }
    cout << "\n";
}
void avarages(vector<Process> processes, int timeEnding, int processesNum) {
    double avgWaitingTime = 0, avgTurnaroundTime = 0,cpuUtili = processes.size(),not_idle = 0;

     for (auto& p : processes) {
        avgWaitingTime += p.waitingTime;
        avgTurnaroundTime += p.turnAroundTime;
        not_idle += p.CPUBurst;
    }

    cpuUtili = 1.0 * not_idle / timeEnding * 100.0;
    avgTurnaroundTime /= 1.0 * processesNum;
    avgWaitingTime /= 1.0 * processesNum;
    cout << "Average Waiting Time: " << avgWaitingTime << endl;
    cout << "Average Turnaround Time: " << avgTurnaroundTime << endl;
    cout<<"CPU Utilization Rate = " << cpuUtili << '%' << endl << endl;
}

pair<int, vector<Process>> RR(int quantum, vector<Process> processes) {
    int time = INT_MAX;
    for (const auto &process : processes) {
        time = min(time, process.arrivalTime);
    }
    time = INT_MAX ? 0 :time;
    deque<Process> currentProc(processes.begin(), processes.end());
    custom_sort(currentProc, time);
    vector<Process> finishedProcesses;

    cout << "Gantt Chart for RR Algorithm:" << endl;
    int lastEndTime = 0;

    if (time != 0) cout << "|0    ";

    vector<int> chartTimes;
    while (!currentProc.empty()) {
        Process currentProcess = currentProc.front();
        currentProc.pop_front();

        bool lastTime = currentProcess.remainingBurst <= quantum;
        int processTime = lastTime ? currentProcess.remainingBurst : quantum;

        int startTime = time;
        chartTimes.push_back(startTime);
        cout << "|" << "  P" << currentProcess.id << "  ";

        time += processTime;
        lastEndTime = time;
        currentProcess = processingInRR(time, currentProcess, processes, lastTime, quantum);

        if (currentProcess.remainingBurst <= 0) {// check if the proc end or no
            finishedProcesses.push_back(currentProcess);
        } else {
            // Move the process to the end of the queue
            currentProc.push_back(currentProcess);
        }
        custom_sort(currentProc, time);
    }
    cout<<"\n";
    for(int i=0;i<chartTimes.size();i++){
        cout<<chartTimes[i]<<"      ";
    }
    cout<<time<<"\n\n";

    for (const auto &process : currentProc) {
        finishedProcesses.push_back(process);
    }
    return {time, finishedProcesses};
}

void readFile(string filePath, vector<Process> &processes, int &processesNum, int &quantum) {
    ifstream fin(filePath);
    
    // Check if file was opened successfully
    if (!fin) {
        cerr << "Unable to open file: " << filePath << endl;
        return;
    }
    //-------->  read number of processes then  quantum
    fin >> processesNum >> quantum;
    int  id , arrivalTime , CPUBurst ;
    // takee the process info
    for (int i=0; i<processesNum; i++){
        fin >> id >> arrivalTime >> CPUBurst;
        Process process = Process(id, arrivalTime, CPUBurst);
        processes.push_back(process);
    } fin.close();
}

int main() {
    vector<Process> processes;
    int n, quantum;
    readFile("RR.txt", processes, n, quantum);
    
    cout << "**** Round Robin algorithm (RR) ****\n\n";
    pair<int, vector<Process>> result = RR(quantum, processes);
    processDet(result.second);
    avarages(result.second, result.first, n);
    return 0;
}
