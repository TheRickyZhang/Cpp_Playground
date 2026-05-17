#include <bits/stdc++.h>
using namespace std;

int calc() {
  vector<int> counts(6);
  for(int i = 0; i < 100; i++) {
    int num = rand() % 6;
    counts[num]++;
  }
  int mn = *min_element(counts.begin(), counts.end());
  int res = 0;
  for(int i = 0; i < 6; i++) {
    res += (i+1) * (counts[i] - mn);
  }
  return res;
}

int main() {
  double res = 0;
  for(int i = 0; i < 100; i++) {
    res += calc();
  }
  cout<<(res/100)<<endl;
}
