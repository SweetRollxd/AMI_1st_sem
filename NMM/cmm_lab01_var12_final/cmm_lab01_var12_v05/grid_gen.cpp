#include "fem.h"

// Генерация сетки
void grid_generator::grid_gen(string filename)
{
    cout << "Generating grid..." << endl;
    tsr.read_file(filename);
    double beg_, end_;

    // Генерация сетки по оси X
    end_ = tsr.x.left;
    for(unsigned int i = 0; i < tsr.x.num; i++)
    {
        beg_ = end_;
        end_ = tsr.x.elems[i].coord;
        double rb, re, sign = tsr.x.elems[i].h_sign;
        if(sign > 0) rb = beg_, re = end_;
        else         rb = end_, re = beg_;
        grid_x.insert(rb);
        double k = tsr.x.elems[i].h_k, h = tsr.x.elems[i].h_min * sign;
        if(tsr.double_x > 0.9)
        {
            k = sqrt(k);
            h = h / (1.0 + k);
        }
        if(tsr.double_x > 1.9)
        {
            k = sqrt(k);
            h = h / (1.0 + k);
        }
        double new_pos = rb + h;
        while(sign * new_pos < re * sign)
        {
            grid_x.insert(new_pos);
            h *= k;
            new_pos += h;
        }
        grid_x.insert(re);
    }

    // Генерация сетки по оси Y
    end_ = tsr.y.left;
    for(unsigned int i = 0; i < tsr.y.num; i++)
    {
        beg_ = end_;
        end_ = tsr.y.elems[i].coord;
        double rb, re, sign = tsr.y.elems[i].h_sign;
        if(sign > 0) rb = beg_, re = end_;
        else         rb = end_, re = beg_;
        grid_y.insert(rb);
        double k = tsr.y.elems[i].h_k, h = tsr.y.elems[i].h_min * sign;
        if(tsr.double_y > 0.9)
        {
            k = sqrt(k);
            h = h / (1.0 + k);
        }
        if(tsr.double_y > 1.9)
        {
            k = sqrt(k);
            h = h / (1.0 + k);
        }
        double new_pos = rb + h;
        while(sign * new_pos < re * sign)
        {
            grid_y.insert(new_pos);
            h *= k;
            new_pos += h;
        }
        grid_y.insert(re);
    }

    ofstream finite_el, nodes, bound1;

    // Вывод в файл узлов
    nodes.open("nodes.txt", ios::out);
    nodes << grid_x.size() * grid_y.size() << endl;
    for(set<double>::iterator j = grid_y.begin(); j != grid_y.end(); j++)
        for(set<double>::iterator i = grid_x.begin(); i != grid_x.end(); i++)
            nodes << * i << " \t" << * j << endl;
    nodes.flush();
    nodes.close();

    // Вывод в файл всех конечных элементов
    finite_el.open("finite_el.txt", ios::out);
    finite_el << (grid_x.size()-1) * (grid_y.size()-1) << endl;
    double mu0 = 4.0 * M_PI * 1.0e-7;
    set<double>::iterator j_it = grid_y.begin();
    for(unsigned int j = 0; j < grid_y.size() - 1; j++)
    {
        set<double>::iterator i_it = grid_x.begin();
        for(unsigned int i = 0; i < grid_x.size() - 1; i++)
        {
            // Определение номера в глобальной нумерации
            unsigned int num[4];
            num[0] = grid_x.size() * j + i;
            num[1] = grid_x.size() * j + i + 1;
            num[2] = grid_x.size() * (j + 1) + i;
            num[3] = grid_x.size() * (j + 1) + i + 1;
            for(unsigned int k = 0; k < 4; k++)
                finite_el << num[k] << " \t";
            finite_el << endl;
            // Определение номера области
            double x0 = * i_it, y0 = * j_it;
            set<double>::iterator k = j_it;
            k++;
            i_it++;
            double x1 = * i_it, y1 = * k;
            unsigned int num_area = tsr.get_num_area(x0, y0, x1, y1);
            // Вывод параметров области
            finite_el << 1.0 / (tsr.areas[num_area].mu * mu0) << " \t" << 0 << endl;
            for(unsigned int k = 0; k < 4; k++)
                finite_el << tsr.areas[num_area].j << "\t";
            finite_el << endl;
        }
        j_it++;
    }
    finite_el.flush();
    finite_el.close();

    // Вывод в файл однородных первых краевых условий
    bound1.open("bound1.txt", ios::out);
    for(unsigned int j = 0; j < grid_y.size(); j++)
    {
        unsigned int num = grid_x.size() * (j + 1) - 1;
        bound1 << num << " \t" << 0.0 << endl;
        num = grid_x.size() * j;
        bound1 << num << " \t" << 0.0 << endl;
    }
    for(unsigned int i = 0; i < grid_x.size(); i++)
    {
        unsigned int num = grid_x.size() * (grid_y.size() - 1) + i;
        bound1 << num << " \t" << 0.0 << endl;
    }
    bound1.flush();
    bound1.close();
}
