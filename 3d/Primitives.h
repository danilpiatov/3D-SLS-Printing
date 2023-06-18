//
// Created by dan on 2/14/23.
//

#ifndef INC_3D_PRIMITIVES_H
#define INC_3D_PRIMITIVES_H

struct Point{
    double x;
    double z;
};

struct Line{
    Point a;
    Point b;
    bool polygonized;
};

struct vertex{
    double x;
    double y;
    double z;
};

struct polygon{
    bool outer = false;
    std::vector<Point> vertices = {};
    int rEdge;
    int lEdge;
    int uEdge;
    int dEdge;
};

#endif //INC_3D_PRIMITIVES_H
