#include <QDialog>
#include <QLineEdit>

class AuthWindow : public QDialog {
Q_OBJECT
public:
    explicit AuthWindow(QWidget *parent = nullptr);

private slots:
    void tryLogin();

private:
    QLineEdit *usernameInput;
    QLineEdit *passwordInput;
};
