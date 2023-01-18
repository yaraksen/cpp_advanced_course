#pragma once

#include <array>
#include "another_pow.h"

constexpr int Round(double det) {
    return (det >= 0) ? int(det + 0.5) : int(det - 0.5);
}

template <int N>
constexpr int determinant(const std::array<std::array<double, N>, N>& a) {
    double det = 1;

    double arr[N][N];
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            arr[i][j] = a[i][j];
        }
    }

    int f = 0;
    for (int col = 0; col < N; ++col) {
        double main_e = 999999999;
        for (int row = f; row < N; ++row) {
            if (arr[row][col] != 0) {
                main_e = arr[row][col];
                if (row != f) {
                    for (int i = 0; i < N; ++i) {
                        double tmp = arr[row][i];
                        arr[row][i] = arr[f][i];
                        arr[f][i] = tmp;
                    }
                    det *= -1;
                }
                break;
            }
        }
        if (main_e != 999999999) {
            for (int row = f + 1; row < N; ++row) {
                double coef = -arr[row][col] / main_e;
                for (int i = col; i < N; ++i) {
                    arr[row][i] += arr[f][i] * coef;
                }
            }
            ++f;
        }
    }

    for (int i = 0; i < N; ++i) {
        det *= arr[i][i];
    }

    return Round(det);
}

template <>
constexpr int determinant<1>(const std::array<std::array<double, 1>, 1>& a) {
    return a[0][0];
}

template <>
constexpr int determinant<2>(const std::array<std::array<double, 2>, 2>& a) {
    return a[0][0] * a[1][1] - a[1][0] * a[0][1];
}