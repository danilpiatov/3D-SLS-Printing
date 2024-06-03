//
// Created by dan on 2/14/23.
//

#ifndef INC_3D_MYAREA_H
#define INC_3D_MYAREA_H

#include <vector>
#include <gtkmm/drawingarea.h>
#include "Polygons.h"
#include "Obj3DModel.h"

class MyArea : public Gtk::DrawingArea {

public:
    void create(Polygons & polygons, Point & startPoint, double laserWidth, double width, double height, Point center);
    void change(Polygons & polygons);
    void prev(Polygons & polygons);
    void coefPlus();
    void coefMinus();
    void moveLeft() {
        center_.x--;
    }
    void setRealLW();

    void moveRight(){
        center_.x++;
    }
    void moveUp(){
        center_.z++;
    }
    void moveDown(){
        center_.z--;
    }

protected:

    //Override default signal handler:

    virtual bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr);
private:
    Polygons polygons_;
    double width_ = 0;
    double height_ = 0;
    std::vector<Point> route_ = {};
    int curLayer_ = 0;
    std::vector<Point> startPoints_ = {};
    double laserWidth_ = 0;
    bool realLaserWidth_ = false;
    double coef_ = 0;
    Point center_ = {0,0};
};


#endif //INC_3D_MYAREA_H
