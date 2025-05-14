#include "SliceWindow.h"
#include <QFrame>
#include <QKeyEvent>

SliceWindow::SliceWindow() : QMainWindow()
{
    setWindowTitle("SLS 3D printing app");

    // Create central widget and main layout
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    hBoxMain = new QHBoxLayout(centralWidget);

    // Create area widget and its layout
    vBoxArea = new QVBoxLayout();
    area = new MyArea();
    vBoxArea->addWidget(area);

    // Create options panel
    vBoxOptions = new QVBoxLayout();

    // Model path
    labelModelPath = new QLabel("Path to model");
    entryModelPath = new QLineEdit();
    vBoxOptions->addWidget(labelModelPath);
    vBoxOptions->addWidget(entryModelPath);
    vBoxOptions->addWidget(new QFrame()); // Separator

    // Slice number
    labelSliceNum = new QLabel("Slice num");
    entrySliceNum = new QLineEdit();
    vBoxOptions->addWidget(labelSliceNum);
    vBoxOptions->addWidget(entrySliceNum);
    vBoxOptions->addWidget(new QFrame()); // Separator

    // Laser width
    labelLaserWidth = new QLabel("Laser width");
    entryLaserWidth = new QLineEdit();
    vBoxOptions->addWidget(labelLaserWidth);
    vBoxOptions->addWidget(entryLaserWidth);
    vBoxOptions->addWidget(new QFrame()); // Separator

    // Start point
    labelStartPointX = new QLabel("Start point");
    entryStartPointX = new QLineEdit();
    entryStartPointX->setFixedWidth(70);
    entryStartPointY = new QLineEdit();
    entryStartPointY->setFixedWidth(70);
    startPointBox = new QHBoxLayout();
    startPointBox->addWidget(entryStartPointX);
    startPointBox->addWidget(entryStartPointY);
    vBoxOptions->addWidget(labelStartPointX);
    vBoxOptions->addLayout(startPointBox);
    vBoxOptions->addWidget(new QFrame()); // Separator

    // Start button
    buttonStart = new QPushButton("Start");
    vBoxOptions->addWidget(buttonStart);
    vBoxOptions->addWidget(new QFrame()); // Separator

    // Prev/Next buttons
    buttonPrev = new QPushButton("Prev");
    buttonNext = new QPushButton("Next");
    prevNextBox = new QHBoxLayout();
    prevNextBox->addWidget(buttonPrev);
    prevNextBox->addWidget(buttonNext);
    vBoxOptions->addLayout(prevNextBox);
    vBoxOptions->addWidget(new QFrame()); // Separator

    // Scale controls
    labelScale = new QLabel("Scale");
    buttonMinus = new QPushButton("-");
    buttonPlus = new QPushButton("+");
    scaleBox = new QHBoxLayout();
    scaleBox->addWidget(buttonMinus);
    scaleBox->addWidget(buttonPlus);
    vBoxOptions->addWidget(labelScale);
    vBoxOptions->addLayout(scaleBox);
    vBoxOptions->addWidget(new QFrame()); // Separator

    // Laser size checkbox
    laserSizeButton = new QCheckBox("Show real laser size");
    vBoxOptions->addWidget(laserSizeButton);

    // Add stretch to push everything up
    vBoxOptions->addStretch();

    // Connect signals
    connect(buttonStart, &QPushButton::clicked, this, &SliceWindow::on_button_start_clicked);
    connect(buttonPrev, &QPushButton::clicked, this, &SliceWindow::on_button_prev_clicked);
    connect(buttonNext, &QPushButton::clicked, this, &SliceWindow::on_button_next_clicked);
    connect(buttonPlus, &QPushButton::clicked, this, &SliceWindow::on_button_plus_clicked);
    connect(buttonMinus, &QPushButton::clicked, this, &SliceWindow::on_button_minus_clicked);
    connect(laserSizeButton, &QCheckBox::clicked, this, &SliceWindow::on_button_laser_width_clicked);

    // Add layouts to main layout
    hBoxMain->addLayout(vBoxOptions);
    hBoxMain->addLayout(vBoxArea, 1); // Give more space to the drawing area
}

SliceWindow::~SliceWindow()
{
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

        area->create(polygons_[curNum_], startPoint, laserWidth, width, height, {x,y});
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
