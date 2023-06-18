//
// Created by dan on 5/13/23.
//

#include "SliceWindow.h"

SliceWindow::SliceWindow() :
        hBoxMain(false, 10),
        labelModelPath("Path to model"),
        entryModelPath(),
        labelSliceNum("Slice num"),
        entrySliceNum(),
        labelLaserWidth("Laser width"),
        entryLaserWidth(),
        buttonStart("Start"),
        labelStartPointX("Start point"),
        buttonNext("Next"),
        buttonPrev("Prev"),
        buttonPlus("+"),
        buttonMinus("-"),
        labelScale("Scale")
        {
    set_title("SLS 3D printing app");
    set_border_width(0);
    add(hBoxMain);
    hBoxMain.pack_start(vBoxOptions, Gtk::PACK_SHRINK, 10);
    vBoxOptions.set_halign(Gtk::ALIGN_END);
    hBoxMain.add(vBoxArea);
    vBoxOptions.pack_start(labelModelPath, Gtk::PACK_SHRINK, 10);
    vBoxOptions.pack_start(entryModelPath, Gtk::PACK_SHRINK, 10);
    vBoxOptions.pack_start(*new Gtk::HSeparator(), Gtk::PACK_SHRINK);
    vBoxOptions.pack_start(labelSliceNum, Gtk::PACK_SHRINK, 10);
    vBoxOptions.pack_start(entrySliceNum, Gtk::PACK_SHRINK, 10);
    vBoxOptions.pack_start(*new Gtk::HSeparator(), Gtk::PACK_SHRINK);
    vBoxOptions.pack_start(labelLaserWidth, Gtk::PACK_SHRINK, 10);
    vBoxOptions.pack_start(entryLaserWidth, Gtk::PACK_SHRINK, 10);
    vBoxOptions.pack_start(*new Gtk::HSeparator(), Gtk::PACK_SHRINK);
    vBoxOptions.pack_start(labelStartPointX, Gtk::PACK_SHRINK, 10);
    entryStartPointX.set_width_chars(7);
    entryStartPointY.set_width_chars(7);
    startPointBox.pack_start(entryStartPointX, Gtk::PACK_EXPAND_WIDGET, 10);
    startPointBox.pack_start(entryStartPointY, Gtk::PACK_EXPAND_WIDGET, 10);
    vBoxOptions.pack_start(startPointBox, Gtk::PACK_SHRINK, 10);
    vBoxOptions.pack_start(*new Gtk::HSeparator(), Gtk::PACK_SHRINK);
    vBoxOptions.pack_start(buttonStart, Gtk::PACK_SHRINK, 10);
    vBoxOptions.pack_start(*new Gtk::HSeparator(), Gtk::PACK_SHRINK,2);
    vBoxOptions.pack_start(*new Gtk::HSeparator(), Gtk::PACK_SHRINK,2);
    vBoxOptions.pack_start(*new Gtk::HSeparator(), Gtk::PACK_SHRINK,2);
    prevNextBox.pack_start(buttonPrev, Gtk::PACK_EXPAND_WIDGET, 10);
    prevNextBox.pack_start(buttonNext, Gtk::PACK_EXPAND_WIDGET, 10);
    vBoxOptions.pack_start(prevNextBox, Gtk::PACK_SHRINK, 10);
    vBoxOptions.pack_start(*new Gtk::HSeparator(), Gtk::PACK_SHRINK);
    vBoxOptions.pack_start(labelScale, Gtk::PACK_SHRINK, 10);
    scaleBox.pack_start(buttonMinus, Gtk::PACK_EXPAND_WIDGET, 10);
    scaleBox.pack_start(buttonPlus, Gtk::PACK_EXPAND_WIDGET, 10);
    vBoxOptions.pack_start(scaleBox, Gtk::PACK_SHRINK, 10);
    vBoxOptions.pack_start(*new Gtk::HSeparator(), Gtk::PACK_SHRINK);
    laserSizeButton.set_label("Show real laser size");
    vBoxOptions.pack_start(laserSizeButton, Gtk::PACK_SHRINK, 10 );
// Make the button the default widget
    buttonStart.set_can_default();
    buttonStart.grab_default();
// Connect the clicked signal of the button to
// LoginWindow::on_button_start_clicked()
    buttonStart.signal_clicked().connect(sigc::mem_fun(*this,
                                                       &SliceWindow::on_button_start_clicked));

    buttonPrev.set_can_default();
    buttonPrev.grab_default();
// Connect the clicked signal of the button to
    buttonPrev.signal_clicked().connect(sigc::mem_fun(*this,
                                                      &SliceWindow::on_button_prev_clicked));

    buttonNext.set_can_default();
    buttonNext.grab_default();
// Connect the clicked signal of the button to
    buttonNext.signal_clicked().connect(sigc::mem_fun(*this,
                                                      &SliceWindow::on_button_next_clicked));

    buttonPlus.set_can_default();
    buttonPlus.grab_default();
// Connect the clicked signal of the button to
    buttonPlus.signal_clicked().connect(sigc::mem_fun(*this,
                                                      &SliceWindow::on_button_plus_clicked));

    buttonMinus.set_can_default();
    buttonMinus.grab_default();
// Connect the clicked signal of the button to
    buttonMinus.signal_clicked().connect(sigc::mem_fun(*this,
                                                      &SliceWindow::on_button_minus_clicked));

    laserSizeButton.set_can_default();
    laserSizeButton.grab_default();
    laserSizeButton.signal_clicked().connect(sigc::mem_fun(*this,
                                                           &SliceWindow::on_button_laser_width_clicked));

    areaEventBox.add(area);
    vBoxArea.pack_start(areaEventBox, Gtk::PACK_EXPAND_WIDGET);

    add_events(Gdk::KEY_PRESS_MASK);
// Show all children of the window
    show_all_children();
}

