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
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

    const int width = this->width();
    const int height = this->height();
    int xc = width / 2 + viewOffset.x() * coef_;
    int yc = height / 2 + viewOffset.y() * coef_;

    // Draw background
    painter.fillRect(rect(), QColor(255, 255, 255));

    // Draw polygons
    QPen polygonPen(QColor(47, 54, 64));
    polygonPen.setWidthF(1.5);
    painter.setPen(polygonPen);

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

    // Draw laser path
    if(!route_.empty()) {
        bool isLaserOn = false;
        QPointF lastPoint(xc + coef_ * (startPoints_[curLayer_].x - center_.x),
                          yc - coef_ * (startPoints_[curLayer_].z - center_.z));

        for (Point p : route_) {
            QPen pathPen(isLaserOn ? QColor(231, 76, 60) : QColor(41, 128, 185));
            pathPen.setWidthF(isLaserOn ? (realLaserWidth_ ? coef_ * laserWidth_ : 2.0) : 1.0);
            pathPen.setCapStyle(Qt::RoundCap);
            painter.setPen(pathPen);

            QPointF currentPoint(xc + coef_ * (p.x - center_.x),
                                 yc - coef_ * (p.z - center_.z));
            painter.drawLine(lastPoint, currentPoint);

            isLaserOn = !isLaserOn;
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

void MyArea::wheelEvent(QWheelEvent *event)
{
    double zoomFactor = 1.1;
    if(event->angleDelta().y() < 0)
        zoomFactor = 1.0 / zoomFactor;

    coef_ *= zoomFactor;
    update();
}

void MyArea::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton) {
        lastDragPos = event->pos();
        isDragging = true;
        setCursor(Qt::ClosedHandCursor);
    }
}

void MyArea::mouseMoveEvent(QMouseEvent *event)
{
    if(isDragging) {
        QPoint delta = event->pos() - lastDragPos;
        viewOffset += QPointF(delta.x()/coef_, delta.y()/coef_);
        lastDragPos = event->pos();
        update();
    }
}

void MyArea::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton) {
        isDragging = false;
        setCursor(Qt::ArrowCursor);
    }
}