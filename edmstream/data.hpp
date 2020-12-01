#include <vector>
#include "file_input.hpp"
#include <unordered_map>
#include <cfloat>
#include <algorithm>
#include <deque>
#include <unordered_set>


// declaration of data
class data;

// definition of dataset
std::vector<data> dataset;

// compute distance
float compute_distance(const data* p, const data* q);


// definition of data class
class data {

public:

	// for numeric data
	std::vector<float> pt;

	// identifier
	unsigned int identifier = 0;

	// local density
	float local_density = 0;

	// dependent point
	unsigned int dependent_point_id = 0;

	// dependent distance
	float dependent_distance = FLT_MAX;

	// local density update flag
	bool update_flag_local_density = 0;

	// flag to suggest necessity of computing dependent point
	bool update_flag_dependent_point = 0;

	// temporal memory for distance to new object
	float distance_temp = 0;

	// seed flag
	bool seed_flag = 0;

	// member (if seed)
	std::unordered_set<unsigned int> member;


	/***************/
	/* constructor */
	/***************/

	// standard
	data() {}

	// with identifier
	data(unsigned int id) {

		identifier = id;
		dependent_point_id = id;
	}

	/*******************/
	/* member function */
	/*******************/

	// point update
	void update_pt(std::vector<float>& point) { for (unsigned int i = 0; i < point.size(); ++i) pt.push_back(point[i]); }

	// dependent point update (scan whole set)
	void update_dependent_point(std::deque<data*> &seed_set) {

		// init dependent distance
		dependent_distance = FLT_MAX;

		for (unsigned int i = 0; i < seed_set.size(); ++i) {

			if (seed_set[i]->local_density > local_density) {

				// compute distance
				float distance = compute_distance(seed_set[i], this);

				// update dependent point
				if (distance < dependent_distance) {

					dependent_distance = distance;
					dependent_point_id = seed_set[i]->identifier;
				}
			}
		}
	}

	// dependent point update (scan updated objects)
	void update_dependent_point(std::vector<unsigned int> &seedset_rho_update) {

		for (unsigned int i = 0; i < seedset_rho_update.size(); ++i) {

			// get identifier
			const unsigned int id = seedset_rho_update[i];

			if (dataset[id].local_density > local_density) {

				// compute distance
				float distance = compute_distance(&dataset[id], this);

				// update dependent point
				if (distance < dependent_distance) {

					dependent_distance = distance;
					dependent_point_id = id;
				}
			}
		}
	}

};


// input data
void input_data() {

	// id variable
	unsigned int id = 0;

	// point coordinates variable
    data dat;
    dat.pt.resize(dimensionality);

    // dataset input
	std::string f_name = "../dataset/";
	if (dataset_id == 1) f_name += "household-7d.txt";
	if (dataset_id == 2) f_name += "pamap2-51d.txt";
	if (dataset_id == 3) f_name += "gas-18d.txt";
	if (dataset_id == 4) f_name += "mirai-115d.txt";

    // file input
	std::ifstream ifs_file(f_name);
    std::string full_data;

	// error check
	if (ifs_file.fail()) {
		std::cout << " data file does not exist." << std::endl;
		std::exit(0);
	}

	while (std::getline(ifs_file, full_data)) {

		std::string meta_info;
		std::istringstream stream(full_data);
		std::string type = "";

		for (unsigned int i = 0; i < dimensionality; ++i) {

			std::getline(stream, meta_info, ',');
			std::istringstream stream_(meta_info);
			long double val = std::stold(meta_info);
			if (val < 0.0000001) val = 0;
			dat.pt[i] = val;
		}

		// update id
		dat.identifier = id;

		// insert into dataset
		dataset.push_back(dat);

		// increment identifier
		++id;
	}
}

// compute distance
float compute_distance(const data* p, const data* q) {

	float distance = 0;

	if (type == 0) {

		// euclidean (l2) distance
		for (unsigned int i = 0; i < dimensionality; ++i) distance += powf(p->pt[i] - q->pt[i], 2.0);
		distance = sqrt(distance);
	}
	else if (type == 1) {

		// manhattan (l1) distance
		for (unsigned int i = 0; i < dimensionality; ++i) distance += fabsf(p->pt[i] - q->pt[i]);
	}

	return distance;
}