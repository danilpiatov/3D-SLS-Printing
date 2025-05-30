#pragma once
#include <QWidget>
#include <QTableWidget>
#include <QPushButton>

class UserManagementTab : public QWidget {
Q_OBJECT
public:
    explicit UserManagementTab(QWidget *parent = nullptr);

private slots:
    void refreshUsers();
    void addUser();
    void deleteUser();

private:
    QTableWidget *usersTable;
    QPushButton *addButton;
    QPushButton *deleteButton;

    void setupUI();
};