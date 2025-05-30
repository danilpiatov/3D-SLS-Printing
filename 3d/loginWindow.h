#include <QDialog>
#include <QLineEdit>
#include "authmanager.h"

class AuthWindow : public QDialog {
Q_OBJECT

public:
    explicit AuthWindow(QWidget *parent = nullptr);
    [[nodiscard]] UserInfo getUserInfo() const { return userInfo; }
private slots:
    void tryLogin();

private:
    UserInfo userInfo;
    QLineEdit *usernameInput;
    QLineEdit *passwordInput;
};
