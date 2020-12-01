#include "file_output.hpp"
#include <omp.h>
#include <deque>


// get dependent distance
void get_dependent_distance() {

    for (unsigned int i = 0; i < dataset.size(); ++i) {

        if (dataset[i].local_density > 0) {

            // compute dependent distance
            dataset[i].dependent_distance = compute_distance(&dataset[i], &dataset[dataset[i].dependent_point_id]);
            if (dataset[i].identifier == dataset[i].dependent_point_id) dataset[i].dependent_distance = FLT_MAX;
        }
    }

    float local_density_max = 0;
    unsigned int id_max = 0;

    for (unsigned int i = 0; i < dataset.size(); ++i) {

        // get local density max
        if (dataset[i].local_density > local_density_max) {
            local_density_max = dataset[i].local_density;
            id_max = i;
        }

        if (dataset[i].local_density < 1 && dataset[i].dependent_distance < delta_min) dataset[i].local_density = dataset[dataset[i].dependent_point_id].local_density;
    }

    dataset[id_max].dependent_point_id = id_max;
    dataset[id_max].dependent_distance = FLT_MAX;

    // set noise
    for (unsigned int i = 0; i < dataset.size(); ++i) {

        if (dataset[i].local_density < rho_min) dataset[i].cluster_label = -1;
        if (dataset[i].local_density == 0) dataset[i].cluster_label = -2;
    }
}

// label propagation
void propagate_label() {

    // get cluster center
    std::deque<unsigned int> stack;
    for (unsigned int i = 0; i < dataset.size(); ++i) {

        if (dataset[i].local_density >= rho_min && dataset[i].dependent_distance >= delta_min) {

            // insert into stack
            stack.push_back(i);

            // set label
            dataset[i].cluster_label = dataset[i].identifier;
        }        
    }

    unsigned int id = 0;
    unsigned int label = 0;

    // DFS
    while (stack.size() > 0) {

        // get the front
        id = stack[0];

        // pop the front
        stack.pop_front();

        // set label
        label = dataset[id].cluster_label;

        for (unsigned int i = 0; i < dataset[id].reverse_dependent_points.size(); ++i) {

            const unsigned int id_ = dataset[id].reverse_dependent_points[i];

            if (dataset[id_].cluster_label == -2) {

                // update label
                dataset[id_].cluster_label = label;

                // update stack
                stack.push_back(id_);
            }
        }
    }

    // output label
    if (flag == 0) output_result();
}

// rand index computation
void get_rand_index() {

    // input exact answer
    std::unordered_map<unsigned int, int> answer;

    std::string f_name = "result/label/";
	if (dataset_id == 1) f_name += "1-household/exact/";
	if (dataset_id == 2) f_name += "2-pamap2/exact/";
	if (dataset_id == 3) f_name += "3-gas/exact/";
	if (dataset_id == 4) f_name += "4-mirai/exact/";

	f_name += "label-ex-id(" + std::to_string(dataset_id) + ")_cutoff(" + std::to_string(cutoff) + ")_deletion_rate(" + std::to_string(deletion_rate) + ")_counter(" + std::to_string(counter) + ").txt";

    // file input
	std::ifstream ifs_file(f_name);

	// error check
	if (ifs_file.fail()) {
		std::cout << " label file does not exist." << std::endl;
		std::exit(0);
	}

    unsigned int id = 0;
    unsigned int label = 0;

    while (!ifs_file.eof()) {
        
        ifs_file >> id >> label;
        answer[id] = label;
    }

    // set sampling ratio
	const float ratio = 0.05;

	// prepare vector for samples
	std::vector<data*> dataset_sample;

	// random value variable
	std::mt19937 mt(1);
	std::uniform_real_distribution<> rnd(0, 1);

	// get samples
	for (unsigned int i = 0; i < dataset.size(); ++i) {

        if (dataset[i].cluster_label != -2) {
            if (rnd(mt) <= ratio) dataset_sample.push_back(&dataset[i]);
        }
	}

    double cnt = 0;
	double cardinality = (double)dataset_sample.size();
	cardinality *= (double)(dataset_sample.size() - 1.0);
	cardinality /= 2.0;

    #pragma omp parallel num_threads(18)
	{
		#pragma omp for schedule(dynamic) reduction(+:cnt)
        for (unsigned int i = 0; i < dataset_sample.size() - 1; ++i) {

            // get id
            const unsigned int id = dataset_sample[i]->identifier;

            for (unsigned int j = i + 1; j < dataset_sample.size(); ++ j) {

                // get id
                const unsigned int id_ = dataset_sample[j]->identifier;

                if (answer[id] == answer[id_]) {

                    if (dataset_sample[i]->cluster_label == dataset_sample[j]->cluster_label) ++cnt;
                }
                else {

                    if (dataset_sample[i]->cluster_label != dataset_sample[j]->cluster_label) ++cnt;
                }
            }
        }
    }

    double rand_index = cnt / cardinality;
    std::cout << " rand index: " << rand_index << "\n";
}