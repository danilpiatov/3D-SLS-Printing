//
// Created by dan on 2/14/23.
//

#ifndef MYAREA_H
#define MYAREA_H

#include <QWidget>
#include <QWheelEvent>
#include <QMouseEvent>
#include <vector>
#include "Polygons.h"
#include "ModelSlicer.h"

class MyArea : public QWidget
{
    Q_OBJECT

public:
    explicit MyArea(QWidget *parent = nullptr);
    void create(Polygons &polygons, Point &startPoint, double laserWidth, double width, double height, Point center);
    void change(Polygons &polygons);
    void prev(Polygons &polygons);
    void coefPlus();
    void coefMinus();
    void moveLeft() { center_.x--; }
    void setRealLW();
    void moveRight() { center_.x++; }
    void moveUp() { center_.z++; }
    void moveDown() { center_.z--; }

protected:
    void paintEvent(QPaintEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    QPoint lastDragPos;
    bool isDragging = false;
    QPointF viewOffset {0, 0};
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

#endif // MYAREA_H