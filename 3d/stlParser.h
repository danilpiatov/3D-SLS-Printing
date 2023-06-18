//
// Created by Jacob Austin on 5/26/18.
//

#ifndef STL_PARSER_H
#define STL_PARSER_H

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include "Primitives.h"

namespace stl {
    struct Triangle {
        Triangle(const vertex &vertex1, const vertex &vertex2, const vertex &vertex3) : v1(vertex1),
                                                                                        v2(vertex2),
                                                                                        v3(vertex3) {};
        vertex v1;
        vertex v2;
        vertex v3;
    };

    struct stlFile {
        std::string header;
        int num_triangles;
        std::vector<Triangle> triangles;
    };

    stlFile parseSTL(std::string path);

    vertex parseVec(std::ifstream & file);
}

#endif //STL_PARSER_H
