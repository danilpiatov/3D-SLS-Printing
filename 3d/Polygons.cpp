//
// Created by dan on 3/6/23.
//

#include <cmath>
#include <algorithm>
#include "Polygons.h"
#include "clipper.hpp"


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

static constexpr double EPS = 1e-9;

/// Преобразования Point <-> Clipper
static ClipperLib::Path toClipperPath(const std::vector<Point>& poly) {
    ClipperLib::Path p; p.reserve(poly.size());
    const double SCALE = 1e9;
    for (auto &pt : poly)
        p.emplace_back((long)std::llround(pt.x * SCALE), (long)std::llround(pt.z * SCALE));
    return p;
}
static std::vector<Point> fromClipperPath(const ClipperLib::Path& path) {
    std::vector<Point> poly; poly.reserve(path.size());
    const double SCALE = 1e9;
    for (auto &ip : path)
        poly.push_back(Point{ ip.X / SCALE, ip.Y / SCALE });
    return poly;
}

// Смещение одного полигона на delta (отрицательное — внутрь для внешнего)
static std::vector<Point> offsetPolygon(const std::vector<Point>& input, double delta) {
    ClipperLib::ClipperOffset co;
    co.AddPath(toClipperPath(input), ClipperLib::jtMiter, ClipperLib::etClosedPolygon);
    ClipperLib::Paths sol;
    const double SCALE = 1e9;
    co.Execute(sol, delta * SCALE);
    if (sol.empty()) return {};
    return fromClipperPath(sol.front());
}

bool isPointInPolygon(const Point &pt, const std::vector<Point> &poly) {
    bool inside = false;
    size_t n = poly.size();
    for (size_t i = 0, j = n - 1; i < n; j = i++) {
        const Point &A = poly[i], &B = poly[j];
        if ((A.z > pt.z) != (B.z > pt.z)) {
            double x = (B.x - A.x) * (pt.z - A.z) / (B.z - A.z + 1e-9) + A.x;
            if (pt.x < x) inside = !inside;
        }
    }
    return inside;
}
bool isPolygonInside(const std::vector<Point> &inner, const std::vector<Point> &outer) {
    int hits = 0;
    for (const auto &p : inner) {
        if (isPointInPolygon(p, outer)) ++hits;
    }
    return hits > inner.size() / 2;
}

std::list<Point> Polygons::findLines(double width) {
    std::list<Point> pts;
    const double EPS = 1e-9;

    // Вложенность по чётности (0 - остров, 1 - дыра, 2 - остров внутри дыры и т.д.)
    std::vector<std::tuple<std::vector<Point>, int>> polygonLevels;

    for (size_t i = 0; i < polygons_.size(); ++i) {
        const auto& A = polygons_[i];
        int level = 0;
        for (size_t j = 0; j < polygons_.size(); ++j) {
            if (i == j) continue;
            const auto& B = polygons_[j];
            int inside = 0;
            for (auto& p : A.vertices)
                if (isPointInPolygon(p, B.vertices)) inside++;
            if (inside > A.vertices.size() / 2)
                level++;
        }
        polygonLevels.emplace_back(A.vertices, level);
    }

    // Обработка только островов
    for (auto &[vertices, level] : polygonLevels) {
        if (level % 2 != 0) continue; // это дыра

        // 1. Применяем PCA только к этому острову
        double sumX = 0, sumZ = 0, totalLen = 0;
        for (size_t i = 0; i + 1 < vertices.size(); ++i) {
            double dx = vertices[i+1].x - vertices[i].x;
            double dz = vertices[i+1].z - vertices[i].z;
            double len = std::hypot(dx, dz);
            if (len < EPS) continue;
            sumX += dx / len;
            sumZ += dz / len;
            totalLen += 1;
        }
        if (totalLen < EPS) continue;
        double theta = std::atan2(sumZ, sumX);
        double cth = std::cos(-theta), sth = std::sin(-theta);

        // 2. Собираем все вложенные дырки (следующий нечётный уровень)
        std::vector<std::vector<Point>> holes;
        for (auto &[v2, lvl2] : polygonLevels) {
            if (lvl2 == level + 1 && isPolygonInside(v2, vertices)) {
                holes.push_back(v2);
            }
        }

        // 3. Преобразование в RV и построение allRings
        auto transform = [&](const std::vector<Point> &in) -> std::vector<Point> {
            std::vector<Point> out;
            for (auto &p : in) {
                double rx = p.x * cth - p.z * sth;
                double rz = p.x * sth + p.z * cth;
                out.push_back({rx, rz});
            }
            return out;
        };

        std::vector<std::vector<Point>> allRings;
        allRings.push_back(transform(vertices));
        for (auto &hole : holes)
            allRings.push_back(transform(hole));

        // 4. Поиск min/max Z
        double minZ = 1e18, maxZ = -1e18;
        for (auto &ring : allRings) {
            for (auto &p : ring) {
                minZ = std::min(minZ, p.z);
                maxZ = std::max(maxZ, p.z);
            }
        }

        size_t levels = static_cast<size_t>(std::ceil((maxZ - minZ) / width));
        for (size_t lvl = 0; lvl < levels; ++lvl) {
            double scanZ = minZ + (lvl + 0.5) * width;
            std::vector<double> Xs;

            for (const auto &ring : allRings) {
                size_t sz = ring.size();
                for (size_t i = 0; i < sz; ++i) {
                    auto &A = ring[i];
                    auto &B = ring[(i + 1) % sz];
                    if (std::abs(B.z - A.z) < EPS) continue;
                    if (scanZ < std::min(A.z, B.z) || scanZ > std::max(A.z, B.z)) continue;
                    double t = (scanZ - A.z) / (B.z - A.z);
                    double x = A.x + t * (B.x - A.x);
                    Xs.push_back(x);
                }
            }

            std::sort(Xs.begin(), Xs.end());
            for (size_t j = 0; j + 1 < Xs.size(); j += 2) {
                double x1 = Xs[j] + width/2;
                double x2 = Xs[j + 1] - width/2;
                if (x2 <= x1) continue;

                Point P1{ x1 * cth + scanZ * sth, -x1 * sth + scanZ * cth };
                Point P2{ x2 * cth + scanZ * sth, -x2 * sth + scanZ * cth };
                pts.push_back(P1);
                pts.push_back(P2);
            }
        }
    }

    return pts;
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

