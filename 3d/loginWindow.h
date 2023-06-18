//
// Created by dan on 4/4/23.
//

#ifndef INC_3D_LOGINWINDOW_H
#define INC_3D_LOGINWINDOW_H


#include <gtkmm/window.h>
#include <gtkmm/radiobutton.h>
#include <gtkmm/box.h>
#include <gtkmm/separator.h>
#include <gtkmm/entry.h>
#include <soci/soci.h>

class LoginWindow : public Gtk::Window
{
public:
    LoginWindow(soci::session & sql, int * id);
    virtual ~LoginWindow();
protected:
//Signal handlers:
    virtual void on_button_clicked();
//Child widgets:
    Gtk::VBox vBoxMain;
    Gtk::Label labelLogin;
    Gtk::Entry entryLogin;
    Gtk::Label labelPassword;
    Gtk::Entry entryPassword;
    Gtk::Button buttonDisplay;
    soci::session & sql_;
    int * id_;
};


#endif //INC_3D_LOGINWINDOW_H
