//
// Created by dan on 5/13/23.
//

#ifndef INC_3D_SLICEWINDOW_H
#define INC_3D_SLICEWINDOW_H

#include <gtkmm/window.h>
#include <gtkmm/radiobutton.h>
#include <gtkmm/box.h>
#include <gtkmm/separator.h>
#include <gtkmm/entry.h>
#include <gtkmm/grid.h>
#include <gtkmm/eventbox.h>
#include "myarea.h"
#include "MyArea2.h"

class SliceWindow : public Gtk::Window
{
public:
    SliceWindow();  
    virtual ~SliceWindow();
protected:
//Signal handlers:
    virtual void on_button_start_clicked();
    virtual void on_button_prev_clicked();
    virtual void on_button_next_clicked();
    virtual void on_button_plus_clicked();
    virtual void on_button_minus_clicked();
    virtual void on_button_laser_width_clicked();
    bool on_key_press_event(GdkEventKey* event) override;
//Child widgets:
    Gtk::HBox hBoxMain;
    Gtk::VBox vBoxArea;
    Gtk::EventBox areaEventBox;
    Gtk::VBox vBoxOptions;
    Gtk::HBox startPointBox;
    Gtk::VBox xBox;
    Gtk::VBox yBox;
    Gtk::HBox prevNextBox;
    Gtk::HBox scaleBox;
    Gtk::Label labelScale;
    Gtk::Label labelModelPath;
    Gtk::Entry entryModelPath;
    Gtk::Label labelSliceNum;
    Gtk::Entry entrySliceNum;
    Gtk::Label labelLaserWidth;
    Gtk::Entry entryLaserWidth;
    Gtk::Label labelStartPointX;
    Gtk::Entry entryStartPointX;
    Gtk::Label labelStartPointY;
    Gtk::Entry entryStartPointY;
    Gtk::Button buttonStart;
    Gtk::Button buttonNext;
    Gtk::Button buttonPrev;
    Gtk::Button buttonPlus;
    Gtk::Button buttonMinus;
    Gtk::CheckButton laserSizeButton;
    MyArea area;
private:
    int curNum_ = 0;
    int minNum_ = 0;
    int maxNum_ = 0;
    double l_;
    double h_;
    double step_;
    Obj3DModel model_;
    std::vector<Polygons> polygons_;
};


#endif //INC_3D_SLICEWINDOW_H
