#include "userstab.h"
#include "authmanager.h"
#include <QVBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QFormLayout>
#include <QComboBox>
#include <QLineEdit>
#include <QDialogButtonBox>

UserManagementTab::UserManagementTab(QWidget *parent) : QWidget(parent) {
    setupUI();
    refreshUsers();
}

void UserManagementTab::setupUI() {
    QVBoxLayout *layout = new QVBoxLayout(this);

    usersTable = new QTableWidget(this);
    usersTable->setColumnCount(2);
    usersTable->setHorizontalHeaderLabels({"Username", "Role"});
    usersTable->horizontalHeader()->setStretchLastSection(true);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    addButton = new QPushButton("Add User", this);
    deleteButton = new QPushButton("Delete User", this);

    connect(addButton, &QPushButton::clicked, this, &UserManagementTab::addUser);
    connect(deleteButton, &QPushButton::clicked, this, &UserManagementTab::deleteUser);

    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(deleteButton);

    layout->addWidget(usersTable);
    layout->addLayout(buttonLayout);
}

void UserManagementTab::refreshUsers() {
    AuthManager auth;
    auto users = auth.getAllUsers();

    usersTable->setRowCount(users.size());
    for(int i = 0; i < users.size(); ++i) {
        usersTable->setItem(i, 0, new QTableWidgetItem(users[i].first));
        usersTable->setItem(i, 1, new QTableWidgetItem(users[i].second));
    }
}

void UserManagementTab::addUser() {
    QDialog dialog(this);
    QFormLayout form(&dialog);

    QLineEdit *usernameInput = new QLineEdit(&dialog);
    QLineEdit *passwordInput = new QLineEdit(&dialog);
    passwordInput->setEchoMode(QLineEdit::Password);
    QComboBox *roleCombo = new QComboBox(&dialog);
    roleCombo->addItems({"admin", "user"});

    form.addRow("Username:", usernameInput);
    form.addRow("Password:", passwordInput);
    form.addRow("Role:", roleCombo);

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                               Qt::Horizontal, &dialog);
    form.addRow(&buttonBox);

    connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if(dialog.exec() == QDialog::Accepted) {
        AuthManager auth;
        if(auth.registerUser(usernameInput->text(),
                             passwordInput->text(),
                             roleCombo->currentText())) {
            refreshUsers();
        } else {
            QMessageBox::critical(this, "Error", "Failed to add user");
        }
    }
}

void UserManagementTab::deleteUser() {
    QItemSelectionModel *selection = usersTable->selectionModel();
    if(!selection->hasSelection()) return;

    int row = selection->currentIndex().row();
    QString username = usersTable->item(row, 0)->text();

    if(QMessageBox::question(this, "Confirm",
                             QString("Delete user %1?").arg(username)) == QMessageBox::Yes) {
        AuthManager auth;
        if(auth.deleteUser(username)) {
            refreshUsers();
        }
    }
}