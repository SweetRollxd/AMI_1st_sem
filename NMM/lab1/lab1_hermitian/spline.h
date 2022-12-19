#ifndef SPLINE_H
#define SPLINE_H
#include <vector>
#include <Eigen/Dense>
#include <gnuplot-iostream.h>

#define BASIS_FUNCTIONS_COUNT 4
#define MAX_DEVIATION_TIMES 2

typedef std::pair<float, float> Point;

struct SplinePoint {
    float x;
    float y;
    float weight;

    SplinePoint(float x_in, float y_in, float w_in = 1);
};

struct Element {
    float n1;
    float n2;
    std::vector<SplinePoint> values;

//    std::ostream& operator<< (std::ostream &os);
//    std::ostream& operator<< (std::ostream &os, const Element &elem);
};

class Spline
{
    float regularizationAlfa;
    std::vector<Element> elements;
    std::vector<float> splineWeights;
    float basisFunc(float x1, float x2, float x, int func_index);
    Eigen::Matrix4f regularizationAlfaMatrix(float x1, float x2);
    std::vector<float> findSplineWeightsVector();
public:
    Spline(std::vector<Point> sourcePoints, std::vector<float> nodes, float regAlfa = 1);
    std::vector<Point> getSplineData(int splinePointsNumber = 1000);
    void filterSpline();
};

#endif // SPLINE_H
