#include "spline.h"

// Одномерные эрмитовы базисные функции
double fe_spline::hermit_func(size_t func_n, char var, double x)
{
    double x0 = nodes[node_n[0]].x;
    double y0 = nodes[node_n[0]].y;
    double hx = fabs(nodes[node_n[3]].x - nodes[node_n[0]].x);
    double hy = fabs(nodes[node_n[3]].y - nodes[node_n[0]].y);

    double ksi = 0.0;
    double h_var = 0.0;

    switch(var)
    {
    case 'x' :
        ksi = (x-x0)/hx;
        h_var = hx;
        break;
    case 'y' :
        ksi = (x-y0)/hy;
        h_var = hy;
        break;
    };

    switch(func_n)
    {
    case 1:
        return 1.0 - 3.0*ksi*ksi + 2.0*ksi*ksi*ksi;
        break;
    case 2:
        return h_var * (ksi - 2.0*ksi*ksi + ksi*ksi*ksi);
        break;
    case 3:
        return 3.0*ksi*ksi - 2.0*ksi*ksi*ksi;
        break;
    case 4:
        return h_var*(-ksi*ksi + ksi*ksi*ksi);
        break;
    };
    cerr << "Unknown number detected!" << endl;
    return 0.0;
}

// Первые производные одномерных эрмитовых базисных функций
double fe_spline::hermit_func_first_der(size_t func_n, char var, double x)
{
    double x0 = nodes[node_n[0]].x;
    double y0 = nodes[node_n[0]].y;
    double hx = fabs(nodes[node_n[3]].x - nodes[node_n[0]].x);
    double hy = fabs(nodes[node_n[3]].y - nodes[node_n[0]].y);

    double ksi = 0.0;
    double h_var = 0.0;

    switch(var)
    {
    case 'x' :
        ksi = (x-x0)/hx;
        h_var = hx;
        break;
    case 'y' :
        ksi = (x-y0)/hy;
        h_var = hy;
        break;
    };

    switch(func_n)
    {
    case 1:
        return (-6.0*ksi + 6.0*ksi*ksi)/h_var;
        break;
    case 2:
        return (1.0 - 4.0*ksi + 3.0*ksi*ksi);
        break;
    case 3:
        return (6.0*ksi - 6.0*ksi*ksi)/h_var;
        break;
    case 4:
        return (-2.0*ksi + 3.0*ksi*ksi);
        break;
    };
    cerr << "Unknown number detected!" << endl;
    return 0.0;
}

// Вторые производные одномерных эрмитовых базисных функций
double fe_spline::hermit_func_second_der(size_t func_n, char var, double x)
{
    double x0 = nodes[node_n[0]].x;
    double y0 = nodes[node_n[0]].y;
    double hx = fabs(nodes[node_n[3]].x - nodes[node_n[0]].x);
    double hy = fabs(nodes[node_n[3]].y - nodes[node_n[0]].y);

    double ksi = 0.0;
    double h_var = 0.0;

    switch(var)
    {
    case 'x' :
        ksi = (x-x0)/hx;
        h_var = hx;
        break;
    case 'y' :
        ksi = (x-y0)/hy;
        h_var = hy;
        break;
    };

    switch(func_n)
    {
    case 1:
        return (-6.0 + 12.0*ksi)/(h_var*h_var);
        break;
    case 2:
        return (-4.0 + 6.0*ksi)/h_var;
        break;
    case 3:
        return (6.0 - 12.0*ksi)/(h_var*h_var);
        break;
    case 4:
        return (-2.0 + 6.0*ksi)/h_var;
        break;
    };
    cerr << "Unknown number detected!" << endl;
    return 0.0;
}

// Значения двумерных эрмитовых базисных функций
double fe_spline::phi(size_t func_n, double x, double y)
{
    switch(func_n)
    {
    case 1 :
        return hermit_func(1, 'x', x) * hermit_func(1, 'y', y);
        break;
    case 2 :
        return hermit_func(2, 'x', x) * hermit_func(1, 'y', y);
        break;
    case 3 :
        return hermit_func(1, 'x', x) * hermit_func(2, 'y', y);
        break;
    case 4 :
        return hermit_func(2, 'x', x) * hermit_func(2, 'y', y);
        break;
    case 5 :
        return hermit_func(3, 'x', x) * hermit_func(1, 'y', y);
        break;
    case 6 :
        return hermit_func(4, 'x', x) * hermit_func(1, 'y', y);
        break;
    case 7 :
        return hermit_func(3, 'x', x) * hermit_func(2, 'y', y);
        break;
    case 8 :
        return hermit_func(4, 'x', x) * hermit_func(2, 'y', y);
        break;
    case 9 :
        return hermit_func(1, 'x', x) * hermit_func(3, 'y', y);
        break;
    case 10 :
        return hermit_func(2, 'x', x) * hermit_func(3, 'y', y);
        break;
    case 11 :
        return hermit_func(1, 'x', x) * hermit_func(4, 'y', y);
        break;
    case 12 :
        return hermit_func(2, 'x', x) * hermit_func(4, 'y', y);
        break;
    case 13 :
        return hermit_func(3, 'x', x) * hermit_func(3, 'y', y);
        break;
    case 14 :
        return hermit_func(4, 'x', x) * hermit_func(3, 'y', y);
        break;
    case 15 :
        return hermit_func(3, 'x', x) * hermit_func(4, 'y', y);
        break;
    case 16 :
        return hermit_func(4, 'x', x) * hermit_func(4, 'y', y);
        break;
    }
    cerr << "Unknown number detected!" << endl;
    return 0.0;
}

