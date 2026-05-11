#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <iomanip>
#include <string>
#include <fstream>

using namespace std;

struct Process {
    int id;
    int arrivalTime;
    int burstTime;
    int priority;

    int remainingTime;
    int completionTime = 0;
    int waitingTime = 0;
    int turnaroundTime = 0;
    bool completed = false;
};

struct GanttSegment {
    string name;
    int start;
    int end;
};

struct AlgorithmResult {
    string algorithmName;
    vector<Process> processes;
    vector<GanttSegment> gantt;
    double averageWaitingTime = 0;
    double averageTurnaroundTime = 0;
};

bool arrivalOrder(const Process& a, const Process& b) {
    if (a.arrivalTime != b.arrivalTime)
        return a.arrivalTime < b.arrivalTime;
    return a.id < b.id;
}

void addSegment(vector<GanttSegment>& gantt, const string& name, int start, int end) {
    if (start == end)
        return;

    // Merge consecutive equal segments to make the Gantt chart cleaner.
    if (!gantt.empty() && gantt.back().name == name && gantt.back().end == start) {
        gantt.back().end = end;
    } else {
        gantt.push_back({name, start, end});
    }
}

void calculateAverages(AlgorithmResult& result) {
    double totalWaiting = 0;
    double totalTurnaround = 0;

    for (const auto& p : result.processes) {
        totalWaiting += p.waitingTime;
        totalTurnaround += p.turnaroundTime;
    }

    int n = (int)result.processes.size();
    result.averageWaitingTime = totalWaiting / n;
    result.averageTurnaroundTime = totalTurnaround / n;
}

void printGanttChart(const vector<GanttSegment>& gantt) {
    cout << "\nGantt Chart:\n";

    for (int i = 0; i < (int)gantt.size(); i++) {
        cout << "+----------";
    }
    cout << "+\n";

    for (const auto& segment : gantt) {
        cout << "|" << setw(10) << left << segment.name;
    }
    cout << "|\n";

    for (int i = 0; i < (int)gantt.size(); i++) {
        cout << "+----------";
    }
    cout << "+\n";

    if (!gantt.empty()) {
        cout << gantt[0].start;
        for (const auto& segment : gantt) {
            cout << setw(10) << right << segment.end;
        }
        cout << "\n";
    }
}

void printAlgorithmResult(const AlgorithmResult& result) {
    cout << "\n============================================================\n";
    cout << result.algorithmName << "\n";
    cout << "============================================================\n";

    printGanttChart(result.gantt);

    vector<Process> sortedProcesses = result.processes;
    sort(sortedProcesses.begin(), sortedProcesses.end(), [](const Process& a, const Process& b) {
        return a.id < b.id;
    });

    cout << "\n";
    cout << left
         << setw(10) << "Process"
         << setw(10) << "Arrival"
         << setw(10) << "Burst"
         << setw(10) << "Priority"
         << setw(12) << "Complete"
         << setw(10) << "Waiting"
         << setw(12) << "Turnaround"
         << "\n";

    cout << string(74, '-') << "\n";

    for (const auto& p : sortedProcesses) {
        cout << left
             << setw(10) << ("P" + to_string(p.id))
             << setw(10) << p.arrivalTime
             << setw(10) << p.burstTime
             << setw(10) << p.priority
             << setw(12) << p.completionTime
             << setw(10) << p.waitingTime
             << setw(12) << p.turnaroundTime
             << "\n";
    }

    cout << fixed << setprecision(2);
    cout << "\nAverage Waiting Time   : " << result.averageWaitingTime << "\n";
    cout << "Average Turnaround Time: " << result.averageTurnaroundTime << "\n";
}

AlgorithmResult runFCFS(vector<Process> processes) {
    AlgorithmResult result;
    result.algorithmName = "First-Come First-Served (FCFS)";

    sort(processes.begin(), processes.end(), arrivalOrder);

    int currentTime = 0;
    for (auto& p : processes) {
        if (currentTime < p.arrivalTime) {
            addSegment(result.gantt, "IDLE", currentTime, p.arrivalTime);
            currentTime = p.arrivalTime;
        }

        int startTime = currentTime;
        currentTime += p.burstTime;

        p.completionTime = currentTime;
        p.turnaroundTime = p.completionTime - p.arrivalTime;
        p.waitingTime = p.turnaroundTime - p.burstTime;
        p.remainingTime = 0;
        p.completed = true;

        addSegment(result.gantt, "P" + to_string(p.id), startTime, currentTime);
    }

    result.processes = processes;
    calculateAverages(result);
    return result;
}

