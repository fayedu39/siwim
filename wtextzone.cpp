#include "wtextzone.h"
#include "ui_wtextzone.h"
#include <QMessageBox>
#include <QString>
#include <QTextStream>
#include <QDebug>

WTextZone::WTextZone(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WTextZone)
{
    ui->setupUi(this);

    ui->formatedDisplay->setOpenLinks(false);

    viewMode();
}

WTextZone::~WTextZone()
{
    if(!saved) {
        QMessageBox::StandardButton save=QMessageBox::question(this,"Enregistrer ?","L'onglet que vous allez fermer ("+title+") n'a pas été enregistré, que voulez-vous faire ?",QMessageBox::Save|QMessageBox::Discard);
        if(save==QMessageBox::Save) {
            this->saveText();
        }
    }
    delete ui;
}

void WTextZone::saveText() {
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    //if (file.open(QIODevice::ReadWrite | QIODevice::Text)) {
        QTextStream out(&file);
        //QString text=ui->plainTextEdit->toPlainText();
        //out.setString(&text);
        //out<<text;
        out<<ui->plainTextEdit->toPlainText();
        out.flush();
        //file.flush();
        file.close();
        saved=true;
        emit pageSaved(title);
    }
}

void WTextZone::setDir(const QString d) {
    dir=d;
}

void WTextZone::setTitle(const QString t) {
    bool s=false;
    title=t;
    QString text="";
    file.setFileName(dir+"/"+title);
    //qDebug()<<file.fileName();
    //if (file.open(QIODevice::ReadWrite | QIODevice::Text)) {
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        s=true;
        QTextStream in(&file);
        text=in.readAll();
        file.close();
    }

    ui->plainTextEdit->setPlainText(text);
    ui->formatedDisplay->setHtml(toHtml(text));
    saved=s;
}

void WTextZone::editMode() {
    ui->editButton->hide();
    ui->formatedDisplay->hide();
    ui->bar->show();
    ui->plainTextEdit->show();
    ui->plainTextEdit->setEnabled(true);
}

void WTextZone::viewMode() {
    saveText();
    ui->bar->hide();
    ui->plainTextEdit->hide();
    ui->plainTextEdit->setEnabled(false);
    ui->editButton->show();
    ui->formatedDisplay->show();
    ui->formatedDisplay->setHtml(toHtml(ui->plainTextEdit->toPlainText()));
    //qDebug()<<ui->formatedDisplay->toHtml();
}

void WTextZone::textChanged() {
    saved=false;
}

void WTextZone::linkClick(QUrl url) {
    emit linkClicked(url);
}

QString WTextZone::toHtml(const QString text,const QString ext) {
    QString html=text;
    html.replace("\n----\n","<hr/>");
    int bodyStart=html.indexOf("\n##body##\n");
    if(bodyStart!=(-1)) {
        html.replace(bodyStart,10,"</td></tr></table><table width=\"70%\"><tr><td>");
        html.append("</td></tr></table>");
        html.prepend("<table style=\"float:right;page-break-before:always;border-style:solid;border-color:black;\" border=\"1\" cellspacing=\"0\" width=\"25%\"><tr><td>");
    }
    html.replace(QRegExp("</h([1-6])>\n"),"</h\\1>");
    html.replace(QRegExp("\n<h([1-6])>"),"<h\\1>");
    html.replace("\n","<br/>");
    html.replace(QRegExp("\\[\\[([^\\]]+)\\]\\]\\(\\(([^\\)]+)\\)\\)"),"<a href=\"\\1"+ext+"\">\\2</a>");
    html.replace(QRegExp("\\[\\[([^\\]]+)\\]\\]"),"<a href=\"\\1"+ext+"\">\\1</a>");
    return html;
}
