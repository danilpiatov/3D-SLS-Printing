//
// Created by dan on 3/6/23.
//

#include <cmath>
#include <algorithm>
#include "Polygons.h"


Polygons::Polygons():
polygons_({})
{}

void Polygons::polygonize(std::vector<Line> &lines, int startLineIndex) {
    polygon p;
    int index = startLineIndex;
    int maxRight=0;
    int maxLeft=0;
    int maxUp=0;
    int maxDown=0;
    while(true){
        lines[index].polygonized = true;
        if(p.vertices.size() > 0){
            if(isEq(lines[index].a.x, p.vertices.back().x) && isEq(lines[index].a.z, p.vertices.back().z)) {
                p.vertices.push_back(lines[index].b);
            }
            else{
                p.vertices.push_back(lines[index].a);
            }
        }
        else{
            p.vertices.push_back(lines[index].a);
            maxUp = p.vertices.size()-1;
            maxLeft = maxUp;
            maxRight = maxUp;
            maxDown = maxUp;
        }
        size_t i = 0;
        while (i < lines.size()){
            if(!lines[i].polygonized && 
             ( (isEq(lines[i].a.x, p.vertices.back().x) && isEq(lines[i].a.z, p.vertices.back().z)) ||
                    (isEq(lines[i].b.x, p.vertices.back().x) && isEq(lines[i].b.z, p.vertices.back().z)))){
                index = i;
                break;
            }
            i++;
        }

        if(p.vertices.back().x > p.vertices[maxRight].x){
            maxRight = p.vertices.size() - 1;
        }
        if(p.vertices.back().x < p.vertices[maxLeft].x){
            maxLeft = p.vertices.size() - 1;
        }
        if(p.vertices.back().z > p.vertices[maxUp].z){
            maxUp = p.vertices.size() - 1;
        }
        if(p.vertices.back().z < p.vertices[maxDown].z){
            maxDown = p.vertices.size() - 1;
        }

        if (i == lines.size()){
            p.vertices.push_back(p.vertices.front());
            break;
        }
    }
    p.uEdge = 0;
    p.rEdge = maxRight;
    p.lEdge = maxLeft;
    p.dEdge = maxDown;

    if(p.vertices.size() > 2)
        polygons_.emplace_back(p);
}

bool Polygons::allLinesPolygonized(std::vector<Line> &lines) {
    for (Line line: lines){
        if(!line.polygonized){
            return false;
        }
    }
    return true;
}

int Polygons::findUpperUnpolygonizedLine(std::vector<Line> &lines) {
    int maxZIndex = 0;
    size_t i;
    for(i = 0; i < lines.size(); i++) {
        if (!lines[i].polygonized) {
            maxZIndex = i;
            break;
        }
    }
    for(size_t j = i; j < lines.size(); j++){
        if(!lines[j].polygonized && (lines[j].a.z > lines[maxZIndex].a.z)){
            maxZIndex = j;
        }
    }
    return maxZIndex;
}

std::vector<polygon> Polygons::getPolygons() {
    return polygons_;
}

bool Polygons::isEq(double x, double y) {
    return std::abs(x-y) < 0.0000001 ;
}

void Polygons::isOuter(polygon & polygon) {
    for (int i = polygons_.size() - 1; i >= 0; --i) {
        bool out = true;
        if(isEq(polygon.vertices[0].x, polygons_[i].vertices[0].x) && isEq(polygon.vertices[0].z, polygons_[i].vertices[0].z)){
            continue;
        }

        for (size_t j = 0; j < polygons_[i].vertices.size() - 1; ++j){
            if (polygon.vertices[polygon.uEdge].x < std::max(polygons_[i].vertices[j].x, polygons_[i].vertices[j + 1].x) &&
                polygon.vertices[polygon.uEdge].z > std::min(polygons_[i].vertices[j].z, polygons_[i].vertices[j + 1].z) &&
                polygon.vertices[polygon.uEdge].z < std::max(polygons_[i].vertices[j].z, polygons_[i].vertices[j + 1].z) &&
                xIntersect(polygon.vertices[polygon.uEdge].x,polygons_[i].vertices[j], polygons_[i].vertices[j  + 1] )
                > polygon.vertices[polygon.uEdge].x){
                out = !out;
            }
        }
        if (!out){
            if(polygons_[i].outer) {
                polygon.outer = false;
                return;
            }
            else{
                polygon.outer = true;
                return;
            }
        }
    }
    polygon.outer = true;
    return;
}

