//
// Created by dan on 3/6/23.
//

#ifndef INC_3D_POLYGONS_H
#define INC_3D_POLYGONS_H

#include <vector>
#include <list>
#include <iostream>
#include "Primitives.h"


class Polygons {
public:
    Polygons();
    void polygonizeAll(std::vector<Line> & lines);
    std::vector<polygon> getPolygons();
    double getWidth();
    double getHeight();
    Point getCenter();
    std::vector<Point> findRoute(double width, Point & startPoint);
private:
    std::list<Point> findLines(double width);
    std::list<Point>::iterator findClosest(std::list<Point>& list, Point point, bool & left);
    double xIntersect(double z, Point a, Point b);
    bool allLinesPolygonized(std::vector<Line> & lines);
    int findUpperUnpolygonizedLine(std::vector<Line> & lines);
    void polygonize(std::vector<Line> & lines, int startLineIndex);
    bool isEq(double x, double y);
    std::vector<polygon> polygons_;
    void isOuter(polygon & polygon);
};


#endif //INC_3D_POLYGONS_H
