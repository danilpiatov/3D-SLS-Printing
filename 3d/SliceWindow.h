//
// Created by dan on 5/13/23.
//
#ifndef SLICEWINDOW_H
#define SLICEWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QToolButton>
#include "myarea.h"
#include "userstab.h"
#include "authmanager.h"

class SliceWindow : public QMainWindow
{
    Q_OBJECT



public:
    SliceWindow();
    SliceWindow(const UserInfo &userInfo, QWidget *parent);
    virtual ~SliceWindow();

protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void browseModelFile();
    void on_button_start_clicked();
    void on_button_prev_clicked();
    void on_button_next_clicked();
    void on_button_laser_width_clicked();

private:
    // Widgets
    QWidget *mainContentWidget;
    QHBoxLayout *hBoxMain;
    QVBoxLayout *vBoxArea;
    QVBoxLayout *vBoxOptions;

    QGroupBox *modelGroup;
    QGroupBox *sliceControlGroup;
    QGroupBox *navigationGroup;
    QGroupBox *laserGroup;

    QHBoxLayout *startPointBox;
    QHBoxLayout *prevNextBox;
    QHBoxLayout *scaleBox;

    QLabel *labelScale;
    QLabel *labelModelPath;
    QLineEdit *entryModelPath;
    QLabel *labelSliceNum;
    QLineEdit *entrySliceNum;
    QLabel *labelLaserWidth;
    QLineEdit *entryLaserWidth;
    QLabel *labelStartPointX;
    QLineEdit *entryStartPointX;
    QLabel *labelStartPointY;
    QLineEdit *entryStartPointY;
    QPushButton *buttonStart;
    QPushButton *buttonNext;
    QPushButton *buttonPrev;
    QCheckBox *laserSizeButton;
    MyArea *area;

    // Variables
    int curNum_ = 0;
    int minNum_ = 0;
    int maxNum_ = 0;
    double l_;
    double h_;
    double step_;
    ModelSlicer model_;
    std::vector<Polygons> polygons_;
    // Элементы для админа
    QTabWidget *adminTabs;
    UserManagementTab *userManagementTab;
    QVBoxLayout *mainLayout;
    void setupMainInterface();
    void setupAdminInterface();
    QString currentUserRole;
};

#endif // SLICEWINDOW_H