// Значения двумерных эрмитовых базисных функций
double fe_spline::phi(size_t func_n, class node p)
{
    return phi(func_n, p.x, p.y);
}

// Лаплассиан двумерных эрмитовых базисных функций
double fe_spline::lap_phi(size_t func_n, double x, double y)
{
    switch(func_n)
    {
    case 1 :
        return hermit_func_second_der(1, 'x', x)
               * hermit_func(1, 'y', y)
               + hermit_func(1, 'x', x)
               * hermit_func_second_der(1, 'y', y);
        break;
    case 2 :
        return hermit_func_second_der(2, 'x', x)
               * hermit_func(1, 'y', y)
               + hermit_func(2, 'x', x)
               * hermit_func_second_der(1, 'y', y);
        break;
    case 3 :
        return hermit_func_second_der(1, 'x', x)
               * hermit_func(2, 'y', y)
               + hermit_func(1, 'x', x)
               * hermit_func_second_der(2, 'y', y);
        break;
    case 4 :
        return hermit_func_second_der(2, 'x', x)
               * hermit_func(2, 'y', y)
               + hermit_func(2, 'x', x)
               * hermit_func_second_der(2, 'y', y);
        break;
    case 5 :
        return hermit_func_second_der(3, 'x', x)
               * hermit_func(1, 'y', y)
               + hermit_func(3, 'x', x)
               * hermit_func_second_der(1, 'y', y);
        break;
    case 6 :
        return hermit_func_second_der(4, 'x', x)
               * hermit_func(1, 'y', y)
               + hermit_func(4, 'x', x)
               * hermit_func_second_der(1, 'y', y);
        break;
    case 7 :
        return hermit_func_second_der(3, 'x', x)
               * hermit_func(2, 'y', y)
               + hermit_func(3, 'x', x)
               * hermit_func_second_der(2, 'y', y);
        break;
    case 8 :
        return hermit_func_second_der(4, 'x', x)
               * hermit_func(2, 'y', y)
               + hermit_func(4, 'x', x)
               * hermit_func_second_der(2, 'y', y);
        break;
    case 9 :
        return hermit_func_second_der(1, 'x', x)
               * hermit_func(3, 'y', y)
               + hermit_func(1, 'x', x)
               * hermit_func_second_der(3, 'y', y);
        break;
    case 10 :
        return hermit_func_second_der(2, 'x', x)
               * hermit_func(3, 'y', y)
               + hermit_func(2, 'x', x)
               * hermit_func_second_der(3, 'y', y);
        break;
    case 11 :
        return hermit_func_second_der(1, 'x', x)
               * hermit_func(4, 'y', y)
               + hermit_func(1, 'x', x)
               * hermit_func_second_der(4, 'y', y);
        break;
    case 12 :
        return hermit_func_second_der(2, 'x', x)
               * hermit_func(4, 'y', y)
               + hermit_func(2, 'x', x)
               * hermit_func_second_der(4, 'y', y);
        break;
    case 13 :
        return hermit_func_second_der(3, 'x', x)
               * hermit_func(3, 'y', y)
               + hermit_func(3, 'x', x)
               * hermit_func_second_der(3, 'y', y);
        break;
    case 14 :
        return hermit_func_second_der(4, 'x', x)
               * hermit_func(3, 'y', y)
               + hermit_func(4, 'x', x)
               * hermit_func_second_der(3, 'y', y);
        break;
    case 15 :
        return hermit_func_second_der(3, 'x', x)
               * hermit_func(4, 'y', y)
               + hermit_func(3, 'x', x)
               * hermit_func_second_der(4, 'y', y);
        break;
    case 16 :
        return hermit_func_second_der(4, 'x', x)
               * hermit_func(4, 'y', y)
               + hermit_func(4, 'x', x)
               * hermit_func_second_der(4, 'y', y);
        break;
    }
    cerr << "Unknown number detected!" << endl;
    return 0.0;
}

// Лаплассиан двумерных эрмитовых базисных функций
double fe_spline::lap_phi(size_t func_n, class node p)
{
    return lap_phi(func_n, p.x, p.y);
}

