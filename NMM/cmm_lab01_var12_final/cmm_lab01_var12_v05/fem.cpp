#include "fem.h"

// Основная функция, вызывающая все остальные
void FEM::calc()
{
    read();         // читаем данные и заполняем портрет
    assembling_global();    // Собираем глобальную матрицу
    applying_bound();       // Применяем краевые условия
    slae.solve();   // решаем СЛАУ

    // Выводим результат в файл
    ofstream solution_s;
    solution_s.open("solution.txt", ios::out);
    for(unsigned int i = 0; i < slae.n; i++)
    {
        solution_s << nodes[i].x << " \t"
                   << nodes[i].y << " \t"
                   << slae.q[i] << endl;
    }
    solution_s.flush();
    solution_s.close();
}

// Чтение данных из файла и генерация портрета
void FEM::read()
{
    cout << "Reading data..." << endl;
    ifstream node_s, fe_s;
    node_s.open("nodes.txt", ios::in);
    fe_s.open("finite_el.txt", ios::in);

    // Получаем число узлов
    node_s >> node_num;
    cout << " > Detected " << node_num << " nodes." << endl;
    nodes = new class node [node_num];

    // Считывание координат узлов
    for(unsigned int i = 0; i < node_num; i++)
    {
        node_s >> nodes[i].x >> nodes[i].y;
    }
    node_s.close();

    // Считывание числа КЭ
    fe_s >> fe_num;
    cout << " > Detected " << fe_num << " finite elements." << endl;
    finite_elements = new class finite_element [fe_num];
    // Чтение параметров конечных элементов
    for(unsigned int i = 0; i < fe_num; i++)
    {
        for(unsigned int j = 0; j < 4; j++)
        {
            fe_s >> finite_elements[i].node_n[j];
        }
        fe_s >> finite_elements[i].lambda >> finite_elements[i].gamma;
        for(unsigned int j = 0; j < 4; j++)
        {
            fe_s >> finite_elements[i].f[j];
        }
    }
    fe_s.close();

    // Формирование профиля (портрета)
    cout << "Generating profile..." << endl;
    unsigned int gg_size = 0;
    slae.n = node_num;
    // Создаем массив списков для хранения связей
    set<unsigned int> * profile = new set<unsigned int> [node_num];

    // Связь есть, если узлы принадлежат одному КЭ
    // Поэтому обходим конечные элементы и добавляем в список общие вершины
    for(unsigned int i = 0; i < fe_num; i++)
        for(unsigned int j = 0; j < 4; j++)
            for(unsigned int k = 0; k < j; k++)
                profile[finite_elements[i].node_n[k]].insert(finite_elements[i].node_n[j]);

    // Удаляем повторяющиеся записи в спиках и сортируем их, попутно считая размер матрицы
    for(unsigned int i = 0; i < node_num; i++)
    {
        gg_size += profile[i].size();
    }
    slae.alloc_all(gg_size);

    // Заполнение профиля (портрета)
    slae.ig[0] = 0;
    slae.ig[1] = 0;
    unsigned int tmp = 0;
    for(unsigned int i = 0; i < slae.n; i++)
    {
        unsigned int k = 0;
        for(unsigned int j = 0; j <= i; j++)
        {
            // Если есть связь между i и j, значит в этом месте матрицы будет ненулевой элемент
            // занесем информацию об этом в jg
            if(profile[j].find(i) != profile[j].end())
            {
                slae.jg[tmp] = j;
                tmp++;
                k++;
            }
        }
        // а в ig занесем информацию о количестве ненулевых элементов в строке
        slae.ig[i + 1] = slae.ig[i] + k;
    }

    // Очистка списков
    for(unsigned int i = 0; i < node_num; i++)
        profile[i].clear();
    delete [] profile;
}

