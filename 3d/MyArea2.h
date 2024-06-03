//
// Created by dan on 5/13/23.
//

#ifndef INC_3D_MYAREA2_H
#define INC_3D_MYAREA2_H


#include <gtkmm/drawingarea.h>

class MyArea2 : public Gtk::DrawingArea
{
public:

protected:
    //Override default signal handler:
    bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;
};


#endif //INC_3D_MYAREA2_H