// Инициализация эрмитовых КЭ из обычных
void fe_spline::init(const finite_element & fe, node * n)
{
    nodes = n;
    for(int i = 0; i < 4; i++)
    {
        node_n[i] = fe.node_n[i];
        f[i] = fe.f[i];
    }
    lambda = fe.lambda;
    gamma = fe.gamma;

    hx = nodes[node_n[3]].x - nodes[node_n[0]].x;
    hy = nodes[node_n[3]].y - nodes[node_n[0]].y;
    jacobian = hx * hy / 4.0;

    // https://ru.wikipedia.org/wiki/Список_квадратурных_формул

    static double g_a = sqrt((114.0 - 3.0 * sqrt(583.0)) / 287.0);
    static double g_b = sqrt((114.0 + 3.0 * sqrt(583.0)) / 287.0);
    static double g_c = sqrt(6.0 / 7.0);
    static double g_wa = 307.0 / 810.0 + 923.0 / (270.0 * sqrt(583.0));
    static double g_wb = 307.0 / 810.0 - 923.0 / (270.0 * sqrt(583.0));
    static double g_wc = 98.0 / 405.0;

    for(size_t i = 0; i < 4; i++)
    {
        gauss_weights[i] = g_wc;
        gauss_weights[i+4] = g_wa;
        gauss_weights[i+8] = g_wb;
    }

    double gauss_points_local[2][12] =
    {
        {-g_c, g_c, 0.0, 0.0, -g_a, g_a, -g_a, g_a, -g_b, g_b, -g_b, g_b},
        {0.0, 0.0, -g_c, g_c, -g_a, -g_a, g_a, g_a, -g_b, -g_b, g_b, g_b}
    };

    for(size_t i = 0; i < 12; i++)
        gauss_points[i] = to_global(node(gauss_points_local[0][i], gauss_points_local[1][i]));
}

// Получние номеров одномерных БФ из номера двумерной
void fe_spline::two2one(size_t two, size_t & one1, size_t & one2)
{
    one1 = 2 * ((size_t)((two - 1) / 4) % 2) + ((two - 1) % 2) + 1;
    one2 = 2 * (size_t)((two - 1) / 8) + ((size_t)((two - 1) / 2) % 2) + 1;
}

// Скалярное произведение градиентов двумерных эрмитовых базисных функций
double fe_spline::grad_phi(size_t bf1, size_t bf2, double x, double y)
{
    size_t b1, b2;
    two2one(bf1, b1, b2);
    double tmp1 = hermit_func_first_der(b1, 'x', x);
    double tmp2 = hermit_func(b1, 'x', x);
    double tmp3 = hermit_func_first_der(b2, 'y', y);
    double tmp4 = hermit_func(b2, 'y', y);
    double dx1 = tmp1 * tmp4;
    double dy1 = tmp3 * tmp2;
    two2one(bf2, b1, b2);
    tmp1 = hermit_func_first_der(b1, 'x', x);
    tmp2 = hermit_func(b1, 'x', x);
    tmp3 = hermit_func_first_der(b2, 'y', y);
    tmp4 = hermit_func(b2, 'y', y);
    double dx2 = tmp1 * tmp4;
    double dy2 = tmp3 * tmp2;
    return dx1 * dx2 + dy1 * dy2;
}

// Скалярное произведение градиентов двумерных эрмитовых базисных функций
double fe_spline::grad_phi(size_t bf1, size_t bf2, node p)
{
    return grad_phi(bf1, bf2, p.x, p.y);
}

// Перевод в систему координат мастер-элемента
node fe_spline::to_local(node p)
{
    double ksi = 2.0 * (p.x - nodes[node_n[0]].x) / hx - 1.0;
    double eta = 2.0 * (p.y - nodes[node_n[0]].y) / hy - 1.0;
    return node(ksi, eta);
}

// Перевод в глобальную систему координат
node fe_spline::to_global(node p)
{
    double x = (p.x + 1.0) * hx / 2.0 + nodes[node_n[0]].x;
    double y = (p.y + 1.0) * hy / 2.0 + nodes[node_n[0]].y;
    return node(x, y);
}

// Интеграл от скалярного произведения градиентов двумерных эрмитовых базисных функций
double fe_spline::integrate_grad_phi(size_t bf1, size_t bf2)
{
    double result = 0.0;
    for(int g = 0; g < 12; g++)
    {
        result += gauss_weights[g] * grad_phi(bf1, bf2, gauss_points[g]);
    }
    return result;
}

// Интеграл от лаплассиана двумерных эрмитовых базисных функций
double fe_spline::integrate_lap_phi(size_t bf1, size_t bf2)
{
    double result = 0.0;
    for(int g = 0; g < 12; g++)
    {
        result += gauss_weights[g] * lap_phi(bf1, gauss_points[g]) * lap_phi(bf2, gauss_points[g]);
    }
    return result;
}
