#include "myarea.h"
#include <cairomm/context.h>

MyArea::~MyArea()
{
}
bool MyArea::on_draw(const Cairo::RefPtr<Cairo::Context>& cr) {
    Gtk::Allocation allocation = get_allocation();
    const int width = allocation.get_width();
    const int height = allocation.get_height();

    // координаты центра окна
    int xc, yc;
    xc = width / 2;
    yc = height / 2;

    cr->set_line_width(1.0);
   // cr->move_to(xc, yc);
   // cr->line_to(xc + startPoint_.x, yc - startPoint_.z);

    double r = 0, g = 1, b = 0;
    cr->set_source_rgb(r, g, b);
    int aa = 0;
  //  for(auto l: lines){
  //      cr->move_to(xc + coef*l.a.x, yc - coef * l.a.z);
   //     cr->line_to(xc + coef*l.b.x, yc - coef * l.b.z);
    //}
   for (polygon polygon: polygons_.getPolygons()) {
        cr->move_to(xc + coef_ * (polygon.vertices.front().x - center_.x), yc - coef_ * (polygon.vertices.front().z - center_.z));
        for (auto vertex: polygon.vertices) {
            //std::cout << vertex.x << vertex.z << "\n";
            cr->line_to(xc + coef_ * (vertex.x - center_.x), yc - coef_ * (vertex.z - center_.z));
        }
        cr->stroke();
    }

    cr->set_source_rgb(0, 0, 0);
    double lineWidth = realLaserWidth_ ? coef_ * laserWidth_ : 1;
    cr->set_line_width(lineWidth);
    cr->set_line_cap(Cairo::LINE_CAP_ROUND);
    std::cout << "start";
    if(polygons_.getPolygons().size() > 0) {
        double up = polygons_.getPolygons()[0].vertices[0].z;
        bool line = false;
        cr->move_to(xc + coef_ * (startPoints_[curLayer_].x - center_.x), yc - coef_ * (startPoints_[curLayer_].z - center_.z));
        for (Point p: route_) {
            if (line) {
                cr->set_line_width(lineWidth);
                cr->set_source_rgb(0, 0, 0);
            } else {
                cr->set_line_width(1);
                cr->set_source_rgb(1, 0, 0);
            }
            cr->line_to(xc + coef_ * (p.x - center_.x), yc - coef_ * (p.z - center_.z));
            line = !line;
            cr->stroke();
            cr->move_to(xc + coef_ * (p.x - center_.x), yc - coef_ * (p.z - center_.z));
        }
    }
//    Point p = polygons_.intersect(++polygons_.getPolygons()[0].vertices.begin(), --(--polygons_.getPolygons()[0].vertices.end()),
    //                                polygons_.getPolygons()[0].vertices);
    //  std::cout << "\n" <<p.x << " " << p.z;

    cr->stroke();
    return true;
}

void MyArea::create(Polygons &polygons, Point & startPoint, double laserWidth, double width, double height, Point center) {
    laserWidth_ = laserWidth;
    startPoints_.clear();
    startPoints_.push_back(startPoint);
    width_ = width;
    height_ = height;
    curLayer_ = 0;
    center_ = center;
    if(!polygons.getPolygons().empty()) {
        polygons_ = polygons;
        Gtk::Allocation allocation = get_allocation();
        const int width = allocation.get_width();
        const int height = allocation.get_height();
        coef_ = std::min((width - 10) / width_, (height - 10) / height_);
        route_ = polygons_.findRoute(laserWidth_, startPoints_[0]);
    }
}

MyArea::MyArea()
{}

void MyArea::change(Polygons &polygons) {
    if(!polygons.getPolygons().empty()) {
        polygons_ = polygons;
        if (!route_.empty()) {
            startPoints_.push_back(Point{route_.back().x, route_.back().z});
            curLayer_++;
        }
        route_ = polygons_.findRoute(laserWidth_, startPoints_[curLayer_]);
    }
}

void MyArea::prev(Polygons &polygons) {
    if(!polygons.getPolygons().empty()) {
        polygons_ = polygons;
        if (!route_.empty()) {
            startPoints_.pop_back();
            curLayer_--;
        }
        route_ = polygons_.findRoute(laserWidth_, startPoints_[curLayer_]);
    }
}

void MyArea::coefPlus() {
    coef_ *= 1.1;
}

void MyArea::coefMinus() {
    coef_ *= 0.9;
}

void MyArea::setRealLW() {
    realLaserWidth_ = !realLaserWidth_;
}

