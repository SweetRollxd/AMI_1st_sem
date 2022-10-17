#include "telma.h"

telma_sreda::telma_sreda()
{
    areas = NULL;
}

telma_sreda::~telma_sreda()
{
    if(areas) delete [] areas;
}

void telma_sreda::read_file(string filename)
{
    ifstream ifs;
    ifs.open(filename.c_str(), ios::in);

    ifs >> num_areas;
    areas = new class telma_sreda_area [num_areas];
    for(unsigned int i = 0; i < num_areas; i++)
    {
        ifs >> areas[i];
    }

    ifs >> x;
    ifs >> y;

    ifs >> double_x;
    ifs >> double_y;

    ifs.close();
}

unsigned int telma_sreda::get_num_area(double x0, double y0, double x1, double y1)
{
    for(unsigned int i = 0; i < num_areas; i++)
    {
        if(areas[i].x0 <= x0 && areas[i].x1 >= x1 &&
                areas[i].y0 <= y0 && areas[i].y1 >= y1)
            return i;
    }
    return 0;
}
