#ifndef SPLINE_H_INCLUDED
#define SPLINE_H_INCLUDED

#include "../fem.h"

class fe_spline: public finite_element
{
public:
    node * nodes;
    // �������� ��������� ��������� �������� �������
    double phi(size_t func_n, double x, double y);
    double phi(size_t func_n, class node p);
    // ���������� ��������� ��������� �������� �������
    double lap_phi(size_t func_n, double x, double y);
    double lap_phi(size_t func_n, class node p);
    // ������������� ��������� �� �� �������
    void init(const finite_element & fe, node * n);
    // ��������� ������������ ���������� ��������� ��������� �������� �������
    double grad_phi(size_t bf1, size_t bf2, double x, double y);
    double grad_phi(size_t bf1, size_t bf2, node p);
    // �������� �� ���������� ������������ ���������� ��������� ��������� �������� �������
    double integrate_grad_phi(size_t bf1, size_t bf2);
    // �������� �� ����������� ��������� ��������� �������� �������
    double integrate_lap_phi(size_t bf1, size_t bf2);
    // �������� ������� ���������� �� �� ������ ���������
    void two2one(size_t two, size_t & one1, size_t & one2);
    // ���������� �������� �������� �������
    double hermit_func(size_t func_n, char var, double x);
    // ������ ����������� ���������� ��������� �������� �������
    double hermit_func_first_der(size_t func_n, char var, double x);
    // ������ ����������� ���������� ��������� �������� �������
    double hermit_func_second_der(size_t func_n, char var, double x);
    // ������� � ������� ��������� ������-��������
    node to_local(node p);
    // ������� � ���������� ������� ���������
    node to_global(node p);
private:
    // ��������� �� � �������
    double hx, hy, jacobian;
    // ���� ������
    double gauss_weights[12];
    // ����� ������
    node gauss_points[12];
};

// ������
class FEM_spline: public FEM
{
public:
    // ���� ��� �������
    class SLAE slae_spline;
    // ���������� �������
    void make_spline();
    // ��������� �������� ������� �������
    void generate_portrait_spline();
    // ������������ �������������
    double alpha;
    double beta;
    // ��������� ������� (�� ���������� �������)
    double get_solution_2(size_t fe_sol, node pnt);
    // ��������� ������� (����� ���������� �������)
    double get_spline_solution(double x, double y);
    // ��������� ������ ��������� ������� (����� ���������� �������)
    double get_spline_b(double x, double y);
    // ���������
    void draw(unsigned int width, unsigned int height, unsigned int num_isolines, bool need_grid);
    void draw(unsigned int width, unsigned int height, unsigned int num_isolines, bool need_grid, double x0, double y0, double x1, double y1);
protected:
    // �������� ��
    fe_spline * fes_s;
    // ��������� ������� � ���������� �������
    size_t get_matrix_pos(unsigned int * nodes, size_t bf_num);
};

#endif // SPLINE_H_INCLUDED
