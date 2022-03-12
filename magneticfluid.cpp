#include "magneticfluid.h"


MagneticFluid::MagneticFluid(const double& eta, const double& c0, const double& tan_d0, const double& T):
    eta_(eta), c0_(c0), tan_d0_(tan_d0), T_(T) {}


double MagneticFluid::ReEpsilon(const double &capacity) {
    if (capacity) {
        return capacity / c0_;
    } else {
        return c0_;
    }
}


double MagneticFluid::ImEpsilon(const double& capacity, const double& tan_d) {
    if(capacity && tan_d) {
        return ReEpsilon(capacity) * tan_d;
    } else {
        return tan_d0_ / c0_;
    }
}


std::map<double, int> MagneticFluid::radius() {
    std::map<double, int> rows;

    for (const auto& [row, value]: frequency) {
        if (!value) { continue;}
        double r = 0.5 * cbrt(2 * k * T_ / (value * eta_ * pi * 1e+3)) * 1e+9;
        double area = 0.5 * pi * im_epsilon[row] * im_epsilon[row];

        total_area += area;
        radius_and_concentration[r] = area;
        rows[r] = row;
    }

    return rows;
}

std::map<double, double> MagneticFluid::intervals(const int& count) {

    int count_ = count ? count : round(1 + 3.322 * log10(radius_and_concentration.size()));
    double intervallbreite = round(prev(radius_and_concentration.end())->first - radius_and_concentration.begin()->first) / count_;

    auto find_elements = [&](const double& r_cur) {
        double mean = 0;
        double score = 0;
        for (auto& [r, area]: radius_and_concentration) {
            if ((r_cur - 0.5 * intervallbreite) <= r && r < (r_cur + 0.5 * intervallbreite)) {
                ++score;
                area /= total_area;
                mean += area;
            }
        }

        return mean / (score? score : 1);
    };


    double r_current = radius_and_concentration.begin()->first;
    std::map<double, double> result;

    while (r_current <= radius_and_concentration.begin()->first + (count_ + 0.5) * intervallbreite) {
        result[r_current] = find_elements(r_current);
        r_current += intervallbreite;
    }

    return result;
}


double roundoff(const double& value, unsigned char prec) {
  float pow_10 = pow(10.0f, (float)prec);
  return round(value * pow_10) / pow_10;
}
