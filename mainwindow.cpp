#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "QLabel"
#include "QDesktopServices"
#include "QUrl"
#include "QKeyEvent"
#include "QTextCodec"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    Alt=false;
    size=200;

    fsModel = new QFileSystemModel;
    fsModel->setReadOnly(true);
    fsModel->setFilter(QDir::AllDirs | QDir::NoDotAndDotDot);
    fsModel->setRootPath("/");
    ui->treeView->setModel(fsModel);
    connect(ui->treeView->selectionModel(),&QItemSelectionModel::selectionChanged,this,&MainWindow::select);
    for (int i = 1; i<fsModel->columnCount(); i++)
        ui->treeView->setColumnHidden(i, true);

    qRegisterMetaType<QList<QListWidgetItem*>>("QList<QListWidgetItem*>");
    qRegisterMetaType<QListWidgetItem*>("QListWidgetItem*");

    connect(&work.workItem,&WorkItem::emitObrFile,&work.workItem,&WorkItem::obrItem);
    connect(&work.workItem,&WorkItem::emitItem,this,&MainWindow::obnSpis);

    connect(this,&MainWindow::getSpis,&work,&Work::nach_getDirFiles);
    connect(&work,&Work::setDirFiles,&work.workItem,&WorkItem::setSpis);
    connect(&work.workItem,&WorkItem::setDirFiles,this,&MainWindow::clearSpis);
    connect(&timer,&QTimer::timeout,this,&MainWindow::sobTimer);

    setDir(Work::Dir_Tek);

    t.start();
    timer.start(400);
}

MainWindow::~MainWindow()
{
    work.stop=true;
    work.Mythread.exit();
    work.Mythread.wait();
    work.saveMap();
    delete ui;
}

void MainWindow::sobTimer()
{
    if(spis_prom.size()>0)
    {
        add_item();
    }
}

void MainWindow::wheelEvent(QWheelEvent *event)
{
    if(Alt)
    {
        size=size*(1+event->delta()*0.0005);
        ui->listWidget->setIconSize(QSize(size,size));
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if(event->key()==Qt::Key_Control)
    {
        Alt=true;
    }
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    if(event->key()==Qt::Key_Control)
    {
        Alt=false;
    }
}

void MainWindow::add_item()
{
    foreach (QListWidgetItem* item, spis_prom)
    {
        ui->listWidget->addItem(item);
    }
    spis_prom.clear();
}

void MainWindow::obnSpis(QListWidgetItem* item)
{
    spis_prom.append(item);
    int i=t.elapsed();
    if(i>400)
    {
        add_item();
        t.start();
    }
}

void MainWindow::clearSpis()
{
        int z=ui->listWidget->count();
        for(int i=0;i<z;i++)
        {
            QListWidgetItem*l=ui->listWidget->takeItem(0);
            if (l)
            {
                delete l;
            }
        }
        ui->listWidget->clear();
        spis_prom.clear();
}

void MainWindow::select(const QItemSelection &index,const QItemSelection &index1)
{
    work.stop=true;
    work.workItem.stop=true;
    QString s=fsModel->filePath(index.indexes().first());
    ui->lineEdit->setText(s);
    emit getSpis(s);
}

void MainWindow::setDir(QString s)
{
    ui->lineEdit->setText(s);
    on_lineEdit_returnPressed();
}

void MainWindow::on_lineEdit_returnPressed()
{
    QString s=ui->lineEdit->text();
    ui->treeView->setCurrentIndex(fsModel->index(s));
    ui->treeView->setExpanded(fsModel->index(s),true);
}
