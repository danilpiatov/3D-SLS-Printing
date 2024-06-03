//
// Created by Jacob Austin on 5/26/18.
//

#include "stlParser.h"
#include <cassert>
#include <tuple>
#include <cstring>

namespace stl {
    vertex parseVec(std::ifstream & file) {
        char triangle[12];
        file.read(triangle, 12);
        vertex v;
        std::memcpy(&v.x, &triangle[0], 4);
        std::memcpy(&v.y, &triangle[4], 4);
        std::memcpy(&v.z, &triangle[8], 4);
        return v;
    }

    stlFile parseSTL(const std::string& path) {
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
        stlFile data;
        data.header = std::string(header);
        std::memcpy(&data.num_triangles, &num_triangles[0], 4);

        for ( int i = 0; i < data.num_triangles; i++) {
            std::ignore = parseVec(file);
            vertex v1 = parseVec(file);
            vertex v2 = parseVec(file);
            vertex v3 = parseVec(file);
            data.triangles.emplace_back(v1, v2, v3);

            char properties[2];
            file.read(properties, 2);
        }

        std::cout << "Found " << data.num_triangles << " triangles. Parsing complete!" << std::endl;

        return data;
    }
}
