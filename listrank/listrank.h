#include "parallel.h"
#include "random.h"

// For timing parts of your code.
#include "get_time.h"

// For computing sqrt(n)
#include <math.h>

using namespace parlay;

// Some helpful utilities
namespace {

// returns the log base 2 rounded up (works on ints or longs or unsigned
// versions)
template <class T>
size_t log2_up(T i) {
  assert(i > 0);
  size_t a = 0;
  T b = i - 1;
  while (b > 0) {
    b = b >> 1;
    a++;
  }
  return a;
}

}  // namespace


struct ListNode {
  ListNode* next;
  size_t rank;
  ListNode(ListNode* next) : next(next), rank(std::numeric_limits<size_t>::max()) {}
};

// Serial List Ranking. The rank of a node is its distance from the
// tail of the list. The tail is the node with `next` field nullptr.
//
// The work/depth bounds are:
// Work = O(n)
// Depth = O(n)
void SerialListRanking(ListNode* head) {
  size_t ctr = 0;
  ListNode* save = head;
  while (head != nullptr) {
    head = head->next;
    ++ctr;
  }
  head = save;
  --ctr;  // last node is distance 0
  while (head != nullptr) {
    head->rank = ctr;
    head = head->next;
    --ctr;
  }
}

// Wyllie's List Ranking. Based on pointer-jumping.
//
// The work/depth bounds of your implementation should be:
// Work = O(n*\log(n))
// Depth = O(\log^2(n))
void WyllieListRanking(ListNode* L, size_t n) {
  int* succ = (int *) malloc (sizeof(int) * n);
  int* succ_prime = (int *) malloc (sizeof(int) * n);
  int* rank_prime = (int *) malloc (sizeof(int) * n);

  parallel_for(0, n, [&](size_t i) {
    if (L[i].next != nullptr) {
      L[i].rank = 1;
      succ[i] = L[i].next - L;
    } else {
      L[i].rank = 0;
      succ[i] = 0;
    }
  });

  for (int k = 0; k < (int)log2_up(n); k++) {
    parallel_for(0, n, [&](size_t i) {
      if (succ[i] != 0) {
        rank_prime[i] = L[i].rank + L[succ[i]].rank;
        succ_prime[i] = succ[succ[i]];
      } else {
        succ_prime[i] = succ[i];
      }
    });

    parallel_for(0, n, [&](size_t i) {
      succ[i] = succ_prime[i];
      L[i].rank = rank_prime[i];
    });
  }

  free(rank_prime);
  free(succ);
  free(succ_prime);
}


// Sampling-Based List Ranking
//
// The work/depth bounds of your implementation should be:
// Work = O(n) whp
// Depth = O(\sqrt(n)* \log(n)) whp
void SamplingBasedListRanking(ListNode* L, size_t n, long num_samples=-1, parlay::random r=parlay::random(0)) {
  // Perhaps use a serial base case for small enough inputs?

  if (num_samples == -1) {
    num_samples = sqrt(n);
  }

}

