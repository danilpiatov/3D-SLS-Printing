#include <QApplication>
#include "loginWindow.h"
#include "SliceWindow.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    AuthWindow authWindow;
    if(authWindow.exec() != QDialog::Accepted) {
        return 0;
    }

    SliceWindow mainWindow;
    mainWindow.show();

    return a.exec();
}
