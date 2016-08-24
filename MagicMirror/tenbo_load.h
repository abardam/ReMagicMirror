#pragma once
#include "stdafx.h"
#include <vector>

template <typename T>
bool read_arr(const char * filename, std::vector<T> & vec) {
	FILE * file;
	errno_t err = fopen_s(&file, filename, "r");
	if (file == NULL) {
		printf("error code: %d\n", err);
		return false;
	}

	vec.clear();

	float inp;
	while (1) {
		int matches = fscanf(file, "%f", &inp);
		if (matches > 0) {
			vec.push_back((T)inp);
		}
		else {
			break;
		}
	}
	fclose(file);
	return true;
}

template <typename T>
bool read_nda(const char * filename, std::vector<T> & vec, bool transpose = false) {
	FILE * file;
	errno_t err = fopen_s(&file, filename, "r");
	if (file == NULL) {
		printf("error code: %d\n", err);
		return false;
	}

	vec.clear();

	float inp;
	char header[128];
	int dim0, dim1; // 2 dimensional only for now
	int matches = fscanf(file, "%s", header);
	if (matches > 0) {
		if (strcmp(header, "dimensions") == 0) {
			matches = fscanf(file, "%d %d\n", &dim0, &dim1);
		}
	}
	else {
		fclose(file);
		return false;
	}

	vec.resize(dim0 * dim1);
	int ctr0 = 0, ctr1 = 0;
	while (1) {
		int matches = fscanf(file, "%f", &inp);
		if (matches == 1) {
			if (!transpose) {
				vec[ctr0 * dim1 + ctr1] = inp;
				++ctr1;
				if (ctr1 >= dim1) {
					ctr1 = 0;
					++ctr0;
				}
			}
			else {
				vec[ctr1 * dim0 + ctr0] = inp;
				++ctr1;
				if (ctr1 >= dim1) {
					ctr1 = 0;
					++ctr0;
				}
			}
		}
		else {
			break;
		}
	}
	fclose(file);
	return true;

}

template <typename T>
std::vector<T> remap_vector(const std::vector<T> & vec, const std::vector<int> & mapping) {
	std::vector<T> map_vec(mapping.size());
	for (int i = 0; i < mapping.size(); ++i) {
		map_vec[i] = vec[mapping[i]];
	}
	return map_vec;
}