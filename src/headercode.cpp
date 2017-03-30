#include <cstdio>
#include <cstdlib>
#include <string.h>
#include <vector>

using namespace std;

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
    if (idx < 0 || idx >= max) {
        fprintf(stderr, "Out of bounds index\n");
        exit(1);
    }
    return idx;
}

template <typename T>

vector<T> golite_slice_append(vector<T> v, T x) {
    vector<T> w = v;
    w.push_back(x);
	return w;
}

// compiled code
