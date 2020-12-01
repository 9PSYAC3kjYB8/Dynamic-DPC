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

// #threads
unsigned int thread_num = 0;

// cell parameter
float cell_radius = 0;


// get current time
void get_current_time() {

	time_t t = time(NULL);
	printf(" %s\n", ctime(&t));
}

// parameter input
void input_parameter() {

	std::ifstream ifs_dataset_id("../parameter/dataset_id.txt");
	std::ifstream ifs_deletion_rate("../parameter/deletion_rate.txt");
	std::ifstream ifs_cutoff("../parameter/cutoff.txt");
	std::ifstream ifs_thread("../parameter/thread_num.txt");

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
	if (ifs_thread.fail()) {
		std::cout << " thread_num.txt does not exist." << std::endl;
		std::exit(0);
	}

	while (!ifs_dataset_id.eof()) { ifs_dataset_id >> dataset_id; }
	while (!ifs_deletion_rate.eof()) { ifs_deletion_rate >> deletion_rate; }
	while (!ifs_cutoff.eof()) { ifs_cutoff >> cutoff; }
	while (!ifs_thread.eof()) { ifs_thread >> thread_num; }

	// set dimensionality & r
	if (dataset_id == 1) {
		dimensionality = 7;
		cell_radius = 0.1;
	}
	else if (dataset_id == 2) {
		dimensionality = 51;
		cell_radius = 0.625;
	}
	else if (dataset_id == 3) {
		dimensionality = 18;
		cell_radius = 25;
	}
	else if (dataset_id == 4) {
		dimensionality = 115;
		//type = 1;
		cell_radius = 600;
	}
}