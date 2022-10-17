#include "spline.h"

// Получение индекса в глобальной матрице
size_t FEM_spline::get_matrix_pos(unsigned int * nodes, size_t bf_num)
{
    bf_num--;
    size_t num1 = (size_t)(bf_num / 4);
    size_t num2 = bf_num - num1 * 4;
    return nodes[num1] * 4 + num2;
}

// Построение сплайна
void FEM_spline::make_spline()
{
    cout << "Making spline ..." << endl;
    fes_s = new fe_spline[fe_num];
    for(size_t i = 0; i < fe_num; i++)
        fes_s[i].init(finite_elements[i], nodes);

    generate_portrait_spline();

    // Узлы по которым берутся значения (в мастер-координатах)
    /*
    const size_t local_nodes_num = 4;
    const node local_nodes[local_nodes_num] =
    {
        node(0.0, 0.0),
        node(1.0, 0.0),
        node(0.0, 1.0),
        node(1.0, 1.0)
    };
    */
    /*
    const size_t local_nodes_num = 9;
    const node local_nodes[local_nodes_num] =
    {
        node(0.0, 0.0),
        node(0.5, 0.0),
        node(1.0, 0.0),
        node(0.0, 0.5),
        node(0.5, 0.5),
        node(1.0, 0.5),
        node(0.0, 1.0),
        node(0.5, 1.0),
        node(1.0, 1.0)
    };
    */
    const size_t local_nodes_num = 16;
    const node local_nodes[local_nodes_num] =
    {
        node(0.0, 0.0),
        node(1.0/3.0, 0.0),
        node(2.0/3.0, 0.0),
        node(1.0, 0.0),
        node(0.0, 1.0/3.0),
        node(1.0/3.0, 1.0/3.0),
        node(2.0/3.0, 1.0/3.0),
        node(1.0, 1.0/3.0),
        node(0.0, 2.0/3.0),
        node(1.0/3.0, 2.0/3.0),
        node(2.0/3.0, 2.0/3.0),
        node(1.0, 2.0/3.0),
        node(0.0, 1.0),
        node(1.0/3.0, 1.0),
        node(2.0/3.0, 1.0),
        node(1.0, 1.0)
    };

    // Цикл по КЭ
    for(size_t k = 0; k < fe_num; k++)
    {
        // Цикл по БФ 1
        for(size_t i = 1; i <= 16; i++)
        {
            size_t ii = get_matrix_pos(fes_s[k].node_n, i);
            // Цикл по БФ 2
            for(size_t j = 1; j < i; j++)
            {
                size_t jj = get_matrix_pos(fes_s[k].node_n, j);
                double sum = 0.0;
                // Цикл по точкам
                for(size_t m = 0; m < local_nodes_num; m++)
                {
                    node global_node = fes_s[k].to_global(local_nodes[m]);
                    sum += fes_s[k].phi(i, global_node) * fes_s[k].phi(j, global_node);
                }
                slae_spline.add(ii, jj, sum + alpha * fes_s[k].integrate_grad_phi(i, j) + beta * fes_s[k].integrate_lap_phi(i, j));
            }
            double sum_di = 0.0, sum_rp = 0.0;
            // Цикл по точкам
            for(size_t m = 0; m < local_nodes_num; m++)
            {
                node global_node = fes_s[k].to_global(local_nodes[m]);
                sum_di += fes_s[k].phi(i, global_node) * fes_s[k].phi(i, global_node);
                sum_rp += fes_s[k].phi(i, global_node) * get_solution_2(k, global_node);
            }
            slae_spline.di[ii] += sum_di + alpha * fes_s[k].integrate_grad_phi(i, i) + beta * fes_s[k].integrate_lap_phi(i, i);
            slae_spline.f[ii] += sum_rp;
        }
    }

    slae_spline.solve();
}

// Генерация портрета матрицы сплайна
void FEM_spline::generate_portrait_spline()
{
    slae_spline.n = node_num * 4; // на 4 узла 16 бф
    set<size_t> * portrait = new set<size_t>[slae_spline.n];

    for(size_t k = 0; k < fe_num; k++)
    {
        for(size_t i = 0; i < 4; i++)
        {
            for(size_t j = 0; j < 4; j++)
            {
                size_t qi = fes_s[k].node_n[i];
                size_t qj = fes_s[k].node_n[j];
                if(qj > qi) swap(qi, qj);
                if(qi != qj)
                {
                    for(size_t m = 0; m < 4; m++)
                    {
                        portrait[qi * 4].insert(qj * 4 + m);
                        portrait[qi * 4 + 1].insert(qj * 4 + m);
                        portrait[qi * 4 + 2].insert(qj * 4 + m);
                        portrait[qi * 4 + 3].insert(qj * 4 + m);
                    }
                }
                else
                {
                    portrait[qi * 4 + 1].insert(qj * 4);
                    portrait[qi * 4 + 2].insert(qj * 4);
                    portrait[qi * 4 + 2].insert(qj * 4 + 1);
                    portrait[qi * 4 + 3].insert(qj * 4);
                    portrait[qi * 4 + 3].insert(qj * 4 + 1);
                    portrait[qi * 4 + 3].insert(qj * 4 + 2);
                }
            }
        }
    }

    size_t gg_size = 0;
    for(size_t i = 0; i < slae_spline.n; i++)
        gg_size += portrait[i].size();

    slae_spline.alloc_all(gg_size);

    slae_spline.ig[0] = 0;
    slae_spline.ig[1] = 0;
    size_t tmp = 0;
    for(size_t i = 0; i < slae_spline.n; i++)
    {
        for(set<size_t>::iterator j = portrait[i].begin(); j != portrait[i].end(); j++)
        {
            slae_spline.jg[tmp] = *j;
            tmp++;
        }
        slae_spline.ig[i + 1] = slae_spline.ig[i] + portrait[i].size();

        portrait[i].clear();
    }

    delete [] portrait;
}

