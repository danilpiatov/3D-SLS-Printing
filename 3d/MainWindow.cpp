//
// Created by dan on 4/4/23.
//

#include "MainWindow.h"
#include <iostream>
#include <gtkmm.h>
#include <fstream>

using namespace soci;
using namespace std;

MainWindow::MainWindow(soci::session & sql, int id) :
        vBoxMain(false, 10),
        labelLogin("Login"),
        entryLogin(),
        labelRegister("Register a new user"),
        labelPwd1("Set a password"),
        labelPwd2("Repeat a password"),
        entryPwd1(),
        entryPwd2(),
        buttonReg("Register"),
        labelChangePwd("Change your password"),
        labelCurPwd("Enter current password"),
        entryCurPwd(),
        labelNewPwd1("Set a new password"),
        labelNewPwd2("Repeat a new password"),
        entryNewPwd1(),
        entryNewPwd2(),
        buttonChange("Change"),
        sql_(sql),
        id_(id)
{
    std::ifstream ifs("/home/dan/CLionProjects/3d/rockyou.txt");
    if(ifs){
        std::cout<<"IFS\n";
    }
    else{
        std::cout <<"FFF\n";
    }
    while (!ifs.eof()){
        std::string commonPwd;
        ifs >> commonPwd;
        commonPwds.insert(commonPwd);
    }
    ifs.close();

    soci::rowset<soci::row> rs = (sql_.prepare << "SELECT * FROM users WHERE id = :id",
            soci::use(id_, "id"));
    bool isAdmin;
    std::string name;
    for (auto &r: rs) {
        name = r.get<std::string>(1);
        isAdmin = r.get<int>(3);
        break;
    }
// Set title and border of the window
    set_title("authorization");
    set_border_width(0);
    add(vBoxMain);
    std::string hello = "Hello " + name;
    labelHello.set_text(hello);
    vBoxMain.add(labelHello);
    vBoxMain.add(*new Gtk::HSeparator());
    if (isAdmin) {
        entryPwd1.set_visibility(false);
        entryPwd2.set_visibility(false);
        vBoxMain.add(labelRegister);
        vBoxMain.add(labelLogin);
        vBoxMain.add(entryLogin);
        vBoxMain.add(labelPwd1);
        vBoxMain.add(entryPwd1);
        vBoxMain.add(labelPwd2);
        vBoxMain.add(entryPwd2);
        vBoxMain.add(buttonReg);
        vBoxMain.add(*new Gtk::HSeparator());
        buttonReg.set_can_default();
        buttonReg.grab_default();
        buttonReg.signal_clicked().connect(sigc::mem_fun(*this,
                                                         &MainWindow::on_button_reg_clicked) );

    }
    entryCurPwd.set_visibility(false);
    entryNewPwd1.set_visibility(false);
    entryNewPwd2.set_visibility(false);
    vBoxMain.add(labelChangePwd);
    vBoxMain.add(labelCurPwd);
    vBoxMain.add(entryCurPwd);
    vBoxMain.add(labelNewPwd1);
    vBoxMain.add(entryNewPwd1);
    vBoxMain.add(labelNewPwd2);
    vBoxMain.add(entryNewPwd2);
    vBoxMain.add(buttonChange);
    buttonChange.set_can_default();
    buttonChange.grab_default();
    buttonChange.signal_clicked().connect(sigc::mem_fun(*this,
                                                     &MainWindow::on_button_change_clicked) );

// Make the button the default widget

// Connect the clicked signal of the button to
// Show all children of the window
    show_all_children();
}

MainWindow::~MainWindow()
{
}

void MainWindow::on_button_reg_clicked() {
    std::string login = entryLogin.get_text();
    soci::rowset<soci::row> rs = (sql_.prepare << "SELECT * FROM users WHERE login = :login",
            soci::use(login, "login"));

    bool found = false;
    for (auto &r: rs) {
        found = true;
        break;
    }
    if(found){
        Gtk::MessageDialog dialog("User with this login already exists");
        dialog.run();
    }
    else {
        std::string pwd1 = entryPwd1.get_text();
        std::string pwd2 = entryPwd2.get_text();
        if (pwd1 == pwd2) {
            if (commonPwds.find(pwd1) != commonPwds.end()){
                Gtk::MessageDialog dialog("This password is commonly used. Try another one");
                std::cout << commonPwds.size();
                dialog.run();
            }
            else {
                sql_ << "INSERT INTO users(login, pwd, admin) VALUES(:login, SHA2(:pwd, 256), false)",
                        use(login, "login"), use(pwd1, "pwd");
                Gtk::MessageDialog dialog("Successfully added user");
                dialog.run();
            }
        } else {
            Gtk::MessageDialog dialog("Passwords does not match");
            dialog.run();
        }
    }
}

void MainWindow::on_button_change_clicked() {
    std::string curPwd = entryCurPwd.get_text();
    std::string pwd1 = entryNewPwd1.get_text();
    std::string pwd2 = entryNewPwd2.get_text();

    soci::rowset<soci::row> rs = (sql_.prepare << "SELECT * FROM users WHERE id = :id"
                                                  " and pwd = SHA2(:pwd, 256)",
            soci::use(id_, "id"), soci::use(curPwd, "pwd"));

    bool found = false;
    for (auto &r: rs) {
        found = true;
        break;
    }
    if(found) {
        if (pwd1 == pwd2) {
            if (commonPwds.find(pwd1) != commonPwds.end()){
                Gtk::MessageDialog dialog("This password is commonly used. Try another one");
                std::cout << commonPwds.size();
                dialog.run();
            }
            else {
                sql_ << "UPDATE users SET pwd = SHA2(:pwd, 256) WHERE id = :id",
                        use(id_, "id"), use(pwd1, "pwd");
                Gtk::MessageDialog dialog("Password changed");
                dialog.run();
            }
        } else {
            Gtk::MessageDialog dialog("Passwords does not match");
            dialog.run();
        }
    }
    else{
        Gtk::MessageDialog dialog("Wrong current password");
        dialog.run();
    }
}