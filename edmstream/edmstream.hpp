#include "file_output.hpp"
#include <omp.h>


// local density update (insertion case)
void update_local_density_insertion(std::vector<unsigned int> &seedset_rho_update, std::deque<data*> &seed_set, std::deque<data*> &dataset_active, data* dat) {

    // set of local density updated seeds (local)
    std::vector<std::vector<unsigned int>> seedset_rho_update_local(thread_num);

    // min dist. to seed (local)
    std::vector<float> distance_min_local(thread_num);
    for (unsigned int i = 0; i < thread_num; ++i) distance_min_local[i] = FLT_MAX;

    // seed index (local)
    std::vector<int> seed_idx_local(thread_num);
    for (unsigned int i = 0; i < thread_num; ++i) seed_idx_local[i] = -1;

    #pragma omp parallel num_threads(thread_num)
    {
        #pragma omp for schedule(static)
        for (unsigned int i = 0; i < seed_set.size(); ++i) {

            const float distance = compute_distance(seed_set[i], dat);

            if (distance <= cutoff) {

                // update local density
                ++seed_set[i]->local_density;
                seedset_rho_update_local[omp_get_thread_num()].push_back(seed_set[i]->identifier);

                // update flag
                seed_set[i]->update_flag_local_density = 1;
            }
            else {

                if (seed_set[i]->dependent_distance > distance - cutoff) seed_set[i]->update_flag_dependent_point = 1;
            }

            // cell candidate check
            if (distance <= cell_radius) {

                if (distance_min_local[omp_get_thread_num()] > distance) {

                    distance_min_local[omp_get_thread_num()] = distance;
                    seed_idx_local[omp_get_thread_num()] = i;
                }
            }
        }
    }

    // reduction
    float distance_min = FLT_MAX;
    int seed_idx = -1;

    for (unsigned int i = 0; i < thread_num; ++i) {

        // local density updated seeds
        for (unsigned int j = 0; j < seedset_rho_update_local[i].size(); ++j) seedset_rho_update.push_back(seedset_rho_update_local[i][j]);

        // min distance update
        if (distance_min_local[i] < distance_min) {

            distance_min = distance_min_local[i];
            seed_idx = seed_idx_local[i];
        }
    }

    if (seed_idx != -1) {

        // dependency update
        dat->dependent_distance = distance_min;
        dat->dependent_point_id = seed_set[seed_idx]->identifier;

        // member update
        seed_set[seed_idx]->member.insert(dat->identifier);
    }
    else {

        // form a new seed
        dat->seed_flag = 1;

        // insert into seed_set
        seed_set.push_back(dat);

        // compute local density
        float local_density = 0;

        #pragma omp parallel num_threads(thread_num)
        {
            #pragma omp for schedule(static) reduction(+:local_density)
            for (unsigned int i = 0; i < dataset_active.size(); ++i) {

                const float distance = compute_distance(dataset_active[i], dat);
                if (distance <= cutoff) local_density = local_density + 1;
            }
        }
        dat->local_density += local_density;

        // insert into seedset_rho_update
        seedset_rho_update.push_back(seed_set.size() - 1);
    }
}

// dependent point update (insertion case)
void update_dependent_point_insertion(std::vector<unsigned int> &seedset_rho_update, std::deque<data*> &seed_set, data* dat) {

    std::vector<std::vector<unsigned int>> scan_local(thread_num), scan_sub_local(thread_num);

    #pragma omp parallel num_threads(thread_num)
    {
        #pragma omp for schedule(static)
        for (unsigned int i = 0; i < seed_set.size(); ++i) {

            if (seed_set[i]->update_flag_local_density) {

                if (seed_set[i]->dependent_point_id == seed_set[i]->identifier) {

                    // scan case
                    scan_local[omp_get_thread_num()].push_back(i);
                }
                else {

                    // get dependent point
                    const unsigned int id = seed_set[i]->dependent_point_id;

                    if (seed_set[i]->local_density > dataset[id].local_density) {

                        // scan case
                        scan_local[omp_get_thread_num()].push_back(i);
                    }
                    else {
                        if (dat->seed_flag) {
                            if (seed_set[i]->local_density < dat->local_density) {
                                
                                // incremental update of dependent point
                                const float distance = compute_distance(seed_set[i], dat);

                                if (distance < seed_set[i]->dependent_distance) {
                                    seed_set[i]->dependent_distance = distance;
                                    seed_set[i]->dependent_point_id = dat->identifier;
                                }
                            }
                        }
                    }
                }
            }
            else {

                // scan N(p) case
                if (seed_set[i]->update_flag_dependent_point) scan_sub_local[omp_get_thread_num()].push_back(i);
            }

            // init flag
            seed_set[i]->update_flag_local_density = 0;
            seed_set[i]->update_flag_dependent_point = 0;
        }
    }

    // reduction
    std::vector<unsigned int> scan, scan_sub;
    for (unsigned int i = 0; i < thread_num; ++i) {
        for (unsigned int j = 0; j < scan_local[i].size(); ++j) scan.push_back(scan_local[i][j]);
        for (unsigned int j = 0; j < scan_sub_local[i].size(); ++j) scan_sub.push_back(scan_sub_local[i][j]);
    }

    // update scan count
    scan_num += (unsigned int)scan.size();

    // scan the whole dataset
    #pragma omp parallel num_threads(thread_num)
    {
        #pragma omp for schedule(static)
        for (unsigned int i = 0; i < scan.size(); ++i) {
            seed_set[scan[i]]->update_dependent_point(seed_set);
        }
    }

    // update subscan count
    subscan_num += (unsigned int)scan_sub.size();

    // scan the set of object with rho update
    #pragma omp parallel num_threads(thread_num)
    {
        #pragma omp for schedule(static)
        for (unsigned int i = 0; i < scan_sub.size(); ++i) {
            seed_set[scan_sub[i]]->update_dependent_point(seedset_rho_update);
        }
    }
}

