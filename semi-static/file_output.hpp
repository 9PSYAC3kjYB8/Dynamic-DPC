#include "data.hpp"
#include <chrono>
#include <unistd.h>


// frequency of output
const unsigned int freq = 10000;

// max update count
const unsigned int max_update_count = 2000000;


// variable for time measure
std::chrono::system_clock::time_point start, end, init;

// computation time
double local_density_update_time = 0;
double dependent_point_update_time = 0;
double update_time_total = 0;

// peak memory
double memory = 0;


// compute memory usage
double process_mem_usage() {

    double resident_set = 0.0;

    // the two fields we want
    unsigned long vsize;
    long rss;
    {
        std::string ignore;
        std::ifstream ifs("/proc/self/stat", std::ios_base::in);
        ifs >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore
            >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore
            >> ignore >> ignore >> vsize >> rss;
    }

    long page_size_kb = sysconf(_SC_PAGE_SIZE) / 1024; // in case x86-64 is configured to use 2MB pages
    resident_set = rss * page_size_kb;

	return resident_set / 1000;
}

// result output
void output_result(const unsigned int counter, const bool flag) {

    std::string f_name = "result/";
    if (dataset_id == 1) f_name += "1-household/";
	if (dataset_id == 2) f_name += "2-pamap2/";
	if (dataset_id == 3) f_name += "3-gas/";
    if (dataset_id == 4) f_name += "4-mirai/";

    f_name += "id(" + std::to_string(dataset_id) + ")_cutoff(" + std::to_string(cutoff) + ")_deletion_rate(" + std::to_string(deletion_rate) + ")_thread_num(" + std::to_string(thread_num) + ").csv";

    std::ofstream file;
    file.open(f_name.c_str(), std::ios::out | std::ios::app);

    if (file.fail()) {
        std::cout << " cannot open the result file.\n";
        file.clear();
        return;
    }

    // increment update_time_total
    update_time_total += local_density_update_time + dependent_point_update_time;

    // output: counter, update time, memory
    if (flag == 1) file << (local_density_update_time + dependent_point_update_time) / (freq * 1000) << "," << memory << "," << local_density_update_time / (freq * 1000) << "," << dependent_point_update_time / (freq * 1000) << "\n";
    if (flag == 0) file << update_time_total / 1000 << "," << memory << "," << counter << "\n";

    // init update_time
    local_density_update_time = 0;
    dependent_point_update_time = 0;
}