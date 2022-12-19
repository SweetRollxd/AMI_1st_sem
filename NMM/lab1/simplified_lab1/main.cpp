#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <Eigen/Dense>
//#include <boost/>

#include <gnuplot-iostream.h>

#define MAX_DEVIATION_TIMES 2

using namespace std;

struct Point {
    float x;
    float y;
    float weight;

    Point(float x_in, float y_in, float w_in = 1){
        x = x_in;
        y = y_in;
        weight = w_in;
    }

};

struct elem {
    int n1;
    int n2;
    vector<Point> values;

};

// функция расчета базисной функции
// x1 - первая X-координата элемента
// x2 - вторая X-координата
// x - X-координата в промежутке между x1 и x2
// func_index - индекс базисной функции (при нуле пси1, иначе пси2)
float baseFunc(float x1, float x2, float x, int func_index){
//     cout << "x1 " << x1 << ", x2 " << x2 << ", x " << x << ", func_index = " << func_index << endl;
     if (func_index == 0) return (x2 - x) / (x2 - x1);
             else return (x - x1) / (x2 - x1);
};

void print_elem(elem element){
    cout << "[" << element.n1 << ", " << element.n2 << "]" << ": ";
    for (vector<Point>::iterator it = element.values.begin(); it != element.values.end(); it++)
        cout << "(" << it->x << ", " << it->y << "), ";
    cout << endl;
}

vector<pair<float, float>> vectorPointsToPairs(vector<Point> pointValues){
    vector<pair<float,float>> pairValues;
    for (int i = 0; i < pointValues.size(); i++)
        pairValues.push_back(pair<float,float>(pointValues[i].x, pointValues[i].y));
//        cout << "First: " << pairValues[]

    return pairValues;
}

float getSplineValue(float x){

}

vector<float> findQVector(vector<elem> elements){
    int nodeCount = elements.size() + 1;
    Eigen::MatrixXf globalMatrix(nodeCount, nodeCount);
    Eigen::VectorXf globalVector(nodeCount);
    globalMatrix.setZero();
    globalVector.setZero();

    // рассчет матрицы A
    for (int i = 0; i < elements.size(); i++){
        for (int nu = 0; nu < 2; nu++){
            for (int mu = 0; mu < 2; mu++){
                float matrixCell = 0;
                for (auto dataInElement = elements[i].values.begin(); dataInElement != elements[i].values.end(); dataInElement++){
                    // рассчет ячейки матрицы
                    float psinu = baseFunc(elements[i].n1, elements[i].n2, dataInElement->x, nu);
                    float psimu = baseFunc(elements[i].n1, elements[i].n2, dataInElement->x, mu);
                    matrixCell += dataInElement->weight * psinu * psimu;
                }
                globalMatrix(i + nu, i + mu) += matrixCell;
            }
        }
    }
    cout << "Global matrix A:" << endl;
    cout << globalMatrix << endl;
    // рассчет вектора b
    for (int i = 0; i < elements.size(); i++){
        for (int nu = 0; nu < 2; nu++){
            for (auto dataInElement = elements[i].values.begin(); dataInElement != elements[i].values.end(); dataInElement++){
                // рассчет ячейки вектора
                float psinu = baseFunc(elements[i].n1, elements[i].n2, dataInElement->x, nu);
                globalVector(i + nu) += dataInElement->weight * psinu * dataInElement->y;
            }

        }
    }
    cout << "Global vector b:" << endl;
    cout << globalVector << endl;

    Eigen::VectorXf resVector = globalMatrix.colPivHouseholderQr().solve(globalVector);
    cout << "Result Q vector:" << resVector << endl;
    vector<float> qVector(resVector.data(), resVector.data() + resVector.size());

    return qVector;
}

