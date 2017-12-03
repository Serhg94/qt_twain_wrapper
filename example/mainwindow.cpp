#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDir>
#include <QFileDialog>

#include <qDebug>

Q_GUI_EXPORT QPixmap qt_pixmapFromWinHBITMAP(HBITMAP bitmap, int hbitmapFormat = 0);

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    twain.initTwain(this->winId());
    twain.selectSource();
    connect(&twain, SIGNAL(pixmapReceived(HBITMAP)), this, SLOT(scanTaken(HBITMAP)));
    connect(this->ui->Scan, SIGNAL(clicked(bool)), &twain, SLOT(getScan()));
    connect(this->ui->save, SIGNAL(clicked(bool)), this, SLOT(saving()));
}

void MainWindow::scaning()
{
    twain.getScans(1);
}

void MainWindow::saving()
{
    QImage img;
    img = curr.toImage();
    QString format = "jpg";
    QString initialPath = QDir::currentPath();
    QString fileName = QFileDialog::getSaveFileName(this, "Сохранить",
                               initialPath,
                               QString("%1 Files (*.%2);;All Files (*)")
                               .arg(format.toUpper())
                               .arg(format));
    if (!fileName.isEmpty())
    {
        img.save(fileName, 0);
    }
}

void MainWindow::scanTaken(HBITMAP img)
{
    curr = QPixmap(qt_pixmapFromWinHBITMAP( img,Qt::NoAlpha));
    this->ui->label->setPixmap(curr);
}

MainWindow::~MainWindow()
{
    delete ui;
}
