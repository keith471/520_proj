#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string.h>
#include <vector>

// header code

char* concat(char *s1, char *s2) {
	int len1 = strlen(s1);
	int len2 = strlen(s2);
	char* new_s = (char*)malloc(sizeof(char) * (len1 + len2 + 1));

	size_t k = 0;
	for (size_t i = 0; i < len1; ++i) {
	       new_s[k++] = s1[i];
	}
	for (size_t i = 0; i <= len2; ++i) {
		new_s[k++] = s2[i];
	}
	return new_s;
}

int GOLITE_CHECK_BOUNDS(int max, int idx) {
    if (idx < 0 || idx > max) {
        fprintf(stderr, "Out of bounds index\n");
        exit(1);
    }
    return idx;
}

template <typename T>

std::vector<T> golite_slice_append(std::vector<T> v, T x) {
    std::vector<T> w = v;
    w.push_back(x);
	return w;
}

// compiled code
