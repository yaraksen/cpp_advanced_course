#pragma once

#include "algo_impl.h"
#include <type_traits>

/// Use one of the following:
/// AlgoRes ComputeIntegral(data);
/// AlgoRes ComputeUnsignedIntegral(data);
/// AlgoRes ComputeFloat(data);
/// AlgoRes ComputeGeneral(data);

template <class T>
AlgoRes Run(T data) {
    if constexpr (std::is_same_v<T, int>) {
        return ComputeIntegral(data);
    } else if constexpr (std::is_same_v<T, double> || std::is_same_v<T, float>) {
        return ComputeFloat(data);
    } else if constexpr (std::is_same_v<T, unsigned int>) {
        return ComputeUnsignedIntegral(data);
    } else {
        return ComputeGeneral(data);
    }
}
