#include "spline.h"

SplinePoint::SplinePoint(float x_in, float y_in, float w_in){
    x = x_in;
    y = y_in;
    weight = w_in;
}

std::ostream& operator<< (std::ostream &os, const Element &elem){
    os << "[" << elem.n1 << ", " << elem.n2 << "]" << ": ";
    for (auto it = elem.values.begin(); it != elem.values.end(); it++)
        os << "(" << it->x << ", " << it->y << "), ";
    os << std::endl;
    return os;
}


Spline::Spline(std::vector<Point> sourcePoints, std::vector<float> nodes, float regAlfa)
{
    int nodeCount = nodes.size();
    int elemCount = nodeCount - 1;
    elements.resize(elemCount);

    for (int i = 0; i < nodes.size(); i++){
        if (i == 0)
          elements[i].n1 = nodes[i];
        else if (i == elemCount)
          elements[i-1].n2 = nodes[i];
        else {
          elements[i-1].n2 = nodes[i];
          elements[i].n1 = nodes[i];
        }
    }

    for (auto it = sourcePoints.begin(); it != sourcePoints.end(); it++){
        for (int j = 0; j < nodes.size(); j++){
            if (it->first >= elements[j].n1 && it->first <= elements[j].n2){
                if (j != elements.size() - 1 && it->first == elements[j].n2) continue; // координата, попадающая на узел, должна включиться только в последний узел
                elements[j].values.push_back(SplinePoint(it->first, it->second)); // вставляем в значения элемента преобразованную к SplinePoint точку
            }
        }
    }

    std::cout << "Filled elements:" << std::endl;
    for(auto it = elements.begin(); it != elements.end(); it++)
        std::cout << *it;

    regularizationAlfa = regAlfa;
    splineWeights = findSplineWeightsVector();

}

std::vector<Point> Spline::getSplineData(int splinePointsNumber)
{
    std::vector<Point> splineData;
    float xmin = elements.begin()->n1;
    float xmax = (elements.end() - 1)->n2;
    std::cout << "Xmin: " << xmin << ", Xmax: " << xmax << std::endl;
    float step = (xmax - xmin) / splinePointsNumber;

    for (float x = xmin; x < xmax; x = x + step){
        float y = 0;
        for (int i = 0; i < elements.size(); i++){
            if (x >= elements[i].n1 && x <= elements[i].n2){
                for (int nu = 0; nu < BASIS_FUNCTIONS_COUNT; nu++)
                    y += splineWeights[2 * i + nu] * basisFunc(elements[i].n1, elements[i].n2, x, nu);
            }
        }
        splineData.push_back(Point(x, y));
    }

    return splineData;
}

void Spline::filterSpline()
{
//    std::vector<float> qVector;
    std::vector<std::vector<float>> deltaVector;
    deltaVector.resize(elements.size());
    int countDelta = 0;
    do {
        float deltaSumm = 0;
        splineWeights = findSplineWeightsVector();
        int point_count = 0;
        for (int i = 0; i < elements.size(); i++){
            deltaVector[i].resize(elements[i].values.size());
            int j = 0;
            for (auto dataInElement = elements[i].values.begin(); dataInElement != elements[i].values.end(); dataInElement++){
                point_count++;
                float splineY = 0;
                for (int nu = 0; nu < BASIS_FUNCTIONS_COUNT; nu++)
                    splineY += splineWeights[2 * i + nu] * basisFunc(elements[i].n1, elements[i].n2, dataInElement->x, nu);
                float delta = abs(splineY - dataInElement->y);

                deltaVector[i][j] = delta;
                deltaSumm += delta;
                j++;
            }
        }

        countDelta = 0;
        float avgDelta = deltaSumm / point_count;
        std::cout << "Average delta: " << avgDelta << std::endl;
        for(int i = 0; i < deltaVector.size(); i++){
            for(int j = 0; j < deltaVector[i].size(); j++){
                if (deltaVector[i][j] >= avgDelta * MAX_DEVIATION_TIMES && elements[i].values[j].weight == 1){
                    countDelta++;
                    elements[i].values[j].weight /= MAX_DEVIATION_TIMES;
                }
            }
        }
        std::cout << "Found " << countDelta << " elements with delta exceeding average delta" << std::endl;
    }
    while(countDelta > 0);
}

