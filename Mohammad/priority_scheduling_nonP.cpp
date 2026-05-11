#include <iostream>
#include <vector>
#include <iomanip>
#include <climits>
#include <string>

struct Process {
    std::string pid;
    int arrival;
    int burst;
    int priority;
    int orig_priority;
    int start_time  = -1;
    int finish_time = -1;
    bool done       = false;

    Process(std::string pid, int arrival, int burst, int priority)
        : pid(pid), arrival(arrival), burst(burst),
          priority(priority), orig_priority(priority) {}

    int waiting_time()    const { return finish_time - arrival - burst; }
    int turnaround_time() const { return finish_time - arrival; }
};

class PriorityScheduler {
public:
    struct GanttEntry {
        std::string pid;
        int start, end;
    };

    PriorityScheduler(std::vector<Process> processes,
                      int aging_interval = INT_MAX)
        : processes_(processes),
          aging_interval_(aging_interval) {}

    PriorityScheduler& run() {
        run_non_preemptive();
        return *this;
    }

    void print_results() const {
        std::cout << std::string(62, '-') << "\n";
        std::cout << std::left
                  << std::setw(6)  << "PID"
                  << std::setw(10) << "Arrival"
                  << std::setw(8)  << "Burst"
                  << std::setw(11) << "Priority"
                  << std::setw(8)  << "WT"
                  << std::setw(8)  << "TAT"
                  << std::setw(8)  << "Finish"
                  << "\n";
        std::cout << std::string(62, '-') << "\n";

        double total_wt = 0, total_tat = 0;
        for (const auto& p : processes_) {
            total_wt  += p.waiting_time();
            total_tat += p.turnaround_time();
            std::cout << std::left
                      << std::setw(6)  << p.pid
                      << std::setw(10) << p.arrival
                      << std::setw(8)  << p.burst
                      << std::setw(11) << p.orig_priority
                      << std::setw(8)  << p.waiting_time()
                      << std::setw(8)  << p.turnaround_time()
                      << std::setw(8)  << p.finish_time
                      << "\n";
        }

        int n = static_cast<int>(processes_.size());
        std::cout << std::string(62, '-') << "\n";
        std::cout << std::fixed << std::setprecision(2);
        std::cout << "Average Waiting Time   : " << total_wt  / n << "\n";
        std::cout << "Average Turnaround Time: " << total_tat / n << "\n";
    }

    void print_gantt() const {
        std::cout << "\nGantt Chart:\n";

        std::string top_bar, labels, timeline;
        int prev = 0;

        for (const auto& e : gantt_) {
            if (e.start > prev) {
                std::string idle = " IDLE ";
                top_bar  += "+" + std::string(idle.size(), '-');
                labels   += "|" + idle;
                timeline += std::to_string(e.start)
                            + std::string(idle.size() + 1
                              - std::to_string(e.start).size(), ' ');
            }
            std::string lbl = " " + e.pid + " ";
            top_bar  += "+" + std::string(lbl.size(), '-');
            labels   += "|" + lbl;
            timeline += std::to_string(e.end)
                        + std::string(lbl.size() + 1
                          - std::to_string(e.end).size(), ' ');
            prev = e.end;
        }
        top_bar += "+";
        labels  += "|";

        std::cout << top_bar  << "\n"
                  << labels   << "\n"
                  << top_bar  << "\n"
                  << "0  " << timeline << "\n\n";
    }

private:
    std::vector<Process> processes_;
    int aging_interval_;
    std::vector<GanttEntry> gantt_;

    Process* pick_best(int current_time) {
        Process* best = nullptr;
        for (auto& p : processes_) {
            if (p.done || p.arrival > current_time) continue;
            if (best == nullptr
                || p.priority < best->priority
                || (p.priority == best->priority && p.arrival < best->arrival))
                best = &p;
        }
        return best;
    }

    void apply_aging(int current_time) {
        for (auto& p : processes_) {
            if (p.done || p.arrival > current_time) continue;
            if (p.priority > 0) p.priority--;
        }
    }

    void run_non_preemptive() {
        int current_time = 0;
        int last_aged    = 0;
        int done_count   = 0;
        int n = static_cast<int>(processes_.size());

        while (done_count < n) {
            if (aging_interval_ != INT_MAX
                && current_time - last_aged >= aging_interval_) {
                apply_aging(current_time);
                last_aged = current_time;
            }

            Process* chosen = pick_best(current_time);
            if (!chosen) { current_time++; continue; }

            chosen->start_time  = current_time;
            chosen->finish_time = current_time + chosen->burst;
            gantt_.push_back({ chosen->pid, current_time, chosen->finish_time });
            current_time = chosen->finish_time;
            chosen->done = true;
            done_count++;
        }
    }
};

int main() {
    std::vector<Process> processes = {
        { "P1", 0, 10, 3 },
        { "P2", 2,  4, 1 },
        { "P3", 4,  6, 2 },
        { "P4", 6,  8, 4 },
    };

    std::cout << "=== Non-Preemptive Priority Scheduling ===\n";
    PriorityScheduler scheduler(processes, 10);
    scheduler.run().print_results();
    scheduler.print_gantt();

    return 0;
}