// Получение решения (до применения сплайна)
double FEM_spline::get_solution_2(size_t fe_sol, node pnt)
{
    double x = pnt.x;
    double y = pnt.y;

    // Вычисление шага
    double hx = fabs(nodes[finite_elements[fe_sol].node_n[1]].x - nodes[finite_elements[fe_sol].node_n[0]].x);
    double hy = fabs(nodes[finite_elements[fe_sol].node_n[2]].y - nodes[finite_elements[fe_sol].node_n[0]].y);

    // Находим линейные одномерные функции
    double X1 = (nodes[finite_elements[fe_sol].node_n[1]].x - x) / hx;
    double X2 = (x - nodes[finite_elements[fe_sol].node_n[0]].x) / hx;
    double Y1 = (nodes[finite_elements[fe_sol].node_n[2]].y - y) / hy;
    double Y2 = (y - nodes[finite_elements[fe_sol].node_n[0]].y) / hy;

    // Находим значение билинейных базисных функций
    double psi[4];
    psi[0] = X1 * Y1;
    psi[1] = X2 * Y1;
    psi[2] = X1 * Y2;
    psi[3] = X2 * Y2;

    // Линейная комбинация базисных функций на веса
    double result = 0.0;
    for(unsigned int i = 0; i < 4; i++)
        result += slae.q[finite_elements[fe_sol].node_n[i]] * psi[i];

    return result;
}

// Получение решения (после применения сплайна)
double FEM_spline::get_spline_solution(double x, double y)
{
    // Определение КЭ, в который попала точка
    bool finded = false;
    unsigned int fe_sol = 0;
    for(unsigned int i = 0; i < fe_num && !finded; i++)
    {
        if(x >= nodes[finite_elements[i].node_n[0]].x && x <= nodes[finite_elements[i].node_n[1]].x &&
                y >= nodes[finite_elements[i].node_n[0]].y && y <= nodes[finite_elements[i].node_n[2]].y)
        {
            finded = true;
            fe_sol = i;
        }
    }

    // Если не нашли, значит точка за пределами области
    if(!finded)
    {
        cerr << "Error: Target point is outside area!" << endl;
        return 0.0;
    }

    // Если нашли, то решение будет линейной комбинацией базисных функций на соответствующие веса
    double result = 0.0;
    for(size_t i = 1; i <= 16; i++)
        result += fes_s[fe_sol].phi(i, node(x, y)) * slae_spline.q[get_matrix_pos(fes_s[fe_sol].node_n, i)];

    return result;
}

// Получение модуля градиента решения (после применения сплайна)
double FEM_spline::get_spline_b(double x, double y)
{
    // Определение КЭ, в который попала точка
    bool finded = false;
    unsigned int fe_sol = 0;
    for(unsigned int i = 0; i < fe_num && !finded; i++)
    {
        if(x >= nodes[finite_elements[i].node_n[0]].x && x <= nodes[finite_elements[i].node_n[1]].x &&
                y >= nodes[finite_elements[i].node_n[0]].y && y <= nodes[finite_elements[i].node_n[2]].y)
        {
            finded = true;
            fe_sol = i;
        }
    }

    // Если не нашли, значит точка за пределами области
    if(!finded)
    {
        cerr << "Error: Target point is outside area!" << endl;
        return 0.0;
    }

    // Если нашли, то решение будет линейной комбинацией базисных функций на соответствующие веса
    double grad_x = 0.0;
    double grad_y = 0.0;
    for(size_t i = 1; i <= 16; i++)
    {
        size_t ii = get_matrix_pos(fes_s[fe_sol].node_n, i);
        size_t b1, b2;
        fes_s[fe_sol].two2one(i, b1, b2);
        grad_x += fes_s[fe_sol].hermit_func_first_der(b1, 'x', x) * fes_s[fe_sol].hermit_func(b2, 'y', y) * slae_spline.q[ii];
        grad_y += fes_s[fe_sol].hermit_func_first_der(b2, 'y', y) * fes_s[fe_sol].hermit_func(b1, 'x', x) * slae_spline.q[ii];
    }

    return sqrt(grad_x * grad_x + grad_y * grad_y);
}
