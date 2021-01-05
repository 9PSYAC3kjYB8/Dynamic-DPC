## Directories
* **app-deper**: source codes of App-deper.
* **dataset**: .txt files of datasets used.
* **edmstream**: source codes of EDMStream.
* **exact**: contains stat files of local density and depedent object.
* **labeling**: source codes for calculating Rand index.
* **parameter**: .txt files of parameters.
* **semi-static**: source codes of Semi-static.

## Datasets
* [Gas](https://archive.ics.uci.edu/ml/datasets/Gas+sensor+array+under+dynamic+gas+mixtures)
* [Household](https://archive.ics.uci.edu/ml/datasets/Individual+household+electric+power+consumption)
* [PAMAP2](https://archive.ics.uci.edu/ml/datasets/PAMAP2+Physical+Activity+Monitoring)
* [Mirai](https://archive.ics.uci.edu/ml/datasets/Kitsune+Network+Attack+Dataset)

## Parameter setting
* See readme.md in parameter directory

## How to compile
* For App-deper case: `g++ -O3 -o app-deper.out main.cpp --std=c++14 -fopenmp -Wall`
* .out file name can be arbitrary.
* We used Ubuntu 18.04 LTS.

## Results
* In app-deper and edmstream, there is `result` directory.
   * For semi-static, create `result` directory.
* For each dataset, the experimental result is provided in the corresponding directory.
   * Dataset IDs of Gas, Household, PAMAP2, and Mirai are 3, 1, 2, and 4, respectively.
   * Create `result/1-household/stat`, `result/2-pamap2/stat`, and `result/3-gas/stat`, for each algorithm.
   * `stat` directory will have a statistics file for local density and dependent point. This is used for calculating accuracy.
* The exact result of Mirai is in `result` directory.
  * The results on the other datasets cannot be uploaded due to the size constraint.
* Codes in `labeling` calculate Rand index and provide clustering labels.
    * For app-deper (edmsteream), set flag = 2 (1).
    * Set counter via counter.txt, see stat directory.
* We calculated NMI by using scikit-learn library.
