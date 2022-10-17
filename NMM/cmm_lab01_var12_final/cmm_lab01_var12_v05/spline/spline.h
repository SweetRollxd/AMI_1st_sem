#ifndef SPLINE_H_INCLUDED
#define SPLINE_H_INCLUDED

#include "../fem.h"

class fe_spline: public finite_element
{
public:
    node * nodes;
    // Значения двумерных эрмитовых базисных функций
    double phi(size_t func_n, double x, double y);
    double phi(size_t func_n, class node p);
    // Лаплассиан двумерных эрмитовых базисных функций
    double lap_phi(size_t func_n, double x, double y);
    double lap_phi(size_t func_n, class node p);
    // Инициализация эрмитовых КЭ из обычных
    void init(const finite_element & fe, node * n);
    // Скалярное произведение градиентов двумерных эрмитовых базисных функций
    double grad_phi(size_t bf1, size_t bf2, double x, double y);
    double grad_phi(size_t bf1, size_t bf2, node p);
    // Интеграл от скалярного произведения градиентов двумерных эрмитовых базисных функций
    double integrate_grad_phi(size_t bf1, size_t bf2);
    // Интеграл от лаплассиана двумерных эрмитовых базисных функций
    double integrate_lap_phi(size_t bf1, size_t bf2);
    // Получние номеров одномерных БФ из номера двумерной
    void two2one(size_t two, size_t & one1, size_t & one2);
    // Одномерные эрмитовы базисные функции
    double hermit_func(size_t func_n, char var, double x);
    // Первые производные одномерных эрмитовых базисных функций
    double hermit_func_first_der(size_t func_n, char var, double x);
    // Вторые производные одномерных эрмитовых базисных функций
    double hermit_func_second_der(size_t func_n, char var, double x);
    // Перевод в систему координат мастер-элемента
    node to_local(node p);
    // Перевод в глобальную систему координат
    node to_global(node p);
private:
    // Геометрия КЭ и якобиан
    double hx, hy, jacobian;
    // Веса Гаусса
    double gauss_weights[12];
    // Точки Гаусса
    node gauss_points[12];
};

// Сплайн
class FEM_spline: public FEM
{
public:
    // СЛАУ для сплайна
    class SLAE slae_spline;
    // Построение сплайна
    void make_spline();
    // Генерация портрета матрицы сплайна
    void generate_portrait_spline();
    // Коэффициенты регуляризации
    double alpha;
    double beta;
    // Получение решения (до применения сплайна)
    double get_solution_2(size_t fe_sol, node pnt);
    // Получение решения (после применения сплайна)
    double get_spline_solution(double x, double y);
    // Получение модуля градиента решения (после применения сплайна)
    double get_spline_b(double x, double y);
    // Рисовалки
    void draw(unsigned int width, unsigned int height, unsigned int num_isolines, bool need_grid);
    void draw(unsigned int width, unsigned int height, unsigned int num_isolines, bool need_grid, double x0, double y0, double x1, double y1);
protected:
    // Эрмитовы КЭ
    fe_spline * fes_s;
    // Получение индекса в глобальной матрице
    size_t get_matrix_pos(unsigned int * nodes, size_t bf_num);
};

#endif // SPLINE_H_INCLUDED
