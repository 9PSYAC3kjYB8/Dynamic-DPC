#include <vector>
#include "file_input.hpp"
#include <unordered_map>
#include <cfloat>
#include <algorithm>
#include <deque>
#include <map>


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

	// reverse dependent point
	std::vector<unsigned int> reverse_dependent_points;

	// cluster label
	int cluster_label = -2;

	bool flag_max = 0;


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

	bool operator<(const data &p) const { return dependent_distance < p.dependent_distance; }
	bool operator>(const data &p) const { return dependent_distance > p.dependent_distance; }

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

// input stat
void input_stat() {

	// stat. input
	std::string f_name = "../exact/result/";
	if (flag == 1) f_name = "../edmstream/result/";
	if (flag == 2) f_name = "../app-deper/result/";
	if (dataset_id == 1) f_name += "1-household/stat/";
	if (dataset_id == 2) f_name += "2-pamap2/stat/";
	if (dataset_id == 3) f_name += "3-gas/stat/";
	if (dataset_id == 4) f_name += "4-mirai/stat/";

	f_name += "id(" + std::to_string(dataset_id) + ")_cutoff(" + std::to_string(cutoff) + ")_deletion_rate(" + std::to_string(deletion_rate) + ")_counter(" + std::to_string(counter) + ").csv";

    // file input
	std::ifstream ifs_file(f_name);
    std::string full_data;

	// error check
	if (ifs_file.fail()) {
		std::cout << " stat file does not exist." << std::endl;
		std::exit(0);
	}

	while (std::getline(ifs_file, full_data)) {

		std::string meta_info;
		std::istringstream stream(full_data);
		std::string type = "";

		unsigned int id = 0;
		float local_density = 0;
		unsigned int dependent_point_id = 0;
		//bool flag_max = 0;

		unsigned int size = 3;

		for (unsigned int i = 0; i < size; ++i) {

			std::getline(stream, meta_info, ',');
			std::istringstream stream_(meta_info);
			float val = std::stof(meta_info);
			
			if (i == 0) id = val;
			if (i == 1) local_density = val;
			if (i == 2) dependent_point_id = val;
			//if (i == 3) flag_max = val;
		}

		if (flag == 1) --local_density;
		dataset[id].local_density = local_density;
		//dataset[id].flag_max = flag_max;

		if (id != dependent_point_id) {
			dataset[id].dependent_point_id = dependent_point_id;
			dataset[dependent_point_id].reverse_dependent_points.push_back(id);
		}
	}
}

// compute distance
float compute_distance(const data* p, const data* q) {

	float distance = 0;
	for (unsigned int i = 0; i < dimensionality; ++i) distance += powf(p->pt[i] - q->pt[i], 2.0);

	return sqrt(distance);
}