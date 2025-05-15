#include "SliceWindow.h"
#include <QFrame>
#include <QKeyEvent>
#include <QIntValidator>
#include <QApplication>
#include <QFileDialog>

SliceWindow::SliceWindow() : QMainWindow()
{
    setWindowTitle("SLS 3D Printing Suite");
    setMinimumSize(1280, 720);

    // Стилизация приложения
    QFile styleFile(":/resources/modern.qss");
    styleFile.open(QFile::ReadOnly);
    QString style(styleFile.readAll());
    qApp->setStyleSheet(style);

    // Create central widget and main layout
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    hBoxMain = new QHBoxLayout(centralWidget);
    hBoxMain->setContentsMargins(12, 12, 12, 12);
    hBoxMain->setSpacing(20);

    // Create area widget and its layout
    vBoxArea = new QVBoxLayout();
    area = new MyArea();
    area->setMinimumSize(800, 600);
    vBoxArea->addWidget(area);

    // Create options panel
    vBoxOptions = new QVBoxLayout();
    vBoxOptions->setSpacing(15);

    // Model Settings Group
    modelGroup = new QGroupBox("Model Settings");
    QVBoxLayout *modelLayout = new QVBoxLayout(modelGroup);
    modelLayout->setContentsMargins(10, 15, 10, 10);
    modelLayout->setSpacing(8);

    labelModelPath = new QLabel("Model File Path");
    entryModelPath = new QLineEdit();
    entryModelPath->setPlaceholderText("Enter path to model...");
    modelLayout->addWidget(labelModelPath);
    modelLayout->addWidget(entryModelPath);
    vBoxOptions->addWidget(modelGroup);

    // Slice Control Group
    sliceControlGroup = new QGroupBox("Slice Parameters");
    QGridLayout *sliceGrid = new QGridLayout(sliceControlGroup);
    sliceGrid->setContentsMargins(10, 15, 10, 10);
    sliceGrid->setHorizontalSpacing(15);
    sliceGrid->setVerticalSpacing(10);

    labelSliceNum = new QLabel("Layers:");
    entrySliceNum = new QLineEdit();
    entrySliceNum->setValidator(new QIntValidator(1, 1000, this));

    labelLaserWidth = new QLabel("Laser Width:");
    entryLaserWidth = new QLineEdit();
    entryLaserWidth->setValidator(new QDoubleValidator(0.01, 10.0, 2, this));

    labelStartPointX = new QLabel("Start Point (X/Y):");
    entryStartPointX = new QLineEdit();
    entryStartPointX->setValidator(new QDoubleValidator(this));
    entryStartPointY = new QLineEdit();
    entryStartPointY->setValidator(new QDoubleValidator(this));

    sliceGrid->addWidget(labelSliceNum, 0, 0);
    sliceGrid->addWidget(entrySliceNum, 0, 1);
    sliceGrid->addWidget(labelLaserWidth, 1, 0);
    sliceGrid->addWidget(entryLaserWidth, 1, 1);
    sliceGrid->addWidget(labelStartPointX, 2, 0, 1, 2);
    QHBoxLayout *startPointLayout = new QHBoxLayout();
    startPointLayout->addWidget(entryStartPointX);
    startPointLayout->addWidget(entryStartPointY);
    sliceGrid->addLayout(startPointLayout, 3, 0, 1, 2);

    buttonStart = new QPushButton(QIcon(":/resources/start.svg"), "Process Model");
    sliceGrid->addWidget(buttonStart, 4, 0, 1, 2);
    vBoxOptions->addWidget(sliceControlGroup);

    // Navigation Group
    navigationGroup = new QGroupBox("Navigation");
    QGridLayout *navGrid = new QGridLayout(navigationGroup);
    navGrid->setContentsMargins(10, 15, 10, 10);
    navGrid->setSpacing(10);

    buttonPrev = new QPushButton(QIcon(":/resources/prev.svg"), "");
    buttonNext = new QPushButton(QIcon(":/resources/next.svg"), "");


    QLabel *scaleLabel = new QLabel("Layers:");
    navGrid->addWidget(scaleLabel, 0, 0);
    navGrid->addWidget(buttonPrev, 0, 1);
    navGrid->addWidget(buttonNext, 0, 2);


    laserSizeButton = new QCheckBox("Real Laser Size");
    navGrid->addWidget(laserSizeButton, 2, 0, 1, 3);
    vBoxOptions->addWidget(navigationGroup);

    // Add stretch to push everything up
    vBoxOptions->addStretch();

    // Connect signals
    connect(buttonStart, &QPushButton::clicked, this, &SliceWindow::on_button_start_clicked);
    connect(buttonPrev, &QPushButton::clicked, this, &SliceWindow::on_button_prev_clicked);
    connect(buttonNext, &QPushButton::clicked, this, &SliceWindow::on_button_next_clicked);
    connect(laserSizeButton, &QCheckBox::clicked, this, &SliceWindow::on_button_laser_width_clicked);

    // Add layouts to main layout
    hBoxMain->addLayout(vBoxOptions, 0);
    hBoxMain->addLayout(vBoxArea, 1);

    QHBoxLayout *pathLayout = new QHBoxLayout();
    QToolButton *browseButton = new QToolButton();
    browseButton->setIcon(QIcon(":/resources/folder.svg"));
    browseButton->setToolTip("Browse model file");
    connect(browseButton, &QToolButton::clicked, this, &SliceWindow::browseModelFile);
    pathLayout->addWidget(entryModelPath);
    pathLayout->addWidget(browseButton);
    vBoxOptions->addLayout(pathLayout);

    entrySliceNum->setValidator(new QIntValidator(1, 10000, this));
    entryLaserWidth->setValidator(new QDoubleValidator(0.01, 10.0, 2, this));
    entryStartPointX->setValidator(new QDoubleValidator(this));
    entryStartPointY->setValidator(new QDoubleValidator(this));

// Добавим подсказки
    entryModelPath->setToolTip("Path to 3D model file\nSupported formats: STL, OBJ, 3MF");
    buttonStart->setToolTip("Start slicing process (Ctrl+Enter)");


}