AlgorithmResult runSJF(vector<Process> processes) {
    AlgorithmResult result;
    result.algorithmName = "Shortest Job First (SJF) - Non-Preemptive";

    int n = (int)processes.size();
    int currentTime = 0;
    int completed = 0;

    while (completed < n) {
        int bestIndex = -1;

        for (int i = 0; i < n; i++) {
            if (processes[i].completed || processes[i].arrivalTime > currentTime)
                continue;

            if (bestIndex == -1 ||
                processes[i].burstTime < processes[bestIndex].burstTime ||
                (processes[i].burstTime == processes[bestIndex].burstTime && processes[i].arrivalTime < processes[bestIndex].arrivalTime) ||
                (processes[i].burstTime == processes[bestIndex].burstTime && processes[i].arrivalTime == processes[bestIndex].arrivalTime && processes[i].id < processes[bestIndex].id)) {
                bestIndex = i;
            }
        }

        if (bestIndex == -1) {
            int nextArrival = -1;
            for (int i = 0; i < n; i++) {
                if (!processes[i].completed) {
                    if (nextArrival == -1 || processes[i].arrivalTime < nextArrival)
                        nextArrival = processes[i].arrivalTime;
                }
            }
            addSegment(result.gantt, "IDLE", currentTime, nextArrival);
            currentTime = nextArrival;
            continue;
        }

        Process& p = processes[bestIndex];
        int startTime = currentTime;
        currentTime += p.burstTime;

        p.completionTime = currentTime;
        p.turnaroundTime = p.completionTime - p.arrivalTime;
        p.waitingTime = p.turnaroundTime - p.burstTime;
        p.remainingTime = 0;
        p.completed = true;
        completed++;

        addSegment(result.gantt, "P" + to_string(p.id), startTime, currentTime);
    }

    result.processes = processes;
    calculateAverages(result);
    return result;
}

AlgorithmResult runSRTF(vector<Process> processes) {
    AlgorithmResult result;
    result.algorithmName = "Shortest Remaining Time First (SRTF) - Preemptive";

    int n = (int)processes.size();
    int currentTime = 0;
    int completed = 0;

    for (auto& p : processes) {
        p.remainingTime = p.burstTime;
        p.completed = false;
    }

    while (completed < n) {
        int bestIndex = -1;

        for (int i = 0; i < n; i++) {
            if (processes[i].completed || processes[i].arrivalTime > currentTime)
                continue;

            if (bestIndex == -1 ||
                processes[i].remainingTime < processes[bestIndex].remainingTime ||
                (processes[i].remainingTime == processes[bestIndex].remainingTime && processes[i].arrivalTime < processes[bestIndex].arrivalTime) ||
                (processes[i].remainingTime == processes[bestIndex].remainingTime && processes[i].arrivalTime == processes[bestIndex].arrivalTime && processes[i].id < processes[bestIndex].id)) {
                bestIndex = i;
            }
        }

        if (bestIndex == -1) {
            addSegment(result.gantt, "IDLE", currentTime, currentTime + 1);
            currentTime++;
            continue;
        }

        Process& p = processes[bestIndex];
        addSegment(result.gantt, "P" + to_string(p.id), currentTime, currentTime + 1);

        p.remainingTime--;
        currentTime++;

        if (p.remainingTime == 0) {
            p.completed = true;
            p.completionTime = currentTime;
            p.turnaroundTime = p.completionTime - p.arrivalTime;
            p.waitingTime = p.turnaroundTime - p.burstTime;
            completed++;
        }
    }

    result.processes = processes;
    calculateAverages(result);
    return result;
}

AlgorithmResult runRoundRobin(vector<Process> processes, int timeQuantum) {
    AlgorithmResult result;
    result.algorithmName = "Round Robin (RR), Quantum = " + to_string(timeQuantum);

    sort(processes.begin(), processes.end(), arrivalOrder);

    int n = (int)processes.size();
    int currentTime = 0;
    int completed = 0;
    int nextProcess = 0;

    for (auto& p : processes) {
        p.remainingTime = p.burstTime;
        p.completed = false;
    }

    queue<int> readyQueue;

    while (completed < n) {
        while (nextProcess < n && processes[nextProcess].arrivalTime <= currentTime) {
            readyQueue.push(nextProcess);
            nextProcess++;
        }

        if (readyQueue.empty()) {
            int nextArrival = processes[nextProcess].arrivalTime;
            addSegment(result.gantt, "IDLE", currentTime, nextArrival);
            currentTime = nextArrival;
            continue;
        }

        int index = readyQueue.front();
        readyQueue.pop();

        Process& p = processes[index];
        int executionTime = min(timeQuantum, p.remainingTime);
        int startTime = currentTime;

        for (int i = 0; i < executionTime; i++) {
            currentTime++;
            p.remainingTime--;

            while (nextProcess < n && processes[nextProcess].arrivalTime <= currentTime) {
                readyQueue.push(nextProcess);
                nextProcess++;
            }
        }

        addSegment(result.gantt, "P" + to_string(p.id), startTime, currentTime);

        if (p.remainingTime == 0) {
            p.completed = true;
            p.completionTime = currentTime;
            p.turnaroundTime = p.completionTime - p.arrivalTime;
            p.waitingTime = p.turnaroundTime - p.burstTime;
            completed++;
        } else {
            readyQueue.push(index);
        }
    }

    result.processes = processes;
    calculateAverages(result);
    return result;
}

