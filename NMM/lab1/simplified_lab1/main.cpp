#include <iostream>
#include <fstream>

using namespace std;

int main()
{

    string line;
    ifstream datafile("data.txt");
    if (datafile.is_open()){
        cout << "test";
        while (getline(datafile, line)){
            cout << line << "\n";
        }
//        datafile.getline();
        datafile.close();
    }
//    fp = fopen('data.txt', 'r');
    else cout << "Unable to open file!";
    return 0;
}
