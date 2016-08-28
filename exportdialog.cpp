#include "exportdialog.h"
#include "ui_exportdialog.h"

exportDialog::exportDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::exportDialog)
{
    ui->setupUi(this);
}

QString exportDialog::wName() {
    return ui->lineEditNom->text();
}
QString exportDialog::indexPage() {
    return ui->lineEditLinksPage->text();
}
bool exportDialog::dispCats() {
    return ui->checkBoxCategoriesLinks->isChecked();
}
bool exportDialog::dispWName() {
    return ui->checkBoxNameTop->isChecked();
}
bool exportDialog::titleWName() {
    return ui->checkBoxNameTitle->isChecked();
}
bool exportDialog::linkIndex() {
    return ui->checkBoxIndexLink->isChecked();
}

exportDialog::~exportDialog()
{
    delete ui;
}
