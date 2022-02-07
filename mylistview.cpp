#include "mylistview.h"
#include "QAction"
#include "QMenu"
#include "QContextMenuEvent"
#include "QDesktopServices"
#include "QDir"
#include "QFileInfo"

void MyListView::Creat()
{
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setViewMode(QListWidget::IconMode);
    setIconSize(QSize(200,200));
    setResizeMode(QListWidget::Adjust);

    // create my context menu with 2 options
    m_menuOption1 = new QAction(tr("Добавить"), this);
    m_menuOption1 ->setEnabled(true);

    m_myListContextMenu = new QMenu(this);
    m_myListContextMenu ->addAction(m_menuOption1);

    // process primary selection

    connect(this, &MyListView::itemActivated, this, &MyListView::itemActivated_slot);
    connect(this, SIGNAL(itemSelectionChanged(void)), this, SLOT(slot_ItemSelectionChanged(void)));
    // process menu option 1
    connect(m_menuOption1, SIGNAL(triggered()), this, SLOT(slot_Option1()));
}

MyListView::MyListView()
{
    Creat();
}

MyListView::MyListView(QSplitter *s) :
    QListWidget(s)
{
    Creat();
}

void MyListView::slot_ItemSelectionChanged(void)
{
  if (selectedItems() != m_selectedItem)
  {
    m_selectedItem = selectedItems();
    //emit signal_ItemSelectionChanged(m_selectedItem->type());
  }
} // end of method slot_ItemSelectionChanged()

void MyListView::itemActivated_slot(QListWidgetItem *item)
{
    m_act_item=item;
}

void MyListView::slot_Option1()
{
    QDir d;
    d.mkpath("d:/Pro");
    QString s=m_selectedItem.first()->data(Qt::StatusTipRole).toString();
    QFile f(s);
    QFileInfo fi(f);
    f.copy("d:/Pro/"+fi.fileName());
}

void MyListView::contextMenuEvent(QContextMenuEvent* menuEvent)
{
  m_rightClickedItem = itemAt(menuEvent->pos());
  setItemSelected(m_rightClickedItem,true);
  m_myListContextMenu->exec(menuEvent->globalPos());
}

void MyListView::mousePressEvent(QMouseEvent* mousePressEvent)
{
  if (!(mousePressEvent->buttons() & Qt::RightButton))
  {
    QListWidget::mousePressEvent(mousePressEvent);
  }
}

void MyListView::mouseDoubleClickEvent(QMouseEvent *event)
{
    QString s=itemAt(event->pos())->data(Qt::StatusTipRole).toString();
    #ifdef Q_OS_WIN32
        QUrl url("file:/"+s);
    #endif

    #ifdef Q_OS_LINUX
        QUrl url("file://"+s);
    #endif

    QDesktopServices::openUrl(url);
} // end of method mousePressEvent()

void MyListView::keyPressEvent(QKeyEvent *event)
{
  switch (event->key())
      {
  case Qt::Key_Delete:
    foreach (QListWidgetItem* item, m_selectedItem)
    {
        QString s=item->data(Qt::StatusTipRole).toString();
        QFile f(s);
        bool b=f.remove();
        if (b)
            takeItem(row(item));
    }
  break;
  case Qt::Key_Return:
    foreach (QListWidgetItem* item, m_selectedItem)
    {
        QString s=item->data(Qt::StatusTipRole).toString();
        #ifdef Q_OS_WIN32
            QUrl url("file:/"+s);
        #endif

        #ifdef Q_OS_LINUX
            QUrl url("file://"+s);
        #endif
        QDesktopServices::openUrl(url);
    }
  break;
  default: break;
      }
}
