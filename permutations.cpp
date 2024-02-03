#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <vector>
#include <algorithm>
#include <random>
#include <iostream>
#include <thread>
namespace py = pybind11;



//std::vector<std::vector<int>> apply_permutations(std::vector<std::vector<int>>& matrix, std::vector<std::vector<int>>& permutations,int start,int end) {
void apply_permutations(std::vector<std::vector<int>>& matrix, std::vector<std::vector<int>>& permutations,int start,int end) {
	
	/*for(int i=0;i<matrix.size();i++){
		for(int j=0;j<matrix[i].size();j++){
			std::cout<<matrix[i][j]<<" ";
		}
		std::cout<<"\n";
	}
	
	for(int i=0;i<permutations.size();i++){
		for(int j=0;j<permutations[i].size();j++){
			std::cout<<permutations[i][j]<<" ";
		}
		std::cout<<"\n";
	}*/
	
	// Temporary matrix to store the result
	std::vector<std::vector<int>> result(matrix.size(), std::vector<int>(matrix[0].size()));
	
	// Initialize a random number generator
	std::default_random_engine rng(std::random_device{}());
	std::uniform_int_distribution<int> distribution(0, int(permutations.size())-1);
	// Apply permutations
	for (size_t i = start; i < end; ++i) {
		int randnum = distribution(rng);
		std::vector<int> perm((matrix[0].size()));
		for (size_t j = 0; j < matrix[i].size()-1; ++j) {
			//cout<<randnum<<"\n";
			//result[i][j] = matrix[i][permutations[randnum][j]];
			perm[j] = matrix[i][permutations[randnum][j]];
		}
		perm[int(matrix[i].size())-1]=randnum;
		matrix[i] = perm;
	}

	// Copy the result back to the original matrix
	//return result;
}
void apply_permutations_multi(std::vector<std::vector<int>>& matrix, std::vector<std::vector<int>>& permutations,const size_t numThreads){
	// Launch threads
	std::vector<std::thread> threads;
	size_t chunkSize = matrix.size() / numThreads;
    for (size_t i = 0; i < numThreads - 1; ++i) {
        size_t start = i * chunkSize;
        size_t end = (i + 1) * chunkSize;
        threads.emplace_back(apply_permutations, std::ref(matrix), std::ref(permutations), start, end);
    }

    // The last thread might have a larger chunk if the vector size is not a multiple of numThreads
    size_t start = (numThreads - 1) * chunkSize;
    size_t end = matrix.size();
    threads.emplace_back(apply_permutations, std::ref(matrix), std::ref(permutations), start, end);

    // Wait for all threads to finish
    for (auto& thread : threads) {
        thread.join();
    }
}

// Helper function to convert a 2D NumPy array to a std::vector<std::vector<int>>
std::vector<std::vector<int>> numpy_to_vector_vector_int(py::array_t<int> input) {
    py::buffer_info buf = input.request();
    if (buf.ndim != 2)
        throw std::runtime_error("Number of dimensions must be two");

    std::vector<std::vector<int>> result;
    result.reserve(buf.shape[0]);
    
    for (ssize_t i = 0; i < buf.shape[0]; ++i) {
        result.push_back(std::vector<int>(
            input.data(i), input.data(i) + buf.shape[1]));
    }
    
    return result;
}



// Helper function to convert a std::vector<std::vector<int>> back to a 2D NumPy array
py::array_t<int> vector_vector_int_to_numpy(std::vector<std::vector<int>>& input) {
    if (input.empty() || input[0].empty())
        return py::array_t<int>(0);

    ssize_t nrows = input.size();
    ssize_t ncols = input[0].size();
    std::vector<int> flat_matrix;
    flat_matrix.reserve(nrows * ncols);

    for (auto& row : input) {
        flat_matrix.insert(flat_matrix.end(), row.begin(), row.end());
    }
    
    return py::array_t<int>(
        {nrows, ncols}, // shape
        flat_matrix.data() // data pointer
    );
}

PYBIND11_MODULE(permutations, m) {
    m.def("apply_permutations", [](py::array_t<int> matrix, py::array_t<int> permutations, int num_cores) {
        std::vector<std::vector<int>> vec_matrix = numpy_to_vector_vector_int(matrix);
        std::vector<std::vector<int>> vec_permutations = numpy_to_vector_vector_int(permutations);
        apply_permutations_multi(vec_matrix, vec_permutations,num_cores);
        //apply_permutations(vec_matrix, vec_permutations,0,vec_matrix.size());
        return vector_vector_int_to_numpy(vec_matrix);
    });
}