// local density update (deletion case)
void update_local_density_deletion(std::deque<data*> &seed_set, data* dat) {

    // local local_density_max
    std::vector<float> local_density_max_array(thread_num);

    // scan while updating local density
    #pragma omp parallel num_threads(thread_num)
    {
        #pragma omp for schedule(static)
        for (unsigned int i = 0; i < seed_set.size(); ++i) {

            // compute distance
            const float distance = compute_distance(seed_set[i], dat);

            // update local density
            if (distance <= cutoff) seed_set[i]->local_density = seed_set[i]->local_density - 1.0;
        }
    }

    // init local density of deleted object
    dat->local_density = 0;
}

// dependent point update (deletion case)
void update_dependent_point_deletion(std::deque<data*> &seed_set, data* dat) {

    // local array for object that needs scan
    std::vector<std::vector<unsigned int>> scan_local(thread_num);

    #pragma omp parallel num_threads(thread_num)
    {
        #pragma omp for schedule(static)
        for (unsigned int i = 0; i < seed_set.size(); ++i) {

            // get id of dependent point, then scan case
            const unsigned int id = seed_set[i]->dependent_point_id;
            if (seed_set[i]->local_density > dataset[id].local_density) scan_local[omp_get_thread_num()].push_back(i);

            // object that may become one with not max local density
            if (seed_set[i]->dependent_distance == FLT_MAX) scan_local[omp_get_thread_num()].push_back(i);
        }
    }

    // reduction
    std::vector<unsigned int> scan;
    for (unsigned int i = 0; i < thread_num; ++i) {
        for (unsigned int j = 0; j < scan_local[i].size(); ++j) scan.push_back(scan_local[i][j]);
    }

    // update scan count
    scan_num += (unsigned int)scan.size();

    // scan the whole dataset
    #pragma omp parallel num_threads(thread_num)
    {
        #pragma omp for schedule(static)
        for (unsigned int i = 0; i < scan.size(); ++i) {
            seed_set[scan[i]]->update_dependent_point(seed_set);
        }
    }
}

// verification
void verification(std::deque<data*> &seed_set, std::deque<data*> &dataset_active, data* dat) {

    if (dat->seed_flag) {

        auto itr = dat->member.begin();
        while (itr != dat->member.end()) {

            // get id
            const unsigned int id = *itr;

            float distance_min = FLT_MAX;
            int seed_idx = -1;

            for (unsigned int i = 0; i < seed_set.size(); ++i) {

                const float distance = compute_distance(seed_set[i], &dataset[id]);

                if (distance <= cell_radius) {

                    distance_min = distance;
                    seed_idx = i;
                }
            }

            if (seed_idx != -1) {

                dataset[id].dependent_distance = distance_min;
                dataset[id].dependent_point_id = seed_set[seed_idx]->identifier;

                // member update
                seed_set[seed_idx]->member.insert(id);
            }
            else {

                // form a new cell
                dataset[id].seed_flag = 1;

                // init dependency
                dataset[id].dependent_point_id = id;
                dataset[id].dependent_distance = FLT_MAX;

                // local density computation
                float local_density = 0;

                #pragma omp parallel num_threads(thread_num)
                {
                    #pragma omp for schedule(static) reduction(+:local_density)
                    for (unsigned int i = 0; i < dataset_active.size(); ++i) {

                        const float distance = compute_distance(&dataset[id], dataset_active[i]);

                        if (distance <= cutoff) local_density = local_density + 1.0;
                    }
                }

                dataset[id].local_density += local_density;

                // dependency update
                for (unsigned int i = 0; i < seed_set.size(); ++i) {                    

                    const float distance = compute_distance(&dataset[id], seed_set[i]);

                    if (dataset[id].local_density < seed_set[i]->local_density) {

                        if (dataset[id].dependent_distance > distance) {
                            dataset[id].dependent_distance = distance;
                            dataset[id].dependent_point_id = seed_set[i]->identifier;
                        }
                    }

                    if (dataset[id].local_density > seed_set[i]->local_density) {

                        if (seed_set[i]->dependent_distance > distance) {
                            seed_set[i]->dependent_distance = distance;
                            seed_set[i]->dependent_point_id = id;
                        }
                    }
                }

                // insert into seed_set
                seed_set.push_back(&dataset[id]);
            }

            ++itr;
        }

        // init member
        dat->member.clear();
    }
    else {

        // get seed id
        const unsigned int id = dat->dependent_point_id;

        // erase from member
        dataset[id].member.erase(dat->identifier);
    }
}