#ifndef WTEXTZONE_H
#define WTEXTZONE_H

#include <QWidget>
#include <QUrl>
#include <QFile>
#include <QFileDialog>

namespace Ui {
class WTextZone;
}

class WTextZone : public QWidget
{
    Q_OBJECT

public:
    explicit WTextZone(QWidget *parent = 0);
    ~WTextZone();
    static QString toHtml(const QString,const QString="");

public slots:
    void saveText();
    void editMode();
    void viewMode();
    void textChanged();
    void linkClick(QUrl url);
    void setTitle(const QString);
    void setDir(const QString);

signals:
    void linkClicked(QUrl url);
    void pageSaved(QString);

private:
    Ui::WTextZone *ui;
    bool saved;
    QString title;
    QFile file;
    QString dir;
};

#endif // WTEXTZONE_H