void SliceWindow::on_button_start_clicked() {
    polygons_.clear();

    curNum_ = 0;
    std::string path = entryModelPath.get_text();

    int sliceNum = std::stoi(entrySliceNum.get_text().c_str());
    double laserWidth = std::stod(entryLaserWidth.get_text());
    double startX = std::stod(entryStartPointX.get_text());
    double startY = std::stod(entryStartPointY.get_text());
    model_.parse(path.c_str());
    h_ = model_.findHighestPoint();
    l_ = model_.findLowestPoint();
    step_ = (h_-l_)/(sliceNum + 1);
    maxNum_ = sliceNum - 1;
    for (int i = 0; i < sliceNum; ++i){
        Polygons p;
        std::vector<Line> lines = model_.slice(l_ + step_ * (i + 1));
        p.polygonizeAll(lines);
        polygons_.push_back(p);
    }
    while(maxNum_ >= 0 && polygons_[maxNum_].getPolygons().empty()) {
        maxNum_--;
    }
    while(curNum_ < maxNum_ &&polygons_[curNum_].getPolygons().empty()) {
        curNum_++;
    }
    minNum_ = curNum_;
    if (maxNum_ > -1) {
        Point startPoint = Point{startX, startY};
        int tmp = curNum_;
        double width = model_.findRightPoint() - model_.findLeftPoint();
        double height = model_.findClosePoint() - model_.findFarPoint();
        double x = model_.findLeftPoint() + width/2;
        double y = model_.findFarPoint() + height/2;

        area.create(polygons_[curNum_], startPoint, laserWidth, width, height, {x,y});
    }
    queue_draw();
    show_all_children();
}

SliceWindow::~SliceWindow() {
}

void SliceWindow::on_button_prev_clicked() {
    if(curNum_ > minNum_) {
        curNum_--;
        area.prev(polygons_[curNum_]);
        queue_draw();
        show_all_children();
    }
}

void SliceWindow::on_button_next_clicked() {
    if(curNum_ < maxNum_) {
        curNum_++;
        area.change(polygons_[curNum_]);
        queue_draw();
        show_all_children();
    }
}

void SliceWindow::on_button_plus_clicked() {
    area.coefPlus();
    queue_draw();
    show_all_children();

}

void SliceWindow::on_button_minus_clicked() {
    area.coefMinus();
    queue_draw();
    show_all_children();

}

bool SliceWindow::on_key_press_event(GdkEventKey* key_event)
{
    //GDK_MOD1_MASK -> the 'alt' key(mask)
    //GDK_KEY_1 -> the '1' key
    //GDK_KEY_2 -> the '2' key

    //select the first radio button, when we press alt + 1
    if(key_event->keyval == GDK_KEY_Left)
    {
        area.moveLeft();
        //returning true, cancels the propagation of the event
      //  return false;
    }
    else if(key_event->keyval == GDK_KEY_Right)
    {
        area.moveRight();
        //returning true, cancels the propagation of the event
        //return true;
    }
    else if(key_event->keyval == GDK_KEY_Up )
    {
        area.moveUp();
        //returning true, cancels the propagation of the event
        //return true;
    }
    else if(key_event->keyval == GDK_KEY_Down )
    {
        area.moveDown();
        //returning true, cancels the propagation of the event
       // return true;
    }
    queue_draw();
    show_all_children();
    //if the event has not been handled, call the base class
    return Gtk::Window::on_key_press_event(key_event);
}

void SliceWindow::on_button_laser_width_clicked() {
    area.setRealLW();
    queue_draw();
    show_all_children();
}

