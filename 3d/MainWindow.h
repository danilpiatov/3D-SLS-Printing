//
// Created by dan on 4/4/23.
//

#ifndef INC_3D_MAINWINDOW_H
#define INC_3D_MAINWINDOW_H

#include <gtkmm/window.h>
#include <gtkmm/radiobutton.h>
#include <gtkmm/box.h>
#include <gtkmm/separator.h>
#include <gtkmm/entry.h>
#include <soci/soci.h>
#include <unordered_set>

class MainWindow : public Gtk::Window {
public:
    MainWindow(soci::session & sql, int id);
    virtual ~MainWindow();
protected:
//Signal handlers:
    virtual void on_button_reg_clicked();
    virtual void on_button_change_clicked();
//Child widgets:
    Gtk::Label labelHello;
    Gtk::VBox vBoxMain;
    Gtk::Label labelRegister;
    Gtk::Label labelLogin;
    Gtk::Entry entryLogin;
    Gtk::Label labelPwd1;
    Gtk::Entry entryPwd1;
    Gtk::Label labelPwd2;
    Gtk::Entry entryPwd2;
    Gtk::Button buttonReg;
    Gtk::Label labelChangePwd;
    Gtk::Label labelCurPwd;
    Gtk::Entry entryCurPwd;
    Gtk::Label labelNewPwd1;
    Gtk::Entry entryNewPwd1;
    Gtk::Label labelNewPwd2;
    Gtk::Entry entryNewPwd2;
    Gtk::Button buttonChange;
    std::unordered_set<std::string> commonPwds;
    soci::session & sql_;
    int id_;
};


#endif //INC_3D_MAINWINDOW_H
