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

// функция расчета базисной функции
// x1 - первая X-координата элемента
// x2 - вторая X-координата
// x - X-координата в промежутке между x1 и x2
// func_index - индекс базисной функции (при нуле пси1, иначе пси2)
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
    cout << "Filled elements:" << endl;
    for(auto it = elemArr.begin(); it != elemArr.end(); it++)
        print_elem(*it);
//    elemArr
    float globalMatrix[3][3];
    float globalVector[3];
    // зануляем всю матрицу и вектор
    for (int i = 0; i < elemArr.size() + 1; i++){
        globalVector[i] = 0;
        for (int j = 0; j < elemArr.size() + 1; j++)
            globalMatrix[i][j] = 0;
    }

    // рассчет матрицы A
    for (int i = 0; i < elemArr.size(); i++){
        for (int nu = 0; nu < 2; nu++){
            for (int mu = 0; mu < 2; mu++){
                float matrixCell = 0;
                for (auto dataInElement = elemArr[i].values.begin(); dataInElement != elemArr[i].values.end(); dataInElement++){
                    // рассчет ячейки матрицы
                    float psinu = baseFunc(elemArr[i].n1, elemArr[i].n2, dataInElement->x, nu);
                    float psimu = baseFunc(elemArr[i].n1, elemArr[i].n2, dataInElement->x, mu);
                    matrixCell += 1 * psinu * psimu;
                }
//                std::cout << "globalMatrix[" << i + nu << "][" << i + mu << "] = " << matrixCell << endl;
                globalMatrix[i + nu][i + mu] += matrixCell;
//                if (nu == mu) globalMatrix[i + nu][i + mu] += matrixCell;
//                else globalMatrix[i + nu][i + mu] = matrixCell;
            }
        }
    }
    cout << "Global matrix A:" << endl;
    for (int i = 0; i < elemArr.size() + 1; i++){
        for (int j = 0; j < elemArr.size() + 1; j++){
            cout << globalMatrix[i][j] << " ";
        }
        cout << endl;
    }
    // рассчет вектора b
    for (int i = 0; i < elemArr.size(); i++){
        for (int nu = 0; nu < 2; nu++){
            for (auto dataInElement = elemArr[i].values.begin(); dataInElement != elemArr[i].values.end(); dataInElement++){
                // рассчет ячейки вектора
                float psinu = baseFunc(elemArr[i].n1, elemArr[i].n2, dataInElement->x, nu);
                globalVector[i + nu] += 1 * psinu * dataInElement->y;
            }

        }
    }
    cout << "Global vector b:" << endl;
    for (int i = 0; i < elemArr.size() + 1; i++){
        cout << globalVector[i] << " ";
    }


    // TODO: добавить решение уравнения

//    draw_data(elemArr[0].values);
    return 0;
}