SliceWindow::~SliceWindow()
{
}

void SliceWindow::browseModelFile()
{
    QString path = QFileDialog::getOpenFileName(this,
                                                "Select 3D Model",
                                                QDir::homePath(),
                                                "3D Files (*.stl *.obj *.3mf)");

    if(!path.isEmpty()) {
        entryModelPath->setText(path);
    }
}

void SliceWindow::on_button_start_clicked() {
    polygons_.clear();

    curNum_ = 0;
    std::string path = entryModelPath->text().toStdString();

    int sliceNum = std::stoi(entrySliceNum->text().toStdString());
    double laserWidth = std::stod(entryLaserWidth->text().toStdString());
    double startX = std::stod(entryStartPointX->text().toStdString());
    double startY = std::stod(entryStartPointY->text().toStdString());
    model_.parse(path.c_str());
    h_ = model_.findHighestPoint();
    l_ = model_.findLowestPoint();
    step_ = (h_-l_)/(sliceNum + 1);
    maxNum_ = sliceNum - 1;
    for (int i = 0; i < sliceNum; ++i){
        Polygons p;
        std::vector<Line> lines = model_.slice(l_ + step_ * (i + 1));
        p.polygonizeAll(lines);
        polygons_.push_back(p);
    }
    while(maxNum_ >= 0 && polygons_[maxNum_].getPolygons().empty()) {
        maxNum_--;
    }
    while(curNum_ < maxNum_ && polygons_[curNum_].getPolygons().empty()) {
        curNum_++;
    }
    minNum_ = curNum_;
    if (maxNum_ > -1) {
        Point startPoint = Point{startX, startY};
        double width = model_.findRightPoint() - model_.findLeftPoint();
        double height = model_.findClosePoint() - model_.findFarPoint();
        double x = model_.findLeftPoint() + width/2;
        double y = model_.findFarPoint() + height/2;

        area->create(polygons_[curNum_], startPoint, laserWidth, area->width(), area->height(), {x,y});
    }
    area->update();
}

void SliceWindow::on_button_prev_clicked() {
    if(curNum_ > minNum_) {
        curNum_--;
        area->prev(polygons_[curNum_]);
        area->update();
    }
}

void SliceWindow::on_button_next_clicked() {
    if(curNum_ < maxNum_) {
        curNum_++;
        area->change(polygons_[curNum_]);
        area->update();
    }
}

void SliceWindow::on_button_plus_clicked() {
    area->coefPlus();
    area->update();
}

void SliceWindow::on_button_minus_clicked() {
    area->coefMinus();
    area->update();
}

void SliceWindow::keyPressEvent(QKeyEvent *event)
{
    switch(event->key()) {
        case Qt::Key_Left:
            area->moveLeft();
            break;
        case Qt::Key_Right:
            area->moveRight();
            break;
        case Qt::Key_Up:
            area->moveUp();
            break;
        case Qt::Key_Down:
            area->moveDown();
            break;
        default:
            QMainWindow::keyPressEvent(event);
            return;
    }
    area->update();
}

void SliceWindow::on_button_laser_width_clicked() {
    area->setRealLW();
    area->update();
}
