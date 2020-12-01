#include "labeling.hpp"


int main() {

    // input parameter
    input_parameter();

    // input dataset
    input_data();

    std::cout << " --------------------\n";
	std::cout << " dataset id: " << dataset_id << "\n";
	std::cout << " dataset cardinality: " << dataset.size() << "\n";
	std::cout << " dataset dimensionality: " << dimensionality << "\n";
    std::cout << " deletion rate: " << deletion_rate << "\n";
    std::cout << " cutoff distance: " << cutoff << "\n";
    std::cout << " counter: " << counter << "\n";
    std::cout << " rho min: " << rho_min << "\n";
    std::cout << " delta min: " << delta_min << "\n";
    std::cout << " flag: " << flag << "\n";
	std::cout << " --------------------\n\n";

    // input stat
    input_stat();

    // get dependent distance
    get_dependent_distance();

    // output decision graph
 //   output_decision_graph();

    // label propagation
    propagate_label();

    // label output
    output_result();

    // rand index comp.
    if (flag != 0) get_rand_index();

    return 0;
}