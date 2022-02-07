#ifndef MYLISTVIEW_H
#define MYLISTVIEW_H

#include <QListWidget>
#include <QSplitter>

class MyListView : public QListWidget
{
  Q_OBJECT

  public:
    MyListView();
    MyListView(QSplitter* s);
    virtual ~MyListView() {}

    /** Add item to the list.
        @param itemName Item's display name.
        @param itemHandle Value to return if item selected.
    */
    void AddItem(const QString itemName, const quint32 itemHandle);

    void Creat();

private slots:
    void slot_ItemSelectionChanged(void);
    void slot_Option1(void);
    void itemActivated_slot(QListWidgetItem *item);


private:
    void mousePressEvent(QMouseEvent* mousePressEvent);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void contextMenuEvent(QContextMenuEvent* menuEvent);

    QListWidgetItem* m_act_item;
    QList<QListWidgetItem*> m_selectedItem;        ///< Pointer to the currently selected item.
    QListWidgetItem *m_rightClickedItem;    ///< Pointer to right-clicked item in the list.
    QAction* m_menuOption1;                 ///< Menu option 1.
    QMenu* m_myListContextMenu;             ///< Menu with all List View menu actions.
};

#endif // MYLISTVIEW_H
