    //First-Come First-Served (FCFS) CPU Scheduling Algorithm (Non-Preemptive) - Faris Asaad
    
    #include <iostream>
    #include <iomanip>
    #include <vector>
    #include <algorithm>
    #include <string>
    
    using namespace std;
    
    struct Process {
        string id;           // Process identifier
        int    arrivalTime;  // Time the process enters the ready queue
        int    burstTime;    // CPU time the process needs to complete
        int    completionTime;  // Filled in by the scheduler
        int    turnaroundTime;  // Filled in by the scheduler
        int    waitingTime;     // Filled in by the scheduler
    };
    
    //FCFS Scheduler
    void fcfsSchedule(vector<Process>& processes) {
        //Sort processes by arrival time
        sort(processes.begin(), processes.end(), [](const Process& a, const Process& b) {
            return a.arrivalTime < b.arrivalTime;
        });
    
        int currentTime = 0;
    
        //Process each job in arrival order
        for (auto& p : processes) {
            //If CPU is idle before this process arrives, jump forward
            if (currentTime < p.arrivalTime) {
                currentTime = p.arrivalTime;
            }
            //Run the process to completion
            currentTime += p.burstTime;
            p.completionTime = currentTime;
            p.turnaroundTime = p.completionTime - p.arrivalTime;
            p.waitingTime = p.turnaroundTime - p.burstTime;
        }
    }
    
    //Print a Gantt Chart (Not required (OPTIONAL) but for better visualization)
    void printGanttChart(const vector<Process>& processes) {
        struct Segment {
            string label;
            int start;
            int end;
        };
        vector<Segment> segments;
    
        int clock = 0;
        for (const auto& p : processes) {
            if (clock < p.arrivalTime) {
                segments.push_back({"IDLE", clock, p.arrivalTime});
                clock = p.arrivalTime;
            }
            segments.push_back({p.id, clock, p.completionTime});
            clock = p.completionTime;
        }
    
        cout << "\nGantt Chart:\n";
    
        cout << " ";
        for (const auto& s : segments) cout << "+----------";
        cout << "+\n";
    
        cout << " ";
        for (const auto& s : segments) cout << "|  " << setw(6) << left << s.label << "  ";
        cout << "|\n";
    
        cout << " ";
        for (const auto& s : segments) cout << "+----------";
        cout << "+\n";
    
        cout << " ";
        for (const auto& s : segments) cout << setw(11) << left << s.start;
        cout << segments.back().end << "\n";
    }
    
    //Print Results Table
    void printResults(const vector<Process>& processes) {
        cout << "\n--- FCFS Scheduling Results ---\n";
        cout << "Process\t\tArrival Time\tBurst Time\tCompletion Time\tTurnaround Time\tWaiting Time\n";
        cout << string(84, '-') << "\n";
    
        double totalTAT = 0, totalWT = 0;
    
        for (const auto& p : processes) {
            cout << p.id << "\t\t" << p.arrivalTime << "\t\t" << p.burstTime << "\t\t"
                << p.completionTime << "\t\t" << p.turnaroundTime << "\t\t" << p.waitingTime << "\n";
            totalTAT += p.turnaroundTime;
            totalWT  += p.waitingTime;
        }
        int n = processes.size();
        cout << fixed << setprecision(2);
        cout << "\nAverage Turnaround Time : " << totalTAT / n << " ms\n";
        cout << "Average Waiting Time    : " << totalWT  / n << " ms\n";
    }
    
    //test case function 
    void runTestCase(int caseNum, const string& description, vector<Process> processes) {
        cout << " Test Case " << caseNum << ": " << description << "\n";
        fcfsSchedule(processes);
        printGanttChart(processes);
        printResults(processes);
    }
    
    int main() {
        // cout << "  First-Come First-Served (FCFS) CPU Scheduling Algorithm  \n";
        cout << "Hello user, if you want to use ur own input for testing the algorothim please enter 1, else (Hardcoded test cases) enter 2: " << endl;
    
        int choice;
        cin >> choice;
    
        if (choice == 1) {
            int n;
            cout << "Enter the number of processes: ";
            cin >> n;
    
            vector<Process> userProcesses(n);
            for (int i = 0; i < n; ++i) {
                cout << "Enter Process ID, Arrival Time, and Burst Time for Process (in order) " << (i + 1) << ": ";
                cin >> userProcesses[i].id >> userProcesses[i].arrivalTime >> userProcesses[i].burstTime;
            }
    
            runTestCase(0, "User Input Test Case", userProcesses);
    
        } else if (choice == 2) {
    
            // Test Case 1
            {
                vector<Process> tc1 = {
                    {"P1", 0, 8}, {"P2", 1, 4}, {"P3", 2, 9}, {"P4", 3, 5}, {"P5", 4, 2},
                };
                runTestCase(1, "General Mixed Case", tc1);
            }
            // Test Case 2
            {
                vector<Process> tc2 = {
                    {"P1", 0, 20}, {"P2", 1, 3}, {"P3", 2, 3}, {"P4", 3, 3},
                };
                runTestCase(2, "Convoy Effect", tc2);
            }
            // Test Case 3
            {
                vector<Process> tc3 = {
                    {"P1", 0, 5}, {"P2", 8, 3}, {"P3", 12, 7}, {"P4", 20, 4},
                };
                runTestCase(3, "CPU Idle Gaps", tc3);
            }
    
        } else {
            cout << "Invalid choice. Please run the program again and either enter 1 or 2 \n";
        }
    
        cout << "End of FCFS Simulation\n";
        return 0;
    }