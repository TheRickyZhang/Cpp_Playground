#include <bits/stdc++.h>

int*** GetSpace(size_t x, size_t y, size_t z) {
    // Implement
    if(!x || !y || !z) return nullptr;
    int*** a = new int**[z];
    for(int i = 0; i < z; i++) {
        int** b = (a[i] = new int*[y]);
        for(int j = 0; j < y; j++) {
            int* c = (b[j] = new int[x]);
            std::fill_n(c, x, INT_MIN);
        } 
    }
    return a;
}
