#include "myarea.h"
#include <QPainter>
#include <QPainterPath>

MyArea::MyArea(QWidget *parent) : QWidget(parent)
{
}

void MyArea::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    const int width = this->width();
    const int height = this->height();

    // координаты центра окна
    int xc = width / 2;
    int yc = height / 2;

    painter.setPen(QPen(QColor(0, 255, 0), 1.0));

    for (polygon polygon : polygons_.getPolygons()) {
        QPainterPath path;
        path.moveTo(xc + coef_ * (polygon.vertices.front().x - center_.x),
                    yc - coef_ * (polygon.vertices.front().z - center_.z));
        for (auto vertex : polygon.vertices) {
            path.lineTo(xc + coef_ * (vertex.x - center_.x),
                        yc - coef_ * (vertex.z - center_.z));
        }
        painter.drawPath(path);
    }

    double lineWidth = realLaserWidth_ ? coef_ * laserWidth_ : 1;
    QPen pen;
    pen.setCapStyle(Qt::RoundCap);

    if(polygons_.getPolygons().size() > 0) {
        bool line = false;
        QPointF lastPoint(xc + coef_ * (startPoints_[curLayer_].x - center_.x),
                          yc - coef_ * (startPoints_[curLayer_].z - center_.z));

        for (Point p : route_) {
            if (line) {
                pen.setWidthF(lineWidth);
                pen.setColor(Qt::black);
            } else {
                pen.setWidthF(1);
                pen.setColor(Qt::red);
            }
            painter.setPen(pen);

            QPointF currentPoint(xc + coef_ * (p.x - center_.x),
                                 yc - coef_ * (p.z - center_.z));
            painter.drawLine(lastPoint, currentPoint);

            line = !line;
            lastPoint = currentPoint;
        }
    }
}

void MyArea::create(Polygons &polygons, Point &startPoint, double laserWidth, double width, double height, Point center) {
    laserWidth_ = laserWidth;
    startPoints_.clear();
    startPoints_.push_back(startPoint);
    width_ = width;
    height_ = height;
    curLayer_ = 0;
    center_ = center;
    if(!polygons.getPolygons().empty()) {
        polygons_ = polygons;
        coef_ = std::min((width - 10) / width_, (height - 10) / height_);
        route_ = polygons_.findRoute(laserWidth_, startPoints_[0]);
    }
}

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