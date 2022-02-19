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


std::pair<std::map<double, double>,  std::map<double, int>> MagneticFluid::intervals_of_radius() {

    std::map<double, int> rows;

    for (const auto& [key, value]: frequency) {
        double r = 0.5 * cbrt(2 * k * T_ / (value * eta_ * pi * 1e+3)) * 1e+9;
        double area = 0.5 * pi * im_epsilon[key] * im_epsilon[key];

        total_area += area;
        radius_and_concentration[r] = area;
        rows[r] = key;
    }

    int count = round(1 + 3.322 * log10(radius_and_concentration.size()));
    double intervallbreite = round(prev(radius_and_concentration.end())->first - radius_and_concentration.begin()->first) / count;

    double r_current = radius_and_concentration.begin()->first;
    std::map<double, double> result;
    int score = 0;
    double con = 0;

    for (auto it = radius_and_concentration.begin(); it != radius_and_concentration.end(); ++it) {

        con = it->second / total_area;
        it->second = con;
        //int row = rows[it->first];

        if (it->first < (r_current + 0.5 * intervallbreite)) {
            result[r_current] += (con);
            ++score;
        } else {
             result[r_current] /= (score? score : 1);
             r_current += intervallbreite;
             result[r_current] += (con);
             score = 1;
        }
    }

    return std::make_pair(result, rows);
}


double roundoff(const double& value, unsigned char prec) {
  float pow_10 = pow(10.0f, (float)prec);
  return round(value * pow_10) / pow_10;
}
