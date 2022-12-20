#include <string>
#include <fstream>
#include <cmath>
#include <random>

#include <spline.h>

bool compare(Point p1, Point p2) {
    return p1.second<p2.second;
}

void drawSpline(std::vector<Point> data, Spline spline){
    auto splineData = spline.getSplineData();
    spline.filterSpline();
    std::vector<Point> splineFilteredData = spline.getSplineData();

    float xmin = data[0].first;
    float xmax = data[data.size() - 1].first;
    float ymin = std::min_element(data.begin(), data.end(), compare)->second;
    float ymax = std::max_element(data.begin(), data.end(), compare)->second;
    float xdelta = (xmax - xmin) / 8;
    float ydelta = (ymax - ymin) / 4;
    Gnuplot gp;
//    gp << "set terminal qt title 'Generated Data'\n";
    gp << "set grid\n \
           set xrange" << "[" << xmin-xdelta << ":" << xmax+xdelta << "]\n \
           set yrange" << "[" << ymin-ydelta << ":" << ymax+ydelta << "]\n";
    gp << "plot '-' with points ps 1 pt 7 title 'Raw data', \
                '-' with lines linecolor rgb 'red' lt 3 title 'Spline', \
                '-' with lines linecolor rgb 'blue' lt 1 title 'Filtered spline', \
                '-' with lines linecolor 10 dashtype 2 smooth acsplines title 'Gnuplot spline'\n";
    gp.send1d(data);
    gp.send1d(splineData);
    gp.send1d(splineFilteredData);
    gp.send1d(data);
}

void splineFromFile(std::ifstream& dataFile, std::ifstream& splineFile){
    std::string buf;
    getline(dataFile, buf);
    int size = stoi(buf);
    std::vector<Point> rawPoints;
    for (int i = 0; i < size; i++){
        getline(dataFile, buf);
        int space_pos = buf.find(' ');
        int x = stoi(buf.substr(0, space_pos));
        int val = stoi(buf.substr(space_pos, buf.length() - 1));
        rawPoints.push_back(Point(x, val));
    }

    getline(splineFile, buf);
    size = stoi(buf);
    std::vector<float> nodes;
    for (int i = 0; i < size; i++){
        getline(splineFile, buf);
        nodes.push_back(stof(buf));
    }

    Spline spline(rawPoints, nodes);
    drawSpline(rawPoints, spline);
}

void splineFromFunction(int pointsCount, int elementsCount, float noiseLevel, float regAlfa = 1){
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<float> dist(0,1);

    std::vector<Point> data(pointsCount);
    float xmin = -M_PI;
    float xmax = M_PI;
//    float xmin = -5;
//    float xmax = 1;
    float step = (xmax - xmin) / pointsCount;
    float x = xmin;
    for (int i = 0; i < pointsCount; i++){
        data[i] = Point(x, sin(x) + dist(gen)*noiseLevel);
//        data[i] = Point(x, exp(x) + dist(gen)*noiseLevel);
        x += step;
        std::cout << "data[" << i << "] = (" << data[i].first << ", " << data[i].second << ")" << std::endl;
    }

    int nodesCount = (int)(pointsCount / elementsCount);

    step = (xmax - xmin) / (nodesCount - 1);
    x = xmin;
    std::vector<float> nodes(nodesCount);
    for(int i = 0; i < nodesCount; i++){
        nodes[i] = x;
        x += step;
    }

    for (auto it = nodes.begin(); it != nodes.end(); it++)
        std::cout << "node " << *it << std::endl;



    Spline spline(data, nodes, regAlfa);
    drawSpline(data, spline);
}

void regularizationTest(int pointsCount, int elementsCount, float noiseLevel){
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<float> dist(0,1);

    std::vector<Point> data(pointsCount);
    float xmin = -M_PI;
    float xmax = M_PI;
    float step = (xmax - xmin) / pointsCount;
    float x = xmin;
    for (int i = 0; i < pointsCount; i++){
        data[i] = Point(x, sin(x) + dist(gen)*noiseLevel);
        x += step;
    }
    int nodesCount = (int)(pointsCount / elementsCount);
    step = (xmax - xmin) / (nodesCount - 1);
    x = xmin;
    std::vector<float> nodes(nodesCount);
    for(int i = 0; i < nodesCount; i++){
        nodes[i] = x;
        x += step;
    }

    Spline spline0(data, nodes, 0);
    Spline spline1(data, nodes, 1);
    Spline spline10(data, nodes, 10);
    Spline spline100(data, nodes, 100);

    Gnuplot gp;
    gp << "set grid\n \
           set xrange" << "[" << xmin-1 << ":" << xmax+1 << "]\n \
           set yrange" << "[" << -1.5 << ":" << 1.5 << "]\n";
    gp << "plot '-' with points ps 1 pt 7 title 'Raw data', \
                '-' with lines linecolor 2 title 'alfa = 0', \
                '-' with lines linecolor 3 title 'alfa = 1', \
                '-' with lines linecolor 4 title 'alfa = 10', \
                '-' with lines linecolor 7 title 'alfa = 100'\n";
    gp.send(data);
    gp.send1d(spline0.getSplineData());
    gp.send1d(spline1.getSplineData());
    gp.send1d(spline10.getSplineData());
    gp.send1d(spline100.getSplineData());
}

int main()
{
    try{
        std::ifstream dataFile("../simplified_lab1/data.txt");
        if(!dataFile.good()) throw std::runtime_error("Data file not found");
        std::ifstream splineFile("../simplified_lab1/spline.txt");
        if(!splineFile.good()) throw std::runtime_error("Spline file not found");

        splineFromFile(dataFile, splineFile);

        splineFromFunction(30, 5, 0.2);
//        regularizationTest(30, 5, 0.2);


    } catch(std::runtime_error& e){
        std::cout << "File not found: " << e.what();
        return 1;
    }

    return 0;
}
