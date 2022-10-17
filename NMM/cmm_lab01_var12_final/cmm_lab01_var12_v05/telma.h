#ifndef TELMA_H_INCLUDED
#define TELMA_H_INCLUDED

#include <iostream>
#include <fstream>
#include <string>
using namespace std;

class telma_sreda_area
{
public:
    double x0, x1, y0, y1;
    double mu;
    double j;
    unsigned int n_mat;
    friend istream & operator >> (istream & is, class telma_sreda_area & a)
    {
        is >> a.x0 >> a.x1 >> a.y0 >> a.y1 >> a.mu >> a.j >> a.n_mat;
        return is;
    }
};

class telma_sreda_grid_elem
{
public:
    double coord;
    double h_min;
    double h_k;
    short h_sign;
};

class telma_sreda_grid
{
public:
    double left;
    unsigned int num;
    class telma_sreda_grid_elem * elems;
    telma_sreda_grid()
    {
        elems = NULL;
    }
    ~telma_sreda_grid()
    {
        if(elems) delete [] elems;
    }
    friend istream & operator >> (istream & is, class telma_sreda_grid & g)
    {
        is >> g.left >> g.num;
        g.elems = new class telma_sreda_grid_elem [g.num];
        for(unsigned int i = 0; i < g.num; i++)
            is >> g.elems[i].coord;
        for(unsigned int i = 0; i < g.num; i++)
            is >> g.elems[i].h_min;
        for(unsigned int i = 0; i < g.num; i++)
            is >> g.elems[i].h_k;
        for(unsigned int i = 0; i < g.num; i++)
            is >> g.elems[i].h_sign;
        return is;
    }
};

class telma_sreda
{
public:
    void read_file(string filename);
    unsigned int get_num_area(double x0, double y0, double x1, double y1);
    telma_sreda();
    ~telma_sreda();

    unsigned int num_areas;
    class telma_sreda_area * areas;

    class telma_sreda_grid x, y;

    unsigned short double_x, double_y;
};

#endif // TELMA_H_INCLUDED