// функция расчета базисной функции
// x1 - первая X-координата элемента
// x2 - вторая X-координата
// x - X-координата в промежутке между x1 и x2
// func_index - индекс базисной функции (при нуле пси1, иначе пси2)
float Spline::basisFunc(float x1, float x2, float x, int func_index){
    float h = x2 - x1;
    float ksi = (x - x1) / h;
    switch(func_index){
        case 0: return 1 - 3 * pow(ksi, 2) + 2 * pow(ksi, 3);
        case 1: return ksi - 2 * pow(ksi, 2) + pow(ksi, 3);
        case 2: return 3 * pow(ksi, 2) - 2 * pow(ksi, 3);
        case 3: return -pow(ksi, 2) + pow(ksi, 3);
    default: throw ("Incorrect index of Basis function");
    }

//    if (func_index == 0) return (x2 - x) / (x2 - x1);
    //    else return (x - x1) / (x2 - x1);
}

Eigen::Matrix4f Spline::regularizationAlfaMatrix(float x1, float x2)
{

    float h = x2 - x1;
    Eigen::Matrix4f matrix;
    matrix << 36, 3*h, -36, 3*h,
              3*h, 4*pow(h,2), -3*h, -pow(h,2),
              -36, -3*h, 36, -3*h,
              3*h, -pow(h,2), -3*h, 4*pow(h,2);
    return 1/(30*h) * matrix;
}

std::vector<float> Spline::findSplineWeightsVector()
{
    int nodeCount = elements.size() + 1;
    Eigen::MatrixXf matrixA(nodeCount*2, nodeCount*2);
    Eigen::VectorXf vectorB(nodeCount*2);
    matrixA.setZero();
    vectorB.setZero();

    // рассчет матрицы A
    for (int i = 0; i < elements.size(); i++){
        Eigen::Matrix4f regMatrix = regularizationAlfaMatrix(elements[i].n1, elements[i].n2);
        for (int nu = 0; nu < BASIS_FUNCTIONS_COUNT; nu++){
            for (int mu = 0; mu < BASIS_FUNCTIONS_COUNT; mu++){
                float matrixCell = 0;
                for (auto dataInElement = elements[i].values.begin(); dataInElement != elements[i].values.end(); dataInElement++){
                    // рассчет ячейки матрицы
                    float psinu = basisFunc(elements[i].n1, elements[i].n2, dataInElement->x, nu);
                    float psimu = basisFunc(elements[i].n1, elements[i].n2, dataInElement->x, mu);
                    matrixCell += dataInElement->weight * psinu * psimu;
                }
                matrixA(2 * i + nu, 2 * i + mu) += matrixCell + regularizationAlfa * regMatrix(nu, mu);

            }
        }
    }
    std::cout << "Global matrix A:" << std::endl;
    std::cout << matrixA << std::endl;
    // рассчет вектора b
    for (int i = 0; i < elements.size(); i++){
        for (int nu = 0; nu < BASIS_FUNCTIONS_COUNT; nu++){
            for (auto dataInElement = elements[i].values.begin(); dataInElement != elements[i].values.end(); dataInElement++){
                // рассчет ячейки вектора
                float psinu = basisFunc(elements[i].n1, elements[i].n2, dataInElement->x, nu);
                vectorB(2 * i + nu) += dataInElement->weight * psinu * dataInElement->y;
            }

        }
    }
    std::cout << "Global vector b:" << std::endl;
    std::cout << vectorB << std::endl;

    Eigen::VectorXf resVector = matrixA.colPivHouseholderQr().solve(vectorB);
    std::cout << "Result Q vector:" << resVector << std::endl;
    std::vector<float> qVector(resVector.data(), resVector.data() + resVector.size());

    return qVector;
}

