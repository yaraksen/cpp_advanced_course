template <bool B, class T, class F>
struct If {
    static const int value = T::value;
};
template <class T, class F>
struct If<false, T, F> {
    static const int value = F::value;
};

template <long A>
struct Int {
    static const int value = A;
};

#define M ((left + right) / 2)
#define LR (left < right - 1)
#define MN (M * M < N)
#define RightBin BinSearch<M, right, N>
#define LeftBin BinSearch<left, M, N>

template <long left, long right, int N>
struct BinSearch {
    static const int value = If<LR, If<MN, RightBin, LeftBin>, Int<right> >::value;
};

template <int N>
struct Sqrt {
    static const int value = BinSearch<-1, N, N>::value;
};
