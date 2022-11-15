#include <iostream>
#include <fstream>
#include <vector>
#include <string>
//#include <boost/>

#include <gnuplot-iostream.h>

using namespace std;

struct point {
    float x;
    float y;
};

struct elem {
    int n1;
    int n2;
    vector<point> values;
};

float baseFunc(float x1, float x2, float x, float func_index){
//     cout << "x1 " << x1 << ", x2 " << x2 << ", x " << x << ", func_index = " << func_index << endl;
     if (func_index == 0) return (x2 - x) / (x2 - x1);
             else return (x - x1) / (x2 - x1);
};

void print_elem(elem element){
    cout << "[" << element.n1 << ", " << element.n2 << "]" << ": ";
    for (vector<point>::iterator it = element.values.begin(); it != element.values.end(); it++)
        cout << "(" << it->x << ", " << it->y << "), ";
    cout << endl;
}

//void print_matrix(int size, int [size][size]){

//}

void draw_data(vector<point> vec) {
    Gnuplot gp;

    vector<pair<float,float>> plot_data;
    for (int i = 0; i  < vec.size(); i++){
//        point pnt;
//        pnt.x = i; pnt.y = vec[i];
        plot_data.push_back(pair<float,float>(vec[i].x, vec[i].y));
    }
    gp << "plot '-' with points'\n";
    gp.send1d(plot_data);

}


int main()
{
    string buf;
    ifstream splineFile("../simplified_lab1/spline.txt");

//    if (splineFile.is_open()){
//        cout << "File is opened\n";
        getline(splineFile, buf);
        int elemCount = stoi(buf) - 1;
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
    for (int i = 0; i < size; i++){
        getline(dataFile, buf);

        int space_pos = buf.find(' ');
        int x = stoi(buf.substr(0, space_pos));
        int val = stoi(buf.substr(space_pos, buf.length() - 1));
        point pnt;
        pnt.x = x; pnt.y = val;
//        cout << "Value: " << val << endl;
        for (int j = 0; j < elemArr.size(); j++){
            if (x >= elemArr[j].n1 && x <= elemArr[j].n2){
                if (j != elemArr.size() - 1 &&  x == elemArr[j].n2) continue;
                elemArr[j].values.push_back(pnt);
            }
        }

    }
    dataFile.close();
    // TODO: граничные элементы должны попадать только в один из интервалов
    cout << "Filled elements:" << endl;
    for(auto it = elemArr.begin(); it != elemArr.end(); it++)
        print_elem(*it);
//    elemArr
    float globalMatrix[3][3];
    float globalVector[3];
    // зануляем всю матрицу
    for (int i = 0; i < elemArr.size() + 1; i++)
        for (int j = 0; j < elemArr.size() + 1; j++)
            globalMatrix[i][j] = 0;

    for (int i = 0; i < elemArr.size(); i++){
        for (int nu = 0; nu < 2; nu++){
            for (int mu = 0; mu < 2; mu++){
                float matrixCell = 0;
                for (auto dataInElement = elemArr[i].values.begin(); dataInElement != elemArr[i].values.end(); dataInElement++){
                    // рассчет локальной матрицы A
                    float psinu = baseFunc(elemArr[i].n1, elemArr[i].n2, dataInElement->x, nu);
                    float psimu = baseFunc(elemArr[i].n1, elemArr[i].n2, dataInElement->x, mu);
                    matrixCell += 1 * psinu * psimu;
                }
                if (nu == mu) globalMatrix[i + nu][i + mu] += matrixCell;
                else globalMatrix[i + nu][i + mu] = matrixCell;
            }
        }
    }
    //TODO: добавить расчет вектора b
    cout << "Global matrix " << i << ":" << endl;
    for (int i = 0; i < elemArr.size() + 1; i++){
        for (int j = 0; j < elemArr.size() + 1; j++){
            cout << globalMatrix[i][j] << " ";
        }
        cout << endl;
    }

    /*
    for (int i = 0; i < elemArr.size(); i++){
        float localMatrix[2][2];
        float localVector[2];
        for (int nu = 0; nu < 2; nu++){
            bool vectorCalculated = false;
            float vectorCell = 0;
            for (int mu = 0; mu < 2; mu++){

                float matrixCell = 0;
                for (auto dataInElement = elemArr[i].values.begin(); dataInElement != elemArr[i].values.end(); dataInElement++){
                    // рассчет локальной матрицы A
                    float psinu = baseFunc(elemArr[i].n1, elemArr[i].n2, dataInElement->x, nu);
                    float psimu = baseFunc(elemArr[i].n1, elemArr[i].n2, dataInElement->x, mu);
//                    cout << "Psinu " << psinu << ", Psimu " << psimu << endl;
                    matrixCell += 1 * psinu * psimu;

                    // расчет локального вектора b
                    if (!vectorCalculated)
                        vectorCell += 1 * psinu * dataInElement->y;
                }
//                cout << "Matrix cell " << "[" << nu << "][" << mu << "] = " << matrixCell << endl;
                localMatrix[nu][mu] = matrixCell;
            }
            localVector[nu] = vectorCell;

        }
        cout << "Local matrix " << i << ":" << endl;
        for (int m = 0; m < 2; m++){
            for (int n = 0; n < 2; n++){
                cout << localMatrix[m][n] << " ";
            }
            cout << endl;
        }
        for (int n = 0; n < 2; n++){
            for (int m = 0; m < 2; m++){
                // здесь неправильно. Надо соединять по принципу, что крайние элементы складываются а те, у которых совпадают номера базисных функций
                if (n == 0 && m == 0) globalMatrix[i+n][i+m] += localMatrix[n][m];
                else globalMatrix[i+n][i+m] = localMatrix[n][m];
//                cout << "Global matrix cell " << "[" << i+n << "][" << i+m << "] = " << localMatrix[n][m] << endl;
            }
            cout << endl;
        }
    }

    for (int i = 0; i < elemArr.size() + 1; i++){
        for (int j = 0; j < elemArr.size() + 1; j++){
            cout << globalMatrix[i][j] << " ";
        }
        cout << endl;
    }
    */

//    draw_data(elemArr[0].values);
    return 0;
}
