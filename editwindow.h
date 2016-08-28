#ifndef EDITWINDOW_H
#define EDITWINDOW_H

#include <QtWidgets>
#include <QStringList>
#include <QStringListModel>
#include <QUrl>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QSettings>
#include <QProgressDialog>
#include <QMap>

namespace Ui {
class EditWindow;
}

class EditWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit EditWindow(QWidget *parent = 0);
    ~EditWindow();
    QStringList getLinksList(QString);
    QString categoryName(QString);
    QString catFileName(QString);

public slots:
    void addTab(QWidget * widget,const QString title);
    void closeTab(int id);
    //void openPage(QModelIndex mi);
    void openPage(QTreeWidgetItem*);
    void openPage(QString title);
    void openPage(QUrl);
    void createPage();
    void about();
    void aboutQt();
    void openWiki();
    void openWiki(const QString);
    void exportWiki();
    void exportWiki(const QString);
    void pageSaved(const QString);
    void dispTree();
    void generateIndex();

private:
    Ui::EditWindow *ui;
    QStringList listePages;
    QStringList openedPages;
    //QStringListModel *modele;
    //QFileSystemModel *modele;
    QMap<QString,QStringList> categories;
    QMap<QString,QStringList> categoriesByPage;
    QStringList noCatPages;
    QString dir;
};

#endif // EDITWINDOW_H
