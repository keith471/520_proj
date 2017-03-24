#include <cstdio>
#include <cstdlib>
#include <vector>

using namespace std;

// header code

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