std::list<Point> Polygons::findLines(double width) {
    double up = polygons_[0].vertices[polygons_[0].uEdge].z;
    double down = polygons_[0].vertices[polygons_[0].dEdge].z;
    std::vector<std::vector<double>> lefts;
    std::vector<std::vector<double>> rights;
    for (size_t i = 1; i < polygons_.size(); ++i){
        if(polygons_[i].vertices[polygons_[i].uEdge].z > up){
            up = polygons_[i].vertices[polygons_[i].uEdge].z;
        }
        if(polygons_[i].vertices[polygons_[i].dEdge].z < down){
            down = polygons_[i].vertices[polygons_[i].dEdge].z;
        }
    }
    for (polygon p: polygons_) {
        bool goingDown = true;
        bool clockwise = p.vertices[1].x > p.vertices[p.vertices.size() - 2].x;
        size_t level = 0;
        while (up - width * (level + 1) + width/2 > p.vertices[0].z) {
            ++level;
        }
        size_t i = 0;
        while (i < p.vertices.size() - 1) {
            if (goingDown ^ (p.vertices[i].z > p.vertices[i + 1].z)) {
                if (goingDown) {
                    level--;
                } else {
                    level++;
                }

                goingDown = !goingDown;
            }
            double x = (p.outer ^ (goingDown ^ clockwise)) ? -1 * width / 2 : width / 2;
            while (lefts.size() < level + 1){
                lefts.emplace_back(std::vector<double>());
                rights.emplace_back(std::vector<double>());
            }
            if (goingDown) {
                if (p.vertices[i + 1].z >= up - width * (level + 1) + width / 2) {
                    ++i;
                    continue;
                } else {
                    if(x>0) {
                        lefts[level].emplace_back(
                                xIntersect(up - width * (level + 1) + width / 2, p.vertices[i], p.vertices[i + 1]) + x);
                    }
                    else{
                        rights[level].emplace_back(
                                xIntersect(up - width * (level + 1) + width / 2, p.vertices[i], p.vertices[i + 1]) + x);
                    }
                }
                level++;
            } else {
                if (p.vertices[i + 1].z <= up - width * (level + 1) + width / 2) {
                    ++i;
                    continue;
                } else {
                    if(x>0) {
                        lefts[level].emplace_back(
                                xIntersect(up - width * (level + 1) + width / 2, p.vertices[i], p.vertices[i + 1]) + x);
                    }
                    else{
                        rights[level].emplace_back(
                                xIntersect(up - width * (level + 1) + width / 2, p.vertices[i], p.vertices[i + 1]) + x);
                    }
                }
                level--;
            }
        }
    }
    std::list<Point> points;
    for (size_t i = 0; i < lefts.size(); ++i){
        std::sort(lefts[i].begin(), lefts[i].end());
        std::sort(rights[i].begin(), rights[i].end());
        for (size_t j = 0; j < lefts[i].size(); ++j){
            if (lefts[i][j] < rights[i][j]) {
                points.emplace_back(Point{lefts[i][j], up - width * (i + 1) + width / 2});
                points.emplace_back(Point{rights[i][j], up - width * (i + 1) + width / 2});
            }
        }
    }
    return points;
}

double Polygons::xIntersect(double z,  Point a, Point b) {
    Point min;
    if (a.x < b.x){
        min = a;
    }
    else{
        min = b;
    }
    return min.x + std::abs(a.x-b.x)*
    (std::abs(z-min.z)/std::abs(a.z - b.z));
}