int main()
{
    string buf;
    ifstream splineFile("../simplified_lab1/spline.txt");

//    if (splineFile.is_open()){
//        cout << "File is opened\n";
        getline(splineFile, buf);
        int nodeCount = stoi(buf);
        int elemCount = nodeCount - 1;
        vector<elem> elemArr(elemCount);

        cout << "Spline.txt:" << endl;
        int i = 0;
        while (getline (splineFile, buf)) {
          int val = stoi(buf);
          if (i == 0)
            elemArr[i].n1 = val;
          else if (i == elemCount)
            elemArr[i-1].n2 = val;
          else {
            elemArr[i-1].n2 = val;
            elemArr[i].n1 = val;
          }

          i++;
          cout << buf << endl;
        }

        cout << "Empty elements:" << endl;
        for(auto it = elemArr.begin(); it != elemArr.end(); it++)
            print_elem(*it);
//    }
//    else {
//        cout << "Error while reading spline file\n";
//    }

//    delete[] elemArr;
    // Close the file
    splineFile.close();

    // Считываем данные для элементов
    ifstream dataFile("../simplified_lab1/data.txt");
    getline(dataFile, buf);
    int size = stoi(buf);
    std::vector<Point> rawData;
    for (int i = 0; i < size; i++){
        getline(dataFile, buf);

        int space_pos = buf.find(' ');
        int x = stoi(buf.substr(0, space_pos));
        int val = stoi(buf.substr(space_pos, buf.length() - 1));
        Point pnt(x, val);
        rawData.push_back(pnt);
        for (int j = 0; j < elemArr.size(); j++){
            if (x >= elemArr[j].n1 && x <= elemArr[j].n2){
                if (j != elemArr.size() - 1 &&  x == elemArr[j].n2) continue;
                elemArr[j].values.push_back(pnt);
            }
        }

    }
    dataFile.close();
    cout << "Filled elements:" << endl;
    for(auto it = elemArr.begin(); it != elemArr.end(); it++)
        print_elem(*it);

    vector<float> qVector;
    vector<vector<float>> deltaVector;
    deltaVector.resize(elemCount);
    int countDelta = 0;
    do {
        float deltaSumm = 0;
        qVector = findQVector(elemArr);
        for (int i = 0; i < elemArr.size(); i++){
            deltaVector[i].resize(elemArr[i].values.size());
            int j = 0;
            for (auto dataInElement = elemArr[i].values.begin(); dataInElement != elemArr[i].values.end(); dataInElement++){
                float splineY = 0;
                for (int nu = 0; nu < 2; nu++)
                    splineY += qVector[i + nu] * baseFunc(elemArr[i].n1, elemArr[i].n2, dataInElement->x, nu);
                float delta = abs(splineY - dataInElement->y);

                deltaVector[i][j] = delta;
                deltaSumm += delta;
                j++;
            }

        }
        countDelta = 0;
        float avgDelta = deltaSumm / rawData.size();
        cout << "Average delta: " << avgDelta << endl;
        for(int i = 0; i < deltaVector.size(); i++){
            for(int j = 0; j < deltaVector[i].size(); j++){
                if (deltaVector[i][j] >= avgDelta * MAX_DEVIATION_TIMES && elemArr[i].values[j].weight == 1){
                    countDelta++;
                    elemArr[i].values[j].weight /= MAX_DEVIATION_TIMES;
                }
            }
        }
        cout << "Count delta: " << countDelta << endl;
    } while(countDelta > 0);
//    qVector.


    int splinePointsNumber = 1000;
    std::vector<Point> spline;
    float xmin = elemArr.begin()->n1;
    float xmax = (elemArr.end() - 1)->n2;
    cout << "Xmin: " << xmin << ", Xmax: " << xmax << endl;
    float step = (xmax - xmin) / splinePointsNumber;

    for (float x = xmin; x < xmax; x = x + step){
        float y = 0;
        for (int i = 0; i < elemArr.size(); i++){
            if (x >= elemArr[i].n1 && x <= elemArr[i].n2){
                for (int nu = 0; nu < 2; nu++)
                    y += qVector[i + nu] * baseFunc(elemArr[i].n1, elemArr[i].n2, x, nu);
            }
        }
//        cout << "x: " << x << ", y: " << y << endl;
        spline.push_back(Point(x, y));
    }
//    draw_data(rawData);

    Gnuplot gp;
    gp << "set grid\nset xrange [0:21]\nset yrange [0:20]\n";
    gp << "plot '-' with lines title 'spline', '-' with points ps 1 pt 7 title 'raw data'\n";
    gp.send1d(vectorPointsToPairs(spline));
    gp.send1d(vectorPointsToPairs(rawData));
    return 0;
}
