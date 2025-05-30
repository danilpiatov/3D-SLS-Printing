#include "loginWindow.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>
#include <QFile>
#include <QApplication>

AuthWindow::AuthWindow(QWidget *parent) : QDialog(parent) {
    setWindowTitle("Авторизация");
    setFixedSize(300, 200);

    QVBoxLayout *layout = new QVBoxLayout(this);

    usernameInput = new QLineEdit(this);
    usernameInput->setPlaceholderText("Логин");

    passwordInput = new QLineEdit(this);
    passwordInput->setPlaceholderText("Пароль");
    passwordInput->setEchoMode(QLineEdit::Password);

    QPushButton *loginBtn = new QPushButton("Войти", this);
    connect(loginBtn, &QPushButton::clicked, this, &AuthWindow::tryLogin);

    layout->addWidget(usernameInput);
    layout->addWidget(passwordInput);
    layout->addWidget(loginBtn);
    QFile styleFile(":/resources/modern.qss");
    styleFile.open(QFile::ReadOnly);
    QString style(styleFile.readAll());
    qApp->setStyleSheet(style);
}

void AuthWindow::tryLogin() {
    AuthManager authManager;
    userInfo = authManager.authenticate(usernameInput->text(), passwordInput->text());

    if(userInfo.isAuthenticated) {
        accept();
    } else {
        QMessageBox::warning(this, "Error", "Invalid credentials");
    }
}