// Сборка глобальной матрицы
void FEM::assembling_global()
{
    // Заполнение глобальной матрицы (без учета краевых)
    cout << "Generating global matrix..." << endl;
    for(unsigned int i = 0; i < fe_num; i++)
    {
        // вычисление шага
        double hx = fabs(nodes[finite_elements[i].node_n[1]].x - nodes[finite_elements[i].node_n[0]].x);
        double hy = fabs(nodes[finite_elements[i].node_n[2]].y - nodes[finite_elements[i].node_n[0]].y);

        // Вычисление констант, зависящих от параметров КЭ, на которые будут умножаться локальные матрицы
        double Gx_c = finite_elements[i].lambda / 6.0 * hy / hx;
        double Gy_c = finite_elements[i].lambda / 6.0 * hx / hy;
        double M1_c = hx * hy / 36.0;
        double M_c = finite_elements[i].gamma * M1_c;

        // Правая часть
        // Перемножим с локальной матрицей значение правой части в точках
        double fr[4];
        for(int j = 0; j < 4; j++)
        {
            fr[j] = 0.0;
            for(int k = 0; k < 4; k++)
            {
                fr[j] += M[j][k] * finite_elements[i].f[k];
            }
        }

        // Матрица
        for(unsigned int j = 0; j < 4; j++)
        {
            // Сперва для внедиагональных элементов
            for(unsigned int k = 0; k < j; k++)
            {
                // Найдем добавку
                double a = M_c * M[j][k] + Gx_c * Gx[j][k] + Gy_c * Gy[j][k];
                // И добавим ее
                if(finite_elements[i].node_n[j] > finite_elements[i].node_n[k])
                    slae.add(finite_elements[i].node_n[j], finite_elements[i].node_n[k], a);
                else
                    slae.add(finite_elements[i].node_n[k], finite_elements[i].node_n[j], a);
            }
            // Посчитаем добавку в диагональ и добавим ее
            double a = M_c * M[j][j] + Gx_c * Gx[j][j] + Gy_c * Gy[j][j];
            slae.di[finite_elements[i].node_n[j]] += a;
            // Добавим добавку в правую часть
            slae.f[finite_elements[i].node_n[j]] += M1_c * fr[j];
        }
    }
}

// Применение краевых условий
void FEM::applying_bound()
{
    ifstream bound1_s, bound2_s;
    bound1_s.open("bound1.txt", ios::in);
    bound2_s.open("bound2.txt", ios::in);

    // Счетчик, сколько краевых применено
    unsigned int counter = 0;

    // Применение вторых краевых условий
    while(bound2_s.good())
    {
        // Номера узлов грани, в которой задано второе краевое
        unsigned int node_beg, node_end;
        bound2_s >> node_beg >> node_end;
        // Значение тета в этих узлах
        double theta[2];
        bound2_s >> theta[0] >> theta[1];
        if(!bound2_s.good())
            break;
        // Вычисление шага
        double hx = fabs(nodes[node_beg].x - nodes[node_end].x);
        double hy = fabs(nodes[node_beg].y - nodes[node_end].y);
        // Определение, в какой плоскости задано краевое
        double h;
        if(hx > hy)
            h = hx;
        else
            h = hy;
        // Вычисление константы, на которую будет умножаться локальная матрица
        double Mb_c = h / 6.0;
        // Перемножим тета с локальной матрицей
        double fr[2];
        for(int j = 0; j < 2; j++)
        {
            fr[j] = 0.0;
            for(int k = 0; k < 2; k++)
            {
                fr[j] += Mb[j][k] * theta[k];
            }
        }
        // Добавим в правую часть
        slae.f[node_beg] += Mb_c * fr[0];
        slae.f[node_end] += Mb_c * fr[1];

        // Увеличим счетчик
        counter++;
    }
    bound2_s.close();
    cout << " > Applied " << counter << " boundary value of type II." << endl;

    // Счетчик, сколько краевых применено
    counter = 0;

    // Применение первых краевых условий
    while(bound1_s.good())
    {
        // Узел, в котором задано краевое
        unsigned int node_b1;
        // Значение краевого
        double value_b1;
        bound1_s >> node_b1 >> value_b1;
        if(!bound1_s.good())
            break;

        // Учет первых краевых "по-плохому"
        /*const double big_number = 1e30;
        // В диагональ пишем большое число
        slae.di[node_b1] = big_number;
        // В правую часть его же, умноженное на значение краевого условия
        slae.f[node_b1] = big_number * value_b1;*/

        // Учет первых краевых "по-хорошему"
        // В диагональ пишем 1
        slae.di[node_b1] = 1.0;
        // В правую часть пишем значение краевого
        slae.f[node_b1] = value_b1;
        // А вот тут все веселье
        // Нам надо занулить строку, а у нас симметричная матрица
        // Поэтому будем бегать по матрице, занулять стоки
        // А то, что было в столбцах - выкидывать в правую часть
        unsigned int i_s = slae.ig[node_b1], i_e = slae.ig[node_b1 + 1];
        for(unsigned int i = i_s; i < i_e; i++)
        {
            slae.f[slae.jg[i]] -= slae.gg[i] * value_b1;
            slae.gg[i] = 0.0;
        }
        for(unsigned int p = node_b1 + 1; p < node_num; p++)
        {
            unsigned int i_s = slae.ig[p], i_e = slae.ig[p + 1];
            for(unsigned int i = i_s; i < i_e; i++)
            {
                if(slae.jg[i] == node_b1)
                {
                    slae.f[p] -= slae.gg[i] * value_b1;
                    slae.gg[i] = 0.0;
                }
            }
        }

        // Увеличим счетчик
        counter++;
    }
    bound1_s.close();
    cout << " > Applied " << counter << " boundary value of type I." << endl;
}

