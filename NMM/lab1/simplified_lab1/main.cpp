#include <iostream>
#include <fstream>
#include <vector>
#include <string>
//#include <boost/>

#include <gnuplot-iostream.h>

using namespace std;

struct elem {
    int n1;
    int n2;
    vector<int> values;
};

void print_elem(elem element){
    cout << "[" << element.n1 << ", " << element.n2 << "]" << ": ";
    for (vector<int>::iterator it = element.values.begin(); it != element.values.end(); it++)
        cout << *it << ", ";
    cout << endl;
}

void draw_data(vector<int> vec) {
    Gnuplot gp;

    vector<pair<int, int>> plot_data;
    for (int i = 0; i  < vec.size(); i++){

        plot_data.push_back(std::pair(i, vec[i]));
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
//        cout << "Value: " << val << endl;
        for (int j = 0; j < elemArr.size(); j++){
            if (x >= elemArr[j].n1 && x <= elemArr[j].n2){
                elemArr[j].values.push_back(val);
            }
        }

    }
    dataFile.close();

    cout << "Filled elements:" << endl;
    for(auto it = elemArr.begin(); it != elemArr.end(); it++)
        print_elem(*it);

    draw_data(elemArr[0].values);
    return 0;
}
