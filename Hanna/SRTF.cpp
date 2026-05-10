#include <iostream>
#include <queue>
#include <vector>
#include <algorithm>
using namespace std;

class Process
{
private:
    int id;
    int arrivalTime;
    int burstTime;
    int remainingTime;
    int completionTime;
    int waitingTime;
    int turnAroundTime;

public:
    Process(int id, int arrivalTime, int burstTime)
    {
        this->id = id;
        this->arrivalTime = arrivalTime;
        this->burstTime = burstTime;
        remainingTime = burstTime;
    }
    int getId() const { return id; }
    int getArrivalTime() const { return arrivalTime; }
    int getBurstTime() const { return burstTime; }
    int getRemainingTime() const { return remainingTime; }
    int getCompletionTime() const { return completionTime; }
    int getWaitingTime() const { return waitingTime; }
    int getTurnaroundTime() const { return turnAroundTime; }

    void setRemainTime() { remainingTime--; }
    void setCompletionTime(int CompletionTime) { this->completionTime = CompletionTime; }
    void setTurnaroundTime(int turnaroundTime) { this->turnAroundTime = turnaroundTime; }
    void setWaitingTime(int waitingTime) { this->waitingTime = waitingTime; }
};

class ProcessComparator
{
public:
    bool operator()(const Process *p1, const Process *p2)
    {
        return p1->getRemainingTime() > p2->getRemainingTime();
    }
};

class SRTF
{
public:
    void runSRTF(vector<Process *> processesQueue, vector<Process *> &finishResult)
    {
        priority_queue<Process *, vector<Process *>, ProcessComparator> readyQueue;
        int currTime = 0;
        int completeProcesses = 0;
        int arrivalIndex = 0;
        while (true)
        {
            if (completeProcesses == processesQueue.size())
                break;
            while (arrivalIndex < processesQueue.size() && processesQueue[arrivalIndex]->getArrivalTime() == currTime)
            {
                readyQueue.push(processesQueue[arrivalIndex]);
                arrivalIndex++;
            }

            if (!readyQueue.empty())
            {
                Process *currProcess = readyQueue.top();
                readyQueue.pop();
                currProcess->setRemainTime();
                if (currProcess->getRemainingTime() == 0)
                {
                    completeProcesses++;
                    currProcess->setCompletionTime(currTime + 1);
                    currProcess->setTurnaroundTime(currProcess->getCompletionTime() - currProcess->getArrivalTime());
                    currProcess->setWaitingTime(currProcess->getTurnaroundTime() - currProcess->getBurstTime());
                    finishResult.push_back(currProcess);
                }
                else
                {
                    readyQueue.push(currProcess);
                }
            }
            currTime++;
        }
    }
};

int main()
{
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
        processesQueue.push_back(p);
    }

    sort(processesQueue.begin(), processesQueue.end(),
         [](Process *a, Process *b)
         {
             return a->getArrivalTime() < b->getArrivalTime();
         });

    SRTF srtf;
    srtf.runSRTF(processesQueue, result);
    int totalWaitingTime = 0;
    int totalTurnAroundTime = 0;
    cout << "-----------------------------------------------------------------------" << endl;
    cout << "Process ID | Arrival Time | Burst Time | Waiting Time | Turnaround Time" << endl;
    cout << "-----------------------------------------------------------------------" << endl;
    for (auto p : result)
    {
        cout << "Process " << p->getId() << " |         " << p->getArrivalTime() << "    |    " << p->getBurstTime() << "    |    " << p->getWaitingTime() << "    |    " << p->getTurnaroundTime() << endl;
        totalWaitingTime += p->getWaitingTime();
        totalTurnAroundTime += p->getTurnaroundTime();
    }
    cout << "===============================================" << endl;
    cout << "Average Waiting Time: " << (double)totalWaitingTime / numProcesses << endl;
    cout << "Average Turnaround Time: " << (double)totalTurnAroundTime / numProcesses << endl;
    for (auto p : processesQueue)
        delete p;
    return 0;
}