// Получение решения в произвольной точке
double FEM::get_solution(double x, double y)
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

// Получение значения B в произвольной точке
double FEM::get_b(double x, double y)
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

    // Вычисление шага
    double hx = fabs(nodes[finite_elements[fe_sol].node_n[1]].x - nodes[finite_elements[fe_sol].node_n[0]].x);
    double hy = fabs(nodes[finite_elements[fe_sol].node_n[2]].y - nodes[finite_elements[fe_sol].node_n[0]].y);

    double Gx1_c = 1.0 / 6.0 * hy / hx;
    double Gx2_c = 1.0 / 6.0 * hx / hy;

    double mes = hx * hy;
    // B^2 = 1/mes * sumi sumj П[i][j]*qi*qj
    double B2 = 0.0;
    for(unsigned int k = 0; k < 4; k++)
        for(unsigned int j = 0; j < 4; j++)
            B2 += (Gx1_c * Gx[k][j] + Gx2_c * Gy[k][j]) * slae.q[finite_elements[fe_sol].node_n[k]] * slae.q[finite_elements[fe_sol].node_n[j]];
    B2 = B2 / mes;

    return sqrt(B2);
}

// Выделение памяти
void SLAE::alloc_all(unsigned int gg_size)
{
    // Выделение памяти
    ig = new unsigned int [n + 1];
    jg = new unsigned int [gg_size];
    di = new double [n];
    gg = new double [gg_size];
    f = new double [n];
    q = new double [n];
    // Обнуление всех массивов
    memset(di, 0, sizeof(double) * n);
    memset(f, 0, sizeof(double) * n);
    memset(q, 0, sizeof(double) * n);
    memset(gg, 0, sizeof(double) * gg_size);
}

// Обнуление gg, di и f
void SLAE::clean()
{
    memset(di, 0, sizeof(double) * n);
    memset(f, 0, sizeof(double) * n);
    memset(gg, 0, sizeof(double) * ig[n]);
}

// Конструктор
SLAE::SLAE()
{
    // Обнуление указателей (на всякий случай)
    ig = jg = NULL;
    di = gg = f = q = NULL;
}

// Деструктор
SLAE::~SLAE()
{
    // Освобождение памяти
    if(ig) delete [] ig;
    if(jg) delete [] jg;
    if(di) delete [] di;
    if(gg) delete [] gg;
    if(f) delete [] f;
    if(q) delete [] q;
}

// Добавление в матрицу элемента elem в позицию i,j
void SLAE::add(unsigned int i, unsigned int j, double elem)
{
    unsigned int ind = 0;
    bool flag = false;
    // Проходим по нужной строке
    for(unsigned int k = ig[i]; k < ig[i + 1] && !flag; k++)
    {
        // Ищем нужный столбец
        if(jg[k] == j)
        {
            ind = k;
            flag = true;
        }
    }
    // Добавляем
    gg[ind] += elem;
}

// Функция решения СЛАУ
void SLAE::solve()
{
    cout << "Solving SLAE..." << endl;
    cgm.init(ig, jg, di, gg, f, n);
    cgm.solve(q);
}
