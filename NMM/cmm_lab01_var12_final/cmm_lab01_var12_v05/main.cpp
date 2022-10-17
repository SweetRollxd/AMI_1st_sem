#include "spline/spline.h"
#include <cstdlib>

int main()
{
    class FEM_spline fem;
    fem.grid.grid_gen("telma_lin/sreda");
    fem.calc();

    fem.alpha = 1.0e-7;
    fem.beta = 1.0e-16;
    fem.make_spline();

    //fem.draw(700, 480, 24, false);
    //fem.draw(700, 480, 24, false, -4.25e-2, 0.0, 4.25e-2, 5.75e-2);
    fem.draw(640, (unsigned int)(640.0 * 5.75 / 8.5), 24, false, -4.25e-2, 0.0, 4.25e-2, 5.75e-2);
    //fem.FEM::draw(986, 677, 24, true, -4.25e-2, 0.0, 4.25e-2, 5.75e-2);

    cout << endl << "Target points:" << endl;
    ifstream ifs;
    ifs.open("telma_lin/Point");
    unsigned int num_points;
    ifs >> num_points;
    cout.setf(ios::scientific);
    for(unsigned int i = 0; i < num_points; i++)
    {
        double x, y;
        ifs >> x >> y;
        cout.precision(3);
        cout << "x =";
        cout.width(11);
        cout << x << "   y =";
        cout.width(11);
        cout << y << "   Az =";
        cout.precision(7);
        cout.width(15);
        cout << fem.get_solution(x, y) << "   |B| =";
        cout.precision(7);
        cout.width(15);
        cout << fem.get_b(x, y) << endl;
    }
    ifs.close();

    cout << endl << "Spline points:" << endl;
    ifs.open("telma_lin/Point");
    ifs >> num_points;
    cout.setf(ios::scientific);
    for(unsigned int i = 0; i < num_points; i++)
    {
        double x, y;
        ifs >> x >> y;
        cout.precision(3);
        cout << "x =";
        cout.width(11);
        cout << x << "   y =";
        cout.width(11);
        cout << y << "   Az =";
        cout.precision(7);
        cout.width(15);
        cout << fem.get_spline_solution(x, y) << "   |B| =";
        cout.precision(7);
        cout.width(15);
        cout << fem.get_spline_b(x, y) << endl;
    }
    ifs.close();

#ifdef _WIN32
    system("pause");
#endif // _WIN32
    return 0;
}

