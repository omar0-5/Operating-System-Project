#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>

using namespace std;

struct Process {
    int id;
    int arrivalTime;
    int burstTime;
    int completionTime;
    int turnaroundTime;
    int waitingTime;
    bool isCompleted = false;
};

void calculateSJF(vector<Process>& processes) {
    int n = processes.size();
    int currentTime = 0;
    int completed = 0;

    while (completed != n) {
        int idx = -1;
        int minBurst = 1e9;

        // Selection logic: Find process with shortest burst among those arrived 
        for (int i = 0; i < n; i++) {
            if (processes[i].arrivalTime <= currentTime && !processes[i].isCompleted) {
                if (processes[i].burstTime < minBurst) {
                    minBurst = processes[i].burstTime;
                    idx = i;
                }
                // Tie-breaker: If bursts are equal, use FCFS (earlier arrival) 
                else if (processes[i].burstTime == minBurst) {
                    if (idx != -1 && processes[i].arrivalTime < processes[idx].arrivalTime) {
                        idx = i;
                    }
                }
            }
        }

        if (idx != -1) {
            // Non-preemptive: Process runs to completion once started 
            processes[idx].completionTime = currentTime + processes[idx].burstTime;
            // Formula: Turnaround Time = Completion - Arrival 
            processes[idx].turnaroundTime = processes[idx].completionTime - processes[idx].arrivalTime;
            // Formula: Waiting Time = Turnaround - Burst 
            processes[idx].waitingTime = processes[idx].turnaroundTime - processes[idx].burstTime;
            processes[idx].isCompleted = true;
            
            currentTime = processes[idx].completionTime;
            completed++;
        } else {
            // If no process has arrived, CPU remains idle 
            currentTime++;
        }
    }
}

int main() {
    int n;
    cout << "Enter number of processes: ";
    cin >> n;

    vector<Process> processes(n);
    for (int i = 0; i < n; i++) {
        processes[i].id = i + 1;
        cout << "Enter Arrival and Burst Time for P" << processes[i].id << ": ";
        cin >> processes[i].arrivalTime >> processes[i].burstTime;
    }

    calculateSJF(processes);

    cout << "\nProcess\tArrival\tBurst\tCompletion\tWaiting\tTurnaround\n";
    float totalWT = 0, totalTAT = 0;
    for (auto &p : processes) {
        totalWT += p.waitingTime;
        totalTAT += p.turnaroundTime;
        cout << "P" << p.id << "\t" << p.arrivalTime << "\t" << p.burstTime << "\t" 
             << p.completionTime << "\t\t" << p.waitingTime << "\t" << p.turnaroundTime << endl;
    }

    cout << fixed << setprecision(2);
    cout << "\nAverage Waiting Time: " << totalWT / n << " ms" << endl;
    cout << "Average Turnaround Time: " << totalTAT / n << " ms" << endl;

    return 0;
}