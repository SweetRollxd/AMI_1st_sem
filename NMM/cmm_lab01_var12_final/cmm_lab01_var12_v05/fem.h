#ifndef FEM_H_INCLUDED
#define FEM_H_INCLUDED

#ifdef _MSC_VER
#define _USE_MATH_DEFINES
#include <math.h>
#include <float.h>
#define isnan(x) _isnan(x)
#define isinf(x) (!_finite(x))
#else
#include <cmath>
#endif

#include <cstring>
#include <cfloat>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <list>
#include <set>
#include <omp.h>

#include "cgm.h"
#include "telma.h"
#include "bmp24_lib.h"
using namespace std;

// Used to express doubt or disapproval
class grid_generator
{
public:
    telma_sreda tsr;
    set <double> grid_x, grid_y;
    void grid_gen(string filename);
};

// Локальная матрица жесткости
static const double Gx[4][4] =
{
    {2.0, -2.0, 1.0, -1.0},
    {-2.0, 2.0, -1.0, 1.0},
    {1.0, -1.0, 2.0, -2.0},
    {-1.0, 1.0, -2.0, 2.0}
};

static const double Gy[4][4] =
{
    {2.0, 1.0, -2.0, -1.0},
    {1.0, 2.0, -1.0, -2.0},
    {-2.0, -1.0, 2.0, 1.0},
    {-1.0, -2.0, 1.0, 2.0}
};

// Локальная матрица массы
static const double M[4][4] =
{
    {4.0, 2.0, 2.0, 1.0},
    {2.0, 4.0, 1.0, 2.0},
    {2.0, 1.0, 4.0, 2.0},
    {1.0, 2.0, 2.0, 4.0}
};

// Локальная матрица массы для вторых краевых
static const double Mb[2][2] =
{
    {2.0, 1.0},
    {1.0, 2.0}
};

// Конечный элемент
class finite_element
{
public:
    unsigned int node_n[4]; // 4 номера узлов в каждом КЭ
    double lambda;          // лямбда в КЭ
    double gamma;           // гамма в КЭ
    double f[4];            // значение правой части в узлах
};

// Узел
class node
{
public:
    double x, y;    // координаты узла
    node() {}
    node(double x, double y) { this->x = x; this->y = y; }
};

// СЛАУ
class SLAE
{
public:
    double * gg, * di, * f, * q;
    unsigned int * ig, * jg;
    unsigned int n;
    SLAE();     // Конструктор
    ~SLAE();    // Деструктор
    void alloc_all(unsigned int gg_size);   // Выделение памяти
    void add(unsigned int i, unsigned int j, double elem);  // Добавление в матрицу элемента elem в позицию i,j
    void solve();   // Функция решения СЛАУ
    void clean();   // Обнуление gg, di и f
private:
    class CGM cgm;  // Решатель МСГ
};

// МКЭ
class FEM
{
public:
    class SLAE slae;    // СЛАУ
    virtual void calc();        // Основная функция, вызывающая все остальные
    double get_solution(double x, double y);    // Получение решения в произвольной точке
    double get_b(double x, double y);           // Получение значения B в произвольной точке
    void draw(unsigned int width, unsigned int height, unsigned int num_isolines, bool need_grid);  // Рисование картинки
    void draw(unsigned int width, unsigned int height, unsigned int num_isolines, bool need_grid, double x0, double y0, double x1, double y1);  // Рисование картинки
    class grid_generator grid;  // Генератор сетки
protected:
    class node * nodes; // Узлы
    class finite_element * finite_elements; // Конечные элементы
    unsigned int fe_num;    // Число КЭ
    unsigned int node_num;  // Число узлов
    void read();        // Чтение данных из файла и генерация портрета
    virtual void assembling_global();   // Сборка глобальной матрицы
    void applying_bound();      // Применение краевых условий
};

#endif // FEM_H_INCLUDED
