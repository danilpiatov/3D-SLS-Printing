//
// Created by dan on 2/14/23.
//

#ifndef INC_3D_MODELSLICER_H
#define INC_3D_MODELSLICER_H
#include <vector>
#include "Primitives.h"
#include "stlParser.h"


class ModelSlicer {
public:
    ModelSlicer();
    void parse(const char *filename);
    std::vector<Line> slice(double z);
    double findLowestPoint();
    double findHighestPoint();
    double findLeftPoint();
    double findRightPoint();
    double findClosePoint();
    double findFarPoint();
private:
    stl::stlFile model_;
};


#endif //INC_3D_MODELSLICER_H