std::vector<Point> Polygons::findRoute(double width, Point & startPoint) {
    std::list<Point> lines = findLines(width);
    std::vector<Point> points;
    Point cur = {startPoint.x, startPoint.z};
    while (!lines.empty()){
        bool left;
        auto it = findClosest(lines, cur, left);
        auto it2 = it;
        if(left){
            ++it2;
            points.emplace_back(*it);
            points.emplace_back(*it2);
            cur = *it2;
            lines.erase(it, ++it2);
        }
        else{
            --it2;
            points.emplace_back(*it);
            points.emplace_back(*it2);
            cur = *it2;
            lines.erase(it2, ++it);
        }
    }
    return points;
}

std::list<Point>::iterator Polygons::findClosest(std::list<Point>& list, Point point, bool & left) {
    auto it = list.begin();
    auto closestIt = it;
    bool curLeft = true;
    left = true;
    double minDistance = std::sqrt(std::pow((point.x - it->x), 2) + std::pow((point.z - it->z), 2));
    ++it;
    while (it != list.end()){
        curLeft = !curLeft;
        if (minDistance > std::sqrt(std::pow((point.x - it->x), 2) + std::pow((point.z - it->z), 2))){
            minDistance = std::sqrt(std::pow((point.x - it->x), 2) + std::pow((point.z - it->z), 2));
            left = curLeft;
            closestIt = it;
        }
        ++it;
    }
    return closestIt;
}

void Polygons::polygonizeAll(std::vector<Line> &lines) {
    if (lines.empty()){
        return;
    }
    while (!allLinesPolygonized(lines)){
        polygonize(lines, findUpperUnpolygonizedLine(lines));
    }

    for (polygon &p : polygons_) {
        isOuter(p);
    }
}

double Polygons::getWidth() {
    if(polygons_.empty()) {
        return 1;
    }
    double left = polygons_[0].vertices[polygons_[0].lEdge].x;
    double right = polygons_[0].vertices[polygons_[0].rEdge].x;
    for(size_t i = 1; i < polygons_.size();++i){
        if(polygons_[i].vertices[polygons_[i].lEdge].x < left){
            left = polygons_[i].vertices[polygons_[i].lEdge].x;
        }
        if(polygons_[i].vertices[polygons_[i].rEdge].x > right){
            right = polygons_[i].vertices[polygons_[i].rEdge].x;
        }
    }
    return right - left;
}

double Polygons::getHeight() {
    if(polygons_.empty()) {
        return 1;
    }
    double up = polygons_[0].vertices[polygons_[0].uEdge].z;
    double down = polygons_[0].vertices[polygons_[0].dEdge].z;
    for(size_t i = 1; i < polygons_.size();++i){
        if(polygons_[i].vertices[polygons_[i].dEdge].z < down){
            down = polygons_[i].vertices[polygons_[i].dEdge].z;
        }
        if(polygons_[i].vertices[polygons_[i].uEdge].z > up){
            up = polygons_[i].vertices[polygons_[i].uEdge].z;
        }
    }
    return up - down;
}

Point Polygons::getCenter() {
    if(polygons_.empty()) {
        return {0,0};
    }
    double left = polygons_[0].vertices[polygons_[0].lEdge].x;
    double down = polygons_[0].vertices[polygons_[0].dEdge].z;
    for(size_t i = 1; i < polygons_.size();++i){
        if(polygons_[i].vertices[polygons_[i].dEdge].z < down){
            down = polygons_[i].vertices[polygons_[i].dEdge].z;
        }
        if(polygons_[i].vertices[polygons_[i].lEdge].x < left){
            left = polygons_[i].vertices[polygons_[i].lEdge].x;
        }
    }
    Point point = {left + this->getWidth()/2, down + this->getHeight()/2};
    return point;
}

