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
    for (int i = 0; i < elemArr.size(); i++){
        float localMatrix[2][2];
        for (int nu = 0; nu < 2; nu++){
            for (int mu = 0; mu < 2; mu++){

                float matrixCell = 0;
                // починить: вместо исков берутся значения данных
                for (auto dataInElement = elemArr[i].values.begin(); dataInElement != elemArr[i].values.end(); dataInElement++){

                    float psi1 = baseFunc(elemArr[i].n1, elemArr[i].n2, dataInElement->x, nu);
                    float psi2 = baseFunc(elemArr[i].n1, elemArr[i].n2, dataInElement->x, mu);

                    matrixCell += 1 * psi1 * psi2;
//                    cout << "Matrix cell " << "[" << nu << "][" << mu << "] = " << matrixCell << endl;
                }
                localMatrix[nu][mu] = matrixCell;
            }
        }
        cout << "Local matrix " << i << ":" << endl;
        for (int m = 0; m < 2; m++){
            for (int n = 0; n < 2; n++){
                cout << localMatrix[m][n] << " ";
            }
            cout << endl;
        }

    }

//    draw_data(elemArr[0].values);
    return 0;
}
