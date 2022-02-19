#ifndef MAGNETICFLUID_H
#define MAGNETICFLUID_H

#include <map>
#include <cmath>



class MagneticFluid {

public:
    MagneticFluid(const double& eta, const double& c0, const double& tan_d0, const double& T);

    double eta_;
    double c0_;
    double tan_d0_;
    double T_;

    std::map<int, double> frequency;
    std::map<int, double> capacity;
    std::map<int, double> tan_d;
    std::map<int, double> re_epsilon;
    std::map<int, double> im_epsilon;
    std::map<double, double> radius_and_concentration;

    double ReEpsilon(const double& capacity);
    double ImEpsilon(const double& capacity, const double& tan_d);

    std::pair<std::map<double, double>,  std::map<double, int>> intervals_of_radius();

    double total_area = 0;

private:


    const double k = 1.380649e-23;
    const double pi = 3.14159265359;
};


double roundoff(const double& value, unsigned char prec);

#endif // MAGNETICFLUID_H
