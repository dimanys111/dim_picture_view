#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileSystemModel>
#include <QListWidgetItem>
#include <QTime>
#include <QTimer>
#include "QProcess"

#include <work.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void add_item();
    
public slots:
    void clearSpis();
    void obnSpis(QListWidgetItem *item);
private slots:
    void select(const QItemSelection &index, const QItemSelection &index1);
    void on_lineEdit_returnPressed();
    void sobTimer();

private:
    bool Alt;
    int size;
    QProcess proc;
    QTime t;
    QTimer timer;
    QList<QListWidgetItem*> spis_prom;
    Work work;
    QFileSystemModel* fsModel;
    Ui::MainWindow *ui;
    void wheelEvent(QWheelEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

    void setDir(QString s);
signals:
    void getSpis(QString s);
};

#endif // MAINWINDOW_H
