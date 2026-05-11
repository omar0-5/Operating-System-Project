#include <iostream>
#include <queue>
#include <vector>
#include <algorithm>
#include <iomanip>
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

class FCFS
{
public:
    void runFCFS(vector<Process *> processesQueue, vector<Process *> &finishResult)
    {
        int currTime = 0;

        for (auto currProcess : processesQueue)
        {
            if (currTime < currProcess->getArrivalTime())
            {
                currTime = currProcess->getArrivalTime();
            }

            currTime += currProcess->getBurstTime();

            currProcess->setCompletionTime(currTime);

            currProcess->setTurnaroundTime(
                currProcess->getCompletionTime() - currProcess->getArrivalTime()
            );

            currProcess->setWaitingTime(
                currProcess->getTurnaroundTime() - currProcess->getBurstTime()
            );

            finishResult.push_back(currProcess);
        }
    }
};

//this function (printGanttChart) was implemented using AI inhanced tools for learing perposes
void printGanttChart(vector<Process *> result)
{
    cout << "\nGantt Chart:\n";

    vector<pair<string, pair<int, int>>> segments;

    int clock = 0;

    for (auto p : result)
    {
        if (clock < p->getArrivalTime())
        {
            segments.push_back({"IDLE", {clock, p->getArrivalTime()}});
            clock = p->getArrivalTime();
        }

        string processName = "P" + to_string(p->getId());

        segments.push_back({processName, {clock, p->getCompletionTime()}});

        clock = p->getCompletionTime();
    }

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
    vector<pair<int, Process *>> processPairs;
    vector<Process *> processesQueue;
    vector<Process *> result;

    int numProcesses;

    cout << "Enter the number of the processes: ";
    cin >> numProcesses;

    int id, arrivalTime, burstTime;

    for (int i = 0; i < numProcesses; i++)
    {
        cout << "Enter the id of the process(" << i + 1 << "): ";
        cin >> id;

        cout << "Enter the arrival time of the process(" << i + 1 << "): ";
        cin >> arrivalTime;

        cout << "Enter the burst time of the process(" << i + 1 << "): ";
        cin >> burstTime;

        Process *p = new Process(id, arrivalTime, burstTime);

        processPairs.push_back({arrivalTime, p});
    }

    sort(processPairs.begin(), processPairs.end());

    for (auto processPair : processPairs)
    {
        processesQueue.push_back(processPair.second);
    }

    FCFS fcfs;
    fcfs.runFCFS(processesQueue, result);

    printGanttChart(result);

    int totalWaitingTime = 0;
    int totalTurnAroundTime = 0;

    cout << "\nProcess ID | Arrival Time | Burst Time | Waiting Time | Turnaround Time" << endl;

    for (auto p : result)
    {
        cout << "Process " << p->getId()
             << " |         " << p->getArrivalTime()
             << "    |    " << p->getBurstTime()
             << "    |    " << p->getWaitingTime()
             << "    |    " << p->getTurnaroundTime()
             << endl;

        totalWaitingTime += p->getWaitingTime();
        totalTurnAroundTime += p->getTurnaroundTime();
    }

    cout << "----------------------------------------------" << endl;
    cout << "Average Waiting Time: " << (double)totalWaitingTime / numProcesses << endl;
    cout << "Average Turnaround Time: " << (double)totalTurnAroundTime / numProcesses << endl;

    return 0;
}
