#include "qtmainwindow.h"
#include "./ui_qtmainwindow.h"

QtMainWindow::QtMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::QtMainWindow)
{
    ui->setupUi(this);
}

QtMainWindow::~QtMainWindow()
{
    delete ui;
}
