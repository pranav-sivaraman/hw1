#include "parallel.h"

using namespace parlay;

// A serial implementation for checking correctness.
//
// Work = O(n)
// Depth = O(n)
template <class T, class F>
T scan_inplace_serial(T *A, size_t n, const F& f, T id) {
  T cur = id;
  for (size_t i=0; i<n; ++i) {
    T next = f(cur, A[i]);
    A[i] = cur;
    cur = next;
  }
  return cur;
}

// Parallel in-place prefix sums. Your implementation can allocate and
// use an extra n*sizeof(T) bytes of memory.
//
// The work/depth bounds of your implementation should be:
// Work = O(n)
// Depth = O(\log(n))
template <class T, class F>
T scan_inplace(T *A, size_t n, const F& f, T id) {
  T* L = (T*)malloc((n - 1) * sizeof(T));

  T total = scan_up(A, L, n, f, id);
  scan_down(A, L, n, f, id);

  free(L);
  return total;
}

template <class T, class F>
T scan_up(T *A , T *L, size_t n, const F& f, T id) {
  if (n <= 1e4) {
    return scan_inplace_serial(A, n, f, id);
  } else {
    T left, right;

    auto f1 = [&]() { left = scan_up(A, L, n / 2, f, id); };
    auto f2 = [&]() { right = scan_up(A + n / 2, L + n / 2 , n - n / 2, f, id); };
    par_do(f1, f2);
    L[n / 2 - 1] = left;
    return f(left, right);
  }
}

template <class T, class F>
void scan_down(T *R, T *L, size_t n, const F& f, T s) {
  if (n <= 1e4) {
    scan_inplace_serial(R, n, f, s);
  } else {
    auto f1 = [&]() { scan_down(R, L, n / 2, f, s); };
    auto f2 = [&]() { scan_down(R + n / 2, L + n / 2 , n - n / 2, f, f(s, L[n / 2 - 1])); };
    par_do(f1, f2);
  }
}
