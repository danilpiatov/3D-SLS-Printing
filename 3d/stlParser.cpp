//
// Created by Jacob Austin on 5/26/18.
//

#include "stlParser.h"
#include <cassert>

namespace stl {
    vertex parseVec(std::ifstream & file) {
        char triangle[12];
        file.read(triangle, 12);
        return vertex({*(float *) (triangle), *(float *) (triangle + 4), *(float *) (triangle + 8)});
    }

    stlFile parseSTL(std::string path) {
        std::ifstream file(path.c_str(), std::ios::in | std::ios::binary);

        if (!file) {
            std::cerr << "ERROR: COULD NOT READ FILE." << std::endl;
            assert(0);
        } else {
            std::cout << "File found. Parsing STL file." << std::endl;
        }

        char header[80];
        char num_triangles[4];
        file.read(header, 80);
        file.read(num_triangles, 4);
        unsigned int num = * (unsigned int *) num_triangles;
        stlFile data;
        data.header = std::string(header);
        data.num_triangles = * (unsigned int *) num_triangles;

        for (unsigned int i = 0; i < data.num_triangles; i++) {
            vertex normal = parseVec(file);
            vertex v1 = parseVec(file);
            vertex v2 = parseVec(file);
            vertex v3 = parseVec(file);
            data.triangles.emplace_back(v1, v2, v3);

            char properties[2];
            file.read(properties, 2);
        }

        std::cout << "Found " << data.num_triangles << " triangles. Parsing complete!" << std::endl;

//        assert((file.peek(), file.eof()));

        return data;
    }
}