#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <random>
#include <time.h>


// dataset identifier
unsigned int dataset_id = 0;

// data dimensionality
unsigned int dimensionality = 2;

// dataset deletion rate
float deletion_rate = 1.0;

// distance function type
unsigned int type = 0;

// cutoff distance
float cutoff = 0;

// rho_min
float rho_min = 0;

// delta_min
float delta_min = 0;

// counter
unsigned int counter = 0;

// algorithm flag, 0: exact, 1: edmstream, 2: pg-msp
unsigned int flag = 0;


// parameter input
void input_parameter() {

	std::ifstream ifs_dataset_id("../parameter/dataset_id.txt");
	std::ifstream ifs_deletion_rate("../parameter/deletion_rate.txt");
	std::ifstream ifs_cutoff("../parameter/cutoff.txt");
	std::ifstream ifs_rho("../parameter/rho_min.txt");
	std::ifstream ifs_delta("../parameter/delta_min.txt");
	std::ifstream ifs_counter("../parameter/counter.txt");
	std::ifstream ifs_flag("../parameter/flag.txt");

	if (ifs_dataset_id.fail()) {
		std::cout << " dataset_id.txt does not exist." << std::endl;
		std::exit(0);
	}
	if (ifs_deletion_rate.fail()) {
		std::cout << " deletion_rate.txt does not exist." << std::endl;
		std::exit(0);
	}
	if (ifs_cutoff.fail()) {
		std::cout << " cutoff.txt does not exist." << std::endl;
		std::exit(0);
	}
	if (ifs_rho.fail()) {
		std::cout << " rho_min.txt does not exist." << std::endl;
		std::exit(0);
	}
	if (ifs_delta.fail()) {
		std::cout << " delta_min.txt does not exist." << std::endl;
		std::exit(0);
	}
	if (ifs_counter.fail()) {
		std::cout << " counter.txt does not exist." << std::endl;
		std::exit(0);
	}
	if (ifs_flag.fail()) {
		std::cout << " flag.txt does not exist." << std::endl;
		std::exit(0);
	}

	while (!ifs_dataset_id.eof()) { ifs_dataset_id >> dataset_id; }
	while (!ifs_deletion_rate.eof()) { ifs_deletion_rate >> deletion_rate; }
	while (!ifs_cutoff.eof()) { ifs_cutoff >> cutoff; }
	while (!ifs_rho.eof()) { ifs_rho >> rho_min; }
	while (!ifs_delta.eof()) { ifs_delta >> delta_min; }
	while (!ifs_counter.eof()) { ifs_counter >> counter; }
	while (!ifs_flag.eof()) { ifs_flag >> flag; }

	// set dimensionality
	if (dataset_id == 1) dimensionality = 7;
	if (dataset_id == 2) dimensionality = 51;
	if (dataset_id == 3) dimensionality = 18;
	if (dataset_id == 4) dimensionality = 115;
}