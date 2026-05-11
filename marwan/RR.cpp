#include <iostream>
#include <queue>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <string>

using namespace std;

class Process
{
private:
    int id;
    int arrivalTime;
    int burseTime;
    int remaningTime;
    int completionTime;
    int waitingTime;
    int turnAroundTime;

public:
    Process(int id, int arrivalTime, int burseTime)
    {
        this->id = id;
        this->arrivalTime = arrivalTime;
        this->burseTime = burseTime;
        remaningTime = burseTime;
    }
    int getId() const { return id; }
    int getArrivalTime() const { return arrivalTime; }
    int getBurstTime() const { return burseTime; }
    int getRemainingTime() const { return remaningTime; }
    int getCompletionTime() const { return completionTime; }
    int getWaitingTime() const { return waitingTime; }
    int getTurnaroundTime() const { return turnAroundTime; }

    void setRemainTime() { remaningTime--; }
    void setCompletionTime(int CompletionTime) { this->completionTime = CompletionTime; }
    void setTurnaroundTime(int turnaroundTime) { this->turnAroundTime = turnaroundTime; }
    void setWaitingTime(int waitingTime) { this->waitingTime = waitingTime; }
};

class RoundRobin {
public:
    void runRR(vector<Process*> processesQueue, vector<Process*>& finishResult, vector<pair<string, pair<int, int>>>& ganttSegments, int timeQuantum) {
        queue<Process*> readyQueue;
        int currTime = 0;
        int completeProcesses = 0;
        int arrivalIndex = 0;
        int n = processesQueue.size();

      
        sort(processesQueue.begin(), processesQueue.end(), [](Process* a, Process* b) {
            return a->getArrivalTime() < b->getArrivalTime();
        });

        while (completeProcesses < n) {
         
            while (arrivalIndex < n && processesQueue[arrivalIndex]->getArrivalTime() <= currTime) {
                readyQueue.push(processesQueue[arrivalIndex]);
                arrivalIndex++;
            }

            if (readyQueue.empty()) {
                if (arrivalIndex < n) {
                    ganttSegments.push_back({"IDLE", {currTime, processesQueue[arrivalIndex]->getArrivalTime()}});
                    currTime = processesQueue[arrivalIndex]->getArrivalTime();
                }
                continue;
            }

            Process* currProcess = readyQueue.front();
            readyQueue.pop();

            int startTime = currTime;
            int executionTime = min(currProcess->getRemainingTime(), timeQuantum);
            
            for (int i = 0; i < executionTime; i++) {
                currTime++;
                currProcess->setRemainTime(); 

                while (arrivalIndex < n && processesQueue[arrivalIndex]->getArrivalTime() == currTime) {
                    readyQueue.push(processesQueue[arrivalIndex]);
                    arrivalIndex++;
                }
            }

            
            ganttSegments.push_back({"P" + to_string(currProcess->getId()), {startTime, currTime}});

            if (currProcess->getRemainingTime() == 0) {
                completeProcesses++;
                currProcess->setCompletionTime(currTime);
                currProcess->setTurnaroundTime(currProcess->getCompletionTime() - currProcess->getArrivalTime());
                currProcess->setWaitingTime(currProcess->getTurnaroundTime() - currProcess->getBurstTime());
                finishResult.push_back(currProcess);
            } else {
                readyQueue.push(currProcess);
            }
        }
    }
};

// this function (printGanttChart) was implemented using AI enhanced tools for learning purposes.
// It is only used for displaying the Gantt chart output and does not perform any scheduling calculations or computations.
void printGanttChart(const vector<pair<string, pair<int, int>>>& segments)
{
    cout << "\nGantt Chart:\n";

    cout << " ";
    for (int i = 0; i < segments.size(); i++)
    {
        cout << "+----------";
    }
    cout << "+\n";

    cout << " ";
    for (auto segment : segments)
    {
        cout << "|  " << setw(6) << left << segment.first << "  ";
    }
    cout << "|\n";

    cout << " ";
    for (int i = 0; i < segments.size(); i++)
    {
        cout << "+----------";
    }
    cout << "+\n";

    cout << " ";
    for (auto segment : segments)
    {
        cout << setw(11) << left << segment.second.first;
    }

    if (!segments.empty())
    {
        cout << segments.back().second.second;
    }
    cout << endl;
}

int main()
{
    vector<Process *> processesQueue;
    vector<Process *> result;
    vector<pair<string, pair<int, int>>> ganttSegments;

    int numProcesses, timeQuantum;

    cout << "Enter the number of the processes: ";
    cin >> numProcesses;

    for (int i = 0; i < numProcesses; i++)
    {
        int id, arrivalTime, burstTime;
        cout << "Enter the id of the process(" << i + 1 << "): ";
        cin >> id;
        cout << "Enter the arrival time of the process(" << i + 1 << "): ";
        cin >> arrivalTime;
        cout << "Enter the burst time of the process(" << i + 1 << "): ";
        cin >> burstTime;

        processesQueue.push_back(new Process(id, arrivalTime, burstTime));
    }

    cout << "Enter Time Quantum: ";
    cin >> timeQuantum;

    RoundRobin rr;
    rr.runRR(processesQueue, result, ganttSegments, timeQuantum);

    printGanttChart(ganttSegments);

    int totalWaitingTime = 0;
    int totalTurnAroundTime = 0;

    cout << "\nProcess ID | Arrival Time | Burst Time | Waiting Time | Turnaround Time" << endl;

    
    sort(result.begin(), result.end(), [](Process* a, Process* b) {
        return a->getId() < b->getId();
    });

    for (auto p : result)
    {
        cout << "Process " << setw(2) << p->getId()
             << " |         " << setw(4) << p->getArrivalTime()
             << "     |    " << setw(4) << p->getBurstTime()
             << "    |    " << setw(4) << p->getWaitingTime()
             << "    |    " << setw(4) << p->getTurnaroundTime()
             << endl;

        totalWaitingTime += p->getWaitingTime();
        totalTurnAroundTime += p->getTurnaroundTime();
    }

    cout << "----------------------------------------------------------------------" << endl;
    cout << fixed << setprecision(2);
    cout << "Average Waiting Time: " << (double)totalWaitingTime / numProcesses << endl;
    cout << "Average Turnaround Time: " << (double)totalTurnAroundTime / numProcesses << endl;

    return 0;
}
