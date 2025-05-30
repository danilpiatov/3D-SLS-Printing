#include <QApplication>
#include "loginWindow.h"
#include "SliceWindow.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
#ifdef USE_AUTH
    AuthWindow authWindow;
    if(authWindow.exec() != QDialog::Accepted) {
        return 0;
    }
    UserInfo userInfo = authWindow.getUserInfo();
    SliceWindow mainWindow(userInfo, nullptr);
#else
    SliceWindow mainWindow({true, "a", "user"}, nullptr);
#endif
    mainWindow.show();
    return a.exec();

    return 0;
}
