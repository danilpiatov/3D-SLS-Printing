//
// Created by dan on 2/14/23.
//

#include "Obj3DModel.h"
#include <string>
#include <iostream>
#include <fstream>
#include <cmath>
#include <algorithm>


Point intersection(vertex a, vertex b, double z){
    double xLen = std::abs(a.x-b.x);
    double yLen = std::abs(a.y-b.y);
    double zLen = std::abs(a.z - b.z);
    double aCoef = (std::abs(a.z-z)/zLen);
    double x = a.x < b.x ? a.x + aCoef * xLen : a.x - aCoef * xLen;
    double y = a.y < b.y ? a.y + aCoef * yLen : a.y - aCoef * yLen;
    return Point({x ,y});
}

Obj3DModel::Obj3DModel()
{
}

std::vector<Line> Obj3DModel::slice(double z) {
    std::vector<Line> lines;
    double max = 0;
    for (stl::Triangle t: model_.triangles){
        std::vector<double> zs {t.v1.z,
                                t.v2.z,
                                t.v3.z };
        if (*std::min_element(zs.begin(), zs.end()) < z && *std::max_element(zs.begin(), zs.end()) > z) {
            if (t.v1.z < z ^ t.v2.z < z ){
                if (t.v1.z < z ^ t.v3.z < z ) {
                    lines.push_back(Line({intersection(t.v1, t.v2, z),
                                          intersection(t.v1, t.v3, z),
                                          false }));
                }
                else{
                    lines.push_back(Line({intersection(t.v1, t.v2, z),
                                          intersection(t.v2, t.v3, z),
                                          false}));
                }
            }
            else{
                lines.push_back(Line({intersection(t.v1, t.v3, z),
                                      intersection(t.v2, t.v3, z),
                                      false}));
            }
            if (std::abs(lines.back().a.x-lines.back().b.x)> 3){
                max = std::abs(lines.back().a.x-lines.back().b.x);
            }
        }
    }
    return lines;
}

double Obj3DModel::findLowestPoint() {
    double z = model_.triangles[0].v1.y;
    for (stl::Triangle t: model_.triangles) {
        if (t.v1.z < z){
            z = t.v1.z;
        }
        if (t.v2.z < z){
            z = t.v2.z;
        }
        if (t.v3.z < z){
            z = t.v3.z;
        }
    }
    return z;
}

double Obj3DModel::findHighestPoint() {
    double z = model_.triangles[0].v1.y;
    for (stl::Triangle t: model_.triangles) {
        if (t.v1.z > z){
            z = t.v1.z;
        }
        if (t.v2.z > z){
            z = t.v2.z;
        }
        if (t.v3.z > z){
            z = t.v3.z;
        }
    }
    return z;
}

void Obj3DModel::parse(const char *filename) {
    model_ = stl::parseSTL(filename);
}

double Obj3DModel::findLeftPoint() {
    double x = model_.triangles[0].v1.x;
    for (stl::Triangle t: model_.triangles) {
        if (t.v1.x < x){
            x = t.v1.x;
        }
        if (t.v2.x < x){
            x = t.v2.x;
        }
        if (t.v3.x < x){
            x = t.v3.x;
        }
    }
    return x;
}

double Obj3DModel::findRightPoint() {
    double x = model_.triangles[0].v1.x;
    for (stl::Triangle t: model_.triangles) {
        if (t.v1.x > x){
            x = t.v1.x;
        }
        if (t.v2.x > x){
            x = t.v2.x;
        }
        if (t.v3.x > x){
            x = t.v3.x;
        }
    }
    return x;
}

double Obj3DModel::findClosePoint() {
    double y = model_.triangles[0].v1.y;
    for (stl::Triangle t: model_.triangles) {
        if (t.v1.y > y){
            y = t.v1.y;
        }
        if (t.v2.y > y){
            y = t.v2.y;
        }
        if (t.v3.y > y){
            y = t.v3.y;
        }
    }
    return y;
}

double Obj3DModel::findFarPoint() {
    double y = model_.triangles[0].v1.y;
    for (stl::Triangle t: model_.triangles) {
        if (t.v1.y < y){
            y = t.v1.y;
        }
        if (t.v2.y < y){
            y = t.v2.y;
        }
        if (t.v3.y < y){
            y = t.v3.y;
        }
    }
    return y;
}

