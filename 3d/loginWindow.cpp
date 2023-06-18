//
// Created by dan on 4/4/23.
//

#include "loginWindow.h"
#include <iostream>
#include <gtkmm.h>

using namespace soci;
using namespace std;

LoginWindow::LoginWindow(soci::session & sql, int * id) :
        vBoxMain(false, 10),
        labelLogin("Login"),
        entryLogin(),
        labelPassword("Password"),
        entryPassword(),
        buttonDisplay("Enter"),
        sql_(sql),
        id_(id)
{
// Set title and border of the window
    set_title("authorization");
    set_border_width(0);
    add(vBoxMain);
    entryPassword.set_visibility(false);
    vBoxMain.add(labelLogin);
    vBoxMain.add(entryLogin);
    vBoxMain.add(labelPassword);
    vBoxMain.add(entryPassword);
    vBoxMain.pack_start(buttonDisplay);

// Make the button the default widget
    buttonDisplay.set_can_default();
    buttonDisplay.grab_default();
// Connect the clicked signal of the button to
// LoginWindow::on_button_start_clicked()
    buttonDisplay.signal_clicked().connect(sigc::mem_fun(*this,
                                                          &LoginWindow::on_button_clicked) );
// Show all children of the window
    show_all_children();
}

LoginWindow::~LoginWindow()
{
}

void LoginWindow::on_button_clicked() {
    std::string login = entryLogin.get_text();
    std::string password = entryPassword.get_text();
    soci::rowset<soci::row> rs = (sql_.prepare << "SELECT * FROM users WHERE login = :login"
                                                  " and pwd = SHA2(:pwd, 256)",
            soci::use(login, "login"), soci::use(password, "pwd"));

    bool found = false;
    for (auto &r: rs) {
        found = true;
        *id_ = r.get<int>(0);
        break;
    }
    if (found) {
        Gtk::MessageDialog dialog("Successfully logged in");
        dialog.run();
        hide();
    }
    else {
        Gtk::MessageDialog dialog("Wrong login or password");
        dialog.run();
    }
}