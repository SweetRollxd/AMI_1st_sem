#include "fem.h"

// Рисование картинки ^^
void FEM::draw(unsigned int width, unsigned int height, unsigned int num_isolines, bool need_grid,
               double x0, double y0, double x1, double y1)
{
    cout << "Drawing picture..." << endl;
    // Инициализируем все как следует
    class bmp24_file pic(width, height, "plot.bmp");
    //pic.set_vertical_mirror(true);

    // Ищем максимальные и минимальные значения, чтобы нормировать цвет
    double max_val = slae.q[0], min_val = slae.q[0];
    // И попутно находим максимальные и минимальные координаты
    double max_x = nodes[0].x, min_x = nodes[0].x;
    double max_y = nodes[0].y, min_y = nodes[0].y;
    for(unsigned int i = 1; i < slae.n; i++)
    {
        if(slae.q[i] > max_val)
            max_val = slae.q[i];
        if(slae.q[i] < min_val)
            min_val = slae.q[i];
        if(nodes[i].x > max_x)
            max_x = nodes[i].x;
        if(nodes[i].x < min_x)
            min_x = nodes[i].x;
        if(nodes[i].y > max_y)
            max_y = nodes[i].y;
        if(nodes[i].y < min_y)
            min_y = nodes[i].y;
    }
    // Проверка соответствия заданной области
    if(x0 > min_x) min_x = x0;
    if(y0 > min_y) min_y = y0;
    if(x1 < max_x) max_x = x1;
    if(y1 < max_y) max_y = y1;

    // Найдем шаг
    double step_x = (max_x - min_x) / (double)width;
    double step_y = (max_y - min_y) / (double)height;
    // Шаги для разбиения по цветовым областям
    double step_u_big = (max_val - min_val) / 4.0;
    double step_u_small = step_u_big / 256.0;

    cout << " > Calculating solution grid..." << endl;
    // Получим решение по сетке и занесем его в массив
    double ** sol_grid;
    // Выделим память
    sol_grid = new double * [height];
    // Цикл по вертикали
    //for(unsigned int i = 0; i < height; i++)
    #pragma omp parallel for schedule(dynamic)
    for(int i = 0; i < (int)height; i++)
    {
        // Выделим память
        sol_grid[i] = new double [width];
        // Цикл по горизонтали
        for(unsigned int j = 0; j < width; j++)
        {
            // Получаем решение в точке
            sol_grid[i][j] = get_solution(min_x + step_x * (double)j, min_y + step_y * (double)i);
        }
    }

    cout << " > Calculating rainbow colormap..." << endl;
    // Задаем пиксели изображения
    for(unsigned int i = 0; i < height; i++)
    {
        for(unsigned int j = 0; j < width; j++)
        {
            // Получаем решение в точке
            double curr_u = sol_grid[i][j];
            // Алгоритм заливки радугой (Rainbow colormap)
            unsigned short r_color = 0, g_color = 0, b_color = 0;
            if(curr_u > min_val + step_u_big * 3.0)
            {
                r_color = 255;
                g_color = 255 - (unsigned short)((curr_u - (min_val + step_u_big * 3.0)) / step_u_small);
                b_color = 0;
            }
            else if(curr_u > min_val + step_u_big * 2.0)
            {
                r_color = (unsigned short)((curr_u - (min_val + step_u_big * 2.0)) / step_u_small);
                g_color = 255;
                b_color = 0;
            }
            else if(curr_u > min_val + step_u_big)
            {
                unsigned short tmp = (unsigned short)((curr_u - (min_val + step_u_big)) / step_u_small);
                r_color = 0;
                g_color = tmp;
                b_color = 255 - tmp;
            }
            else
            {
                unsigned short tmp = 76 - (unsigned short)((curr_u - min_val) / (step_u_small * (255.0 / 76.0)));
                r_color = tmp;
                g_color = 0;
                b_color = 255 - tmp;
            }
            pic.set_pixel(j, i, (BYTE)r_color, (BYTE)g_color, (BYTE)b_color);
        }
    }
    // Приглушаем кислотные цвета
    for(unsigned int i = 0; i < width; i++)
    {
        for(unsigned int j = 0; j < height; j++)
        {
            RGBTRIPLE col = pic.get_pixel(i, j);
            col.rgbtBlue  = (BYTE)((unsigned int)col.rgbtBlue  * 3 / 4 + 64);
            col.rgbtGreen = (BYTE)((unsigned int)col.rgbtGreen * 3 / 4 + 64);
            col.rgbtRed   = (BYTE)((unsigned int)col.rgbtRed   * 3 / 4 + 64);
            //pic.set_pixel(i, j, col);
            pic.set_pixel(i, j, (BYTE)255, (BYTE)255, (BYTE)255);
        }
    }

    // Сетка
    if(need_grid)
    {
        cout << " > Calculating grid lines..." << endl;
        //unsigned short color_grid = 200;
        unsigned short color_grid = 140;
        for(set<double>::iterator i = grid.grid_x.begin(); i != grid.grid_x.end(); i++)
        {
            unsigned int coord = (unsigned int)((* i - min_x) / step_x);
            for(unsigned int j = 0; j < height; j++)
                pic.set_pixel(coord, j, (BYTE)color_grid, (BYTE)color_grid, (BYTE)color_grid);
        }
        for(set<double>::iterator j = grid.grid_y.begin(); j != grid.grid_y.end(); j++)
        {
            unsigned int coord = (unsigned int)((* j - min_y) / step_y);
            for(unsigned int i = 0; i < width; i++)
                pic.set_pixel(i, coord, (BYTE)color_grid, (BYTE)color_grid, (BYTE)color_grid);
        }
    }

    cout << " > Calculating isolines..." << endl;
    // Зададим цвет изолиний
    unsigned short color_isolines = /*88*//*255*/0;
    // Посчитаем шаг, с которым рисовать изолинии
    double isolines_step = (max_val - min_val) / (double)(num_isolines + 1);

    // Цикл по изолиниям
    for(unsigned int k = 1; k <= num_isolines; k++)
    {
        // Получим значение, в котором нужно рисовать изолинию
        double isoline_val = min_val + isolines_step * (double)k;
        // Цикл по вертикали
        for(unsigned int i = 1; i < height - 1; i++)
        {
            // Цикл по горизонтали
            for(unsigned int j = 1; j < width - 1; j++)
            {
                // Если в точке i, j нужно отметить пиксель изолинии
                // Так линии будут потолще
                /*if((sol_grid[i - 1][j] > isoline_val && sol_grid[i + 1][j] < isoline_val) ||
                        (sol_grid[i - 1][j] < isoline_val && sol_grid[i + 1][j] > isoline_val) ||
                        (sol_grid[i][j - 1] > isoline_val && sol_grid[i][j + 1] < isoline_val) ||
                        (sol_grid[i][j - 1] < isoline_val && sol_grid[i][j + 1] > isoline_val))*/
                // А так - потоньше
                if((sol_grid[i][j] >= isoline_val && sol_grid[i + 1][j] < isoline_val) ||
                        (sol_grid[i][j] <= isoline_val && sol_grid[i + 1][j] > isoline_val) ||
                        (sol_grid[i][j] >= isoline_val && sol_grid[i][j + 1] < isoline_val) ||
                        (sol_grid[i][j] <= isoline_val && sol_grid[i][j + 1] > isoline_val))
                {
                    // Сделаем это
                    pic.set_pixel(j, i, (BYTE)color_isolines, (BYTE)color_isolines, (BYTE)color_isolines);
                }
            }
        }
    }

    // Области
    unsigned short color_areas = 0;
    for(unsigned int i = 0; i < grid.tsr.num_areas; i++)
    {
        for(unsigned int j = 0; j < grid.tsr.num_areas; j++)
        {
            if(grid.tsr.areas[i].n_mat != grid.tsr.areas[j].n_mat)
            {
                if(grid.tsr.areas[i].x0 == grid.tsr.areas[j].x1 &&
                        ((grid.tsr.areas[i].y0 >= grid.tsr.areas[j].y0 && grid.tsr.areas[i].y0 <= grid.tsr.areas[j].y1) ||
                         (grid.tsr.areas[i].y1 >= grid.tsr.areas[j].y0 && grid.tsr.areas[i].y1 <= grid.tsr.areas[j].y1)))
                {
                    double beg_, end_;
                    if(grid.tsr.areas[i].y0 > grid.tsr.areas[j].y0)
                        beg_ = grid.tsr.areas[i].y0;
                    else
                        beg_ = grid.tsr.areas[j].y0;
                    if(grid.tsr.areas[i].y1 < grid.tsr.areas[j].y1)
                        end_ = grid.tsr.areas[i].y1;
                    else
                        end_ = grid.tsr.areas[j].y1;
                    unsigned int coord_x = (unsigned int)((grid.tsr.areas[i].x0 - min_x) / step_x);
                    unsigned int coord_y_b = (unsigned int)((beg_ - min_y) / step_y);
                    unsigned int coord_y_e = (unsigned int)((end_ - min_y) / step_y);
                    for(unsigned int coord_y = coord_y_b; coord_y <= coord_y_e; coord_y++)
                    {
                        pic.set_pixel(coord_x, coord_y, (BYTE)color_areas, (BYTE)color_areas, (BYTE)color_areas);
                        pic.set_pixel(coord_x - 1, coord_y, (BYTE)color_areas, (BYTE)color_areas, (BYTE)color_areas);
                        pic.set_pixel(coord_x + 1, coord_y, (BYTE)color_areas, (BYTE)color_areas, (BYTE)color_areas);
                    }
                }
                if(grid.tsr.areas[i].x1 == grid.tsr.areas[j].x0 &&
                        ((grid.tsr.areas[i].y0 >= grid.tsr.areas[j].y0 && grid.tsr.areas[i].y0 <= grid.tsr.areas[j].y1) ||
                         (grid.tsr.areas[i].y1 >= grid.tsr.areas[j].y0 && grid.tsr.areas[i].y1 <= grid.tsr.areas[j].y1)))
                {
                    double beg_, end_;
                    if(grid.tsr.areas[i].y0 > grid.tsr.areas[j].y0)
                        beg_ = grid.tsr.areas[i].y0;
                    else
                        beg_ = grid.tsr.areas[j].y0;
                    if(grid.tsr.areas[i].y1 < grid.tsr.areas[j].y1)
                        end_ = grid.tsr.areas[i].y1;
                    else
                        end_ = grid.tsr.areas[j].y1;
                    unsigned int coord_x = (unsigned int)((grid.tsr.areas[i].x1 - min_x) / step_x);
                    unsigned int coord_y_b = (unsigned int)((beg_ - min_y) / step_y);
                    unsigned int coord_y_e = (unsigned int)((end_ - min_y) / step_y);
                    for(unsigned int coord_y = coord_y_b; coord_y <= coord_y_e; coord_y++)
                    {
                        pic.set_pixel(coord_x, coord_y, (BYTE)color_areas, (BYTE)color_areas, (BYTE)color_areas);
                        pic.set_pixel(coord_x - 1, coord_y, (BYTE)color_areas, (BYTE)color_areas, (BYTE)color_areas);
                        pic.set_pixel(coord_x + 1, coord_y, (BYTE)color_areas, (BYTE)color_areas, (BYTE)color_areas);
                    }
                }
                if(grid.tsr.areas[i].y0 == grid.tsr.areas[j].y1 &&
                        ((grid.tsr.areas[i].x0 >= grid.tsr.areas[j].x0 && grid.tsr.areas[i].x0 <= grid.tsr.areas[j].x1) ||
                         (grid.tsr.areas[i].x1 >= grid.tsr.areas[j].x0 && grid.tsr.areas[i].x1 <= grid.tsr.areas[j].x1)))
                {
                    double beg_, end_;
                    if(grid.tsr.areas[i].x0 > grid.tsr.areas[j].x0)
                        beg_ = grid.tsr.areas[i].x0;
                    else
                        beg_ = grid.tsr.areas[j].x0;
                    if(grid.tsr.areas[i].x1 < grid.tsr.areas[j].x1)
                        end_ = grid.tsr.areas[i].x1;
                    else
                        end_ = grid.tsr.areas[j].x1;
                    unsigned int coord_y = (unsigned int)((grid.tsr.areas[i].y0 - min_y) / step_y);
                    unsigned int coord_x_b = (unsigned int)((beg_ - min_x) / step_x);
                    unsigned int coord_x_e = (unsigned int)((end_ - min_x) / step_x);
                    for(unsigned int coord_x = coord_x_b; coord_x <= coord_x_e; coord_x++)
                    {
                        pic.set_pixel(coord_x, coord_y, (BYTE)color_areas, (BYTE)color_areas, (BYTE)color_areas);
                        pic.set_pixel(coord_x, coord_y - 1, (BYTE)color_areas, (BYTE)color_areas, (BYTE)color_areas);
                        pic.set_pixel(coord_x, coord_y + 1, (BYTE)color_areas, (BYTE)color_areas, (BYTE)color_areas);
                    }
                }
                if(grid.tsr.areas[i].y1 == grid.tsr.areas[j].y0 &&
                        ((grid.tsr.areas[i].x0 >= grid.tsr.areas[j].x0 && grid.tsr.areas[i].x0 <= grid.tsr.areas[j].x1) ||
                         (grid.tsr.areas[i].x1 >= grid.tsr.areas[j].x0 && grid.tsr.areas[i].x1 <= grid.tsr.areas[j].x1)))
                {
                    double beg_, end_;
                    if(grid.tsr.areas[i].x0 > grid.tsr.areas[j].x0)
                        beg_ = grid.tsr.areas[i].x0;
                    else
                        beg_ = grid.tsr.areas[j].x0;
                    if(grid.tsr.areas[i].x1 < grid.tsr.areas[j].x1)
                        end_ = grid.tsr.areas[i].x1;
                    else
                        end_ = grid.tsr.areas[j].x1;
                    unsigned int coord_y = (unsigned int)((grid.tsr.areas[i].y1 - min_y) / step_y);
                    unsigned int coord_x_b = (unsigned int)((beg_ - min_x) / step_x);
                    unsigned int coord_x_e = (unsigned int)((end_ - min_x) / step_x);
                    for(unsigned int coord_x = coord_x_b; coord_x <= coord_x_e; coord_x++)
                    {
                        pic.set_pixel(coord_x, coord_y, (BYTE)color_areas, (BYTE)color_areas, (BYTE)color_areas);
                        pic.set_pixel(coord_x, coord_y - 1, (BYTE)color_areas, (BYTE)color_areas, (BYTE)color_areas);
                        pic.set_pixel(coord_x, coord_y + 1, (BYTE)color_areas, (BYTE)color_areas, (BYTE)color_areas);
                    }
                }
            }
        }
    }

    // Вывод изображения в файл
    pic.write();

    // Почистим память
    for(unsigned int i = 0; i < height; i++)
        delete [] sol_grid[i];
    delete [] sol_grid;
}

void FEM::draw(unsigned int width, unsigned int height, unsigned int num_isolines, bool need_grid)
{
    draw(width, height, num_isolines, need_grid, -DBL_MAX, -DBL_MAX, DBL_MAX, DBL_MAX);
}
