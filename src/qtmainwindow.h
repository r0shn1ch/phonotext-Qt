#ifndef QTMAINWINDOW_H
#define QTMAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class QtMainWindow;
}
QT_END_NAMESPACE

class QtMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    QtMainWindow(QWidget *parent = nullptr);
    ~QtMainWindow();

private:
    Ui::QtMainWindow *ui;
};
#endif // QTMAINWINDOW_H
