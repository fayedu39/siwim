#ifndef EXPORTDIALOG_H
#define EXPORTDIALOG_H

#include <QDialog>

namespace Ui {
class exportDialog;
}

class exportDialog : public QDialog
{
    Q_OBJECT

public:
    explicit exportDialog(QWidget *parent = 0);
    ~exportDialog();
    QString wName();
    QString indexPage();
    bool dispWName();
    bool titleWName();
    bool dispCats();
    bool linkIndex();

private:
    Ui::exportDialog *ui;
};

#endif // EXPORTDIALOG_H
