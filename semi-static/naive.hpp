#include "file_output.hpp"


// local density update (insertion case)
void update_local_density_insertion(std::deque<data*> &dataset_active, data* dat) {

    // copy local density of new object
    float local_density = dat->local_density;

    // scan while updating local density
    #pragma omp parallel num_threads(thread_num)
    {
        #pragma omp for schedule(static) reduction(+:local_density)
        for (unsigned int i = 0; i < dataset_active.size() - 1; ++i) {

            // compute distance
            const float distance = compute_distance(dataset_active[i], dat);

            // update local density
            if (distance <= cutoff) {

                dataset_active[i]->local_density = dataset_active[i]->local_density + 1.0;
                local_density = local_density + 1.0;
            }
        }
    }

    // copy again
    dat->local_density = local_density;
}

// dependent point update (insertion case)
void update_dependent_point(std::deque<data*> &dataset_active) {

    std::vector<std::vector<unsigned int>> scan_local(thread_num), scan_sub_local(thread_num);

    #pragma omp parallel num_threads(thread_num)
    {
        #pragma omp for schedule(static)
        for (unsigned int i = 0; i < dataset_active.size(); ++i) {

            // reset
            dataset_active[i]->dependent_distance = FLT_MAX;
            dataset_active[i]->dependent_point_id = dataset_active[i]->identifier;

            for (unsigned int j = 0; j < dataset_active.size(); ++j) {

                if (dataset_active[i]->local_density < dataset_active[j]->local_density) {

                    // compute distance
                    const float distance = compute_distance(dataset_active[i], dataset_active[j]);

                    if (dataset_active[i]->dependent_distance > distance) {
                        
                        dataset_active[i]->dependent_distance = distance;
                        dataset_active[i]->dependent_point_id = dataset_active[j]->identifier;
                    }
                }
            }
        }
    }
}

// local density update (insertion case)
void update_local_density_deletion(std::deque<data*> &dataset_active, data* dat) {

    // scan while updating local density
    #pragma omp parallel num_threads(thread_num)
    {
        #pragma omp for schedule(static)
        for (unsigned int i = 0; i < dataset_active.size(); ++i) {

            // compute distance
            const float distance = compute_distance(dataset_active[i], dat);

            // update local density
            if (distance <= cutoff) dataset_active[i]->local_density = dataset_active[i]->local_density - 1.0;
        }
    }
}