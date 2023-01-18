#include "bad.h"
#include "ugly.h"
#include <vector>

struct SplineImpl {
    double *arr;
    const double *x, *y;
    size_t n;
    ~SplineImpl() {
        delete[] arr;
    }
};

Spline::Spline(const std::vector<double> &x, const std::vector<double> &y, double a, double b) {
    impl_ = std::make_shared<SplineImpl>();
    impl_->n = x.size();
    impl_->x = x.data();
    impl_->y = y.data();
    impl_->arr = new double[x.size()];
    mySplineSnd(impl_->x, impl_->y, impl_->n, a, b, impl_->arr);
}

double Spline::Interpolate(double x) {
    double arr;
    mySplintCube(impl_->x, impl_->y, impl_->arr, impl_->n, x, &arr);
    return arr;
}