AlgorithmResult runPriorityNonPreemptive(vector<Process> processes) {
    AlgorithmResult result;
    result.algorithmName = "Priority Scheduling - Non-Preemptive";

    int n = (int)processes.size();
    int currentTime = 0;
    int completed = 0;

    while (completed < n) {
        int bestIndex = -1;

        for (int i = 0; i < n; i++) {
            if (processes[i].completed || processes[i].arrivalTime > currentTime)
                continue;

            // Smaller priority number means higher priority.
            if (bestIndex == -1 ||
                processes[i].priority < processes[bestIndex].priority ||
                (processes[i].priority == processes[bestIndex].priority && processes[i].arrivalTime < processes[bestIndex].arrivalTime) ||
                (processes[i].priority == processes[bestIndex].priority && processes[i].arrivalTime == processes[bestIndex].arrivalTime && processes[i].id < processes[bestIndex].id)) {
                bestIndex = i;
            }
        }

        if (bestIndex == -1) {
            int nextArrival = -1;
            for (int i = 0; i < n; i++) {
                if (!processes[i].completed) {
                    if (nextArrival == -1 || processes[i].arrivalTime < nextArrival)
                        nextArrival = processes[i].arrivalTime;
                }
            }
            addSegment(result.gantt, "IDLE", currentTime, nextArrival);
            currentTime = nextArrival;
            continue;
        }

        Process& p = processes[bestIndex];
        int startTime = currentTime;
        currentTime += p.burstTime;

        p.completionTime = currentTime;
        p.turnaroundTime = p.completionTime - p.arrivalTime;
        p.waitingTime = p.turnaroundTime - p.burstTime;
        p.remainingTime = 0;
        p.completed = true;
        completed++;

        addSegment(result.gantt, "P" + to_string(p.id), startTime, currentTime);
    }

    result.processes = processes;
    calculateAverages(result);
    return result;
}

void printComparisonTable(const vector<AlgorithmResult>& results) {
    cout << "\n\n==================== Final Comparison ====================\n";
    cout << left
         << setw(58) << "Algorithm"
         << setw(20) << "Avg Waiting"
         << setw(20) << "Avg Turnaround"
         << "\n";
    cout << string(98, '-') << "\n";

    cout << fixed << setprecision(2);
    for (const auto& result : results) {
        cout << left
             << setw(58) << result.algorithmName
             << setw(20) << result.averageWaitingTime
             << setw(20) << result.averageTurnaroundTime
             << "\n";
    }
}



int main() {
    int numberOfProcesses;
    cout << "Enter number of processes: ";
    cin >> numberOfProcesses;

    vector<Process> originalProcesses(numberOfProcesses);

    cout << "\nEnter process data as: ID Arrival Burst Priority\n";
    cout << "Note: smaller priority number means higher priority.\n\n";

    for (int i = 0; i < numberOfProcesses; i++) {
        cout << "Process " << i + 1 << ": ";
        cin >> originalProcesses[i].id
            >> originalProcesses[i].arrivalTime
            >> originalProcesses[i].burstTime
            >> originalProcesses[i].priority;

        originalProcesses[i].remainingTime = originalProcesses[i].burstTime;
    }

    int timeQuantum;
    cout << "\nEnter time quantum for Round Robin: ";
    cin >> timeQuantum;

    vector<AlgorithmResult> results;

    results.push_back(runFCFS(originalProcesses));
    results.push_back(runSJF(originalProcesses));
    results.push_back(runSRTF(originalProcesses));
    results.push_back(runRoundRobin(originalProcesses, timeQuantum));
    results.push_back(runPriorityNonPreemptive(originalProcesses));
   // results.push_back(runPriorityPreemptive(originalProcesses));

    for (const auto& result : results) {
        printAlgorithmResult(result);
    }

    printComparisonTable(results);
   // writeComparisonCSV(results);

   // cout << "\nA file named comparison_results.csv was generated in the same folder.\n";
   // cout << "You can open it in Excel or Google Sheets and create bar charts from it.\n";

    return 0;
}
