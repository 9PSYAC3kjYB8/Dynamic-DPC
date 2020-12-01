#include "data.hpp"
#include <chrono>
#include <unistd.h>


// decision graph update
void output_decision_graph() {

    std::string f_name = "result/decision-graph/";
    if (dataset_id == 1) f_name += "1-household/";
	if (dataset_id == 2) f_name += "2-pamap2/";
	if (dataset_id == 3) f_name += "3-gas/";
    if (dataset_id == 4) f_name += "4-mirai/";
    if (flag == 0) f_name += "exact/ex-";
    if (flag == 1) f_name += "edmstream/edm-";
    if (flag == 2) f_name += "app-deper/";

    f_name += "id(" + std::to_string(dataset_id) + ")_cutoff(" + std::to_string(cutoff) + ")_deletion_rate(" + std::to_string(deletion_rate) + ")_counter(" + std::to_string(counter) + ").csv";

    std::ofstream file;
    file.open(f_name.c_str(), std::ios::out | std::ios::app);

    if (file.fail()) {
        std::cout << " cannot open the decision-graph file.\n";
        file.clear();
        return;
    }

    // sort by dependent distance
    std::sort(dataset.begin(), dataset.end(), std::greater<data>());

    // update the dependent distance of object with the max local density
    if (flag == 0 || flag == 2) {

        for (unsigned int i = 0; i < dataset.size(); ++i) {
            if (dataset[i].local_density > 0) {                
                for (unsigned int j = i + 1; j < dataset.size(); ++j) {
                    if (dataset[j].local_density > 0) {
                        dataset[i].dependent_distance = dataset[j].dependent_distance + 1;
                        break;
                    }
                }
                break;
            }
        }
    }

    for (unsigned int i = 0; i < dataset.size(); ++i) {
        
        if (dataset[i].local_density > 0) file << dataset[i].identifier << "," << dataset[i].local_density << "," << dataset[i].dependent_distance << "," << dataset[i].flag_max << "\n";
    }
}

// result output
void output_result() {

    std::string f_name = "result/label/";
    if (dataset_id == 1) f_name += "1-household/";
	if (dataset_id == 2) f_name += "2-pamap2/";
	if (dataset_id == 3) f_name += "3-gas/";
    if (dataset_id == 4) f_name += "4-mirai/";
    if (flag == 0) f_name += "exact/label-ex-";
    if (flag == 1) f_name += "edmstream/label-edm-";
    if (flag == 2) f_name += "app-deper/label-";

    f_name += "id(" + std::to_string(dataset_id) + ")_cutoff(" + std::to_string(cutoff) + ")_deletion_rate(" + std::to_string(deletion_rate) + ")_counter(" + std::to_string(counter) + ").txt";

    std::ofstream file;
    file.open(f_name.c_str(), std::ios::out | std::ios::app);

    if (file.fail()) {
        std::cout << " cannot open the result file.\n";
        file.clear();
        return;
    }

    for (unsigned int i = 0; i < dataset.size(); ++i) file << i << "\t" << dataset[i].cluster_label << "\n";
}