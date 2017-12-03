#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qtwain.h>
#include <QPixmap>
#include <QThread>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    QTwain twain;
    QPixmap curr;
    QThread t;
    ~MainWindow();

public slots:
    void scanTaken(HBITMAP img);
    void scaning();
    void saving();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
