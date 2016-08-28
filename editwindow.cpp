#include "editwindow.h"
#include "ui_editwindow.h"
#include "wtextzone.h"
#include "exportdialog.h"
#include <QDebug>

EditWindow::EditWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::EditWindow)
{
    ui->setupUi(this);

    //listePages << "France" << "Espagne" << "Italie" << "Portugal" << "Suisse";
    //modele = new QStringListModel(listePages);
    //modele = new QFileSystemModel();
    //modele->setFilter(QDir::Files);
    //ui->listView->setModel(modele);

    QSettings settings;

    QString lastdir=settings.value("lastWikiLocation").toString();
    if(!lastdir.isEmpty()) {
        openWiki(lastdir);
        QStringList lastopenedPages=settings.value("lastOpenedPages").toStringList();
        while(!lastopenedPages.isEmpty()) {
            QString page=lastopenedPages.takeFirst();
            if(listePages.contains(page)) {
                openPage(page);
            }
        }
    }
    bool showTips=settings.value("showTips",true).toBool();
    //qDebug()<<showTips;
    if(!showTips) {
        ui->tipsLabel->setVisible(showTips);
        ui->actionAstuces->setChecked(showTips);
    }
    bool showPagesList=settings.value("showPagesList",true).toBool();
    if(!showPagesList) {
        ui->listView->setVisible(showPagesList);
        ui->actionListe_des_pages->setChecked(showPagesList);
    }
    settings.sync();
}

EditWindow::~EditWindow()
{
    QSettings settings;
    settings.setValue("lastOpenedPages",openedPages);
    settings.setValue("showTips",ui->actionAstuces->isChecked());
    settings.setValue("showPagesList",ui->actionListe_des_pages->isChecked());
    delete ui;
}

void EditWindow::addTab(QWidget * widget,const QString title)
{
    ui->tabWidget->addTab(widget,title);

}

/*void EditWindow::openPage(QModelIndex mi)
{
    //QVariant elementSelectionne = modele->data(mi, Qt::DisplayRole);
    QVariant elementSelectionne = modele->data(mi, QFileSystemModel::FileNameRole);
    openPage(elementSelectionne.toString());

}*/
void EditWindow::openPage(QTreeWidgetItem * wi)
{
    if(wi->childCount()) {
        openPage(catFileName(wi->text(0)));
    } else {
        openPage(wi->text(0));
    }
}

void EditWindow::openPage(const QUrl url)
{
    openPage(url.toString());
}
void EditWindow::openPage(const QString title)
{
    if(!title.isEmpty()) {
        if(openedPages.contains(title)) {
            ui->tabWidget->setCurrentIndex(openedPages.indexOf(title));
        } else {
            openedPages<<title;
            WTextZone *TZ=new WTextZone();
            addTab(TZ,title);
            TZ->setDir(dir);
            TZ->setTitle(title);
            ui->tabWidget->setCurrentIndex(ui->tabWidget->indexOf(TZ));
            connect(TZ,SIGNAL(linkClicked(QUrl)),this,SLOT(openPage(QUrl)));
            connect(TZ,SIGNAL(pageSaved(QString)),this,SLOT(pageSaved(QString)));
        }
    }
}

void EditWindow::closeTab(int id)
{
    int cid=ui->tabWidget->currentIndex();
    if(cid>id)
        cid--;
    ui->tabWidget->setCurrentIndex(id);
    openedPages.removeOne(ui->tabWidget->tabText(id));
    delete ui->tabWidget->currentWidget();
    //ui->tabWidget->removeTab(id);
    ui->tabWidget->setCurrentIndex(cid);
}

void EditWindow::createPage() {
    bool ok=false;
    QString title=QInputDialog::getText(this,"Nouvelle page","Titre de la nouvelle page :",QLineEdit::Normal,QString(),&ok);
    if(ok && !title.isEmpty()) {
        openPage(title);
    }
}

void EditWindow::pageSaved(const QString title) {
    if(title.startsWith(catFileName(""))) {
        QString catName=categoryName(title);
        QStringList pages=categories.value(catName);
        for(int i=0;i<pages.size();i++) {
            categoriesByPage[pages.at(i)].removeOne(catName);
        }
        QFile file(dir+"/"+title);
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        QString text;
        QTextStream in(&file);
        text=in.readAll();
        file.close();
        pages=getLinksList(text);
        pages.prepend(title);
        for(int i=0;i<pages.size();i++) {
            categoriesByPage[pages.at(i)].append(catName);
        }
        categories[catName]=pages;
    } else {
        categoriesByPage[title];
    }
    if(!listePages.contains(title)) {
        listePages.append(title);
        listePages.sort(Qt::CaseInsensitive);
    }
    for(int i=0;i<listePages.size();i++) {
        if(categoriesByPage[listePages.at(i)].size()) {
            noCatPages.removeOne(listePages.at(i));
        } else if(!noCatPages.contains(listePages.at(i))) {
            noCatPages.append(listePages.at(i));
        }
    }
    //noCatPages.removeDuplicates();
    noCatPages.sort(Qt::CaseInsensitive);
    dispTree();
}

void EditWindow::openWiki() {
    QString folder=QFileDialog::getExistingDirectory(this,"Choisir un dossier contenant un wiki");
    openWiki(folder);
}

void EditWindow::openWiki(const QString d) {
    if(!d.isEmpty()) {
        QSettings settings;
        settings.setValue("lastWikiLocation",d);

        dir=d;
        //modele->setRootPath(dir);

        QDir td(dir);
        td.setFilter(QDir::Files);
        listePages=td.entryList();
        listePages.sort(Qt::CaseInsensitive);
        noCatPages=listePages;
        //qDebug()<<modele->rootPath();
        //ui->listView->setRootIndex(modele->index(dir));

        categories.clear();
        for(int i=0;i<listePages.size();i++) {
            if(listePages.at(i).startsWith(catFileName(""))) {
                //categories.insert(listePages.at(i),*new QStringList());
                QFile file(dir+"/"+listePages.at(i));
                file.open(QIODevice::ReadOnly | QIODevice::Text);
                QString text;
                QTextStream in(&file);
                text=in.readAll();
                file.close();
                QStringList qsl=getLinksList(text);
                qsl.prepend(listePages.at(i));
                QString catName=categoryName(listePages.at(i));
                categories.insert(catName,qsl);
                for(int j=0;j<qsl.size();j++) {
                    noCatPages.removeOne(qsl.at(j));
                    categoriesByPage[qsl.at(j)].append(catName);
                }
            }
        }

        dispTree();

        //setWindowTitle("Siwim - "+modele->rootDirectory().dirName());
        setWindowTitle("Siwim - "+td.dirName());
        ui->actionNouveau->setEnabled(true);
        ui->listView->setEnabled(true);
        ui->actionExporter_en_HTML->setEnabled(true);
        ui->actionGenIndex->setEnabled(true);
        while(ui->tabWidget->count()) {
            closeTab(ui->tabWidget->currentIndex());
        }
    }
}

void EditWindow::dispTree() {
    ui->listView->clear();
    QStringList qsl=categories.keys();
    for(int i=0;i<qsl.size();i++) {
        QTreeWidgetItem *wi=new QTreeWidgetItem(QStringList(qsl.at(i)));
        for(int j=0;j<categories.value(qsl.at(i)).size();j++) {
            //QTreeWidgetItem *cwi=new QTreeWidgetItem(QStringList(categories.at(qsl.at(i)).at(j)));
            wi->addChild(new QTreeWidgetItem(QStringList(categories.value(qsl.at(i)).at(j))));
        }

        ui->listView->addTopLevelItem(wi);
    }
    for(int i=0;i<noCatPages.size();i++) {
        ui->listView->addTopLevelItem(new QTreeWidgetItem(QStringList(noCatPages.at(i))));
    }
}

void EditWindow::exportWiki() {
    QString folder=QFileDialog::getExistingDirectory(this,"Choisir un dossier pour exporter le wiki");
    if(!folder.isEmpty()) {
        if(folder==dir) {
            QMessageBox::StandardButton ans=QMessageBox::question(this,"Collision","Le dossier d'export que vous avez choisi est le même que celui d'édition et cela pourrait poser des problèmes par la suite.\nVoulez-vous changer de dossier d'export ?",QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel);
            if(ans==QMessageBox::Yes) {
                exportWiki();
            } else if(ans==QMessageBox::No) {
                exportWiki(folder);
            }
        } else {
            exportWiki(folder);
        }
    }
}

void EditWindow::exportWiki(const QString folder) {
    exportDialog ED(this);
    if(!ED.exec())
        return;
    bool dispCats=ED.dispCats();
    QString titleEnd=ED.titleWName()?" - "+ED.wName():"";
    QString nameHead="";
    if(ED.dispWName()) {
        if(ED.linkIndex()) {
            nameHead="<h1>[["+ED.indexPage()+"]](("+ED.wName()+"))</h1>";
        } else {
            nameHead="<h1>"+ED.wName()+"</h1>";
        }
    }
    QProgressDialog progress("Pages exportées :","",0,listePages.size(),this);
    progress.setValue(0);
    progress.setCancelButton(0);
    progress.setWindowTitle("Siwim - Export");
    /*progress.setMaximum(listePages.size());
    progress.setLabelText("Pages exportées :");*/
    QFile read;
    QFile write;
    QString text;
    QString html;
    for(int i=0;i<listePages.size();i++) {
        QString page=listePages.at(i);
        //qDebug()<<page;
        read.setFileName(dir+"/"+page);
        write.setFileName(folder+"/"+page+".html");

        read.open(QFile::ReadOnly | QIODevice::Text);
        write.open(QFile::WriteOnly | QIODevice::Text);

        QTextStream in(&read);
        text=in.readAll();
        read.close();

        QString header=nameHead;
        if(dispCats) {
            header.append("Catégories : ");
            for(int j=0;j<categoriesByPage[page].size();j++) {
                header.append(" [["+catFileName(categoriesByPage[page].at(j))+"]](("+categoriesByPage[page].at(j)+"))");
            }
            header.append("\n\n");
        }
        html=WTextZone::toHtml(text,".html");
        html.prepend(WTextZone::toHtml(header,".html"));
        html.prepend("<!DOCTYPE HTML><html><head><title>"+page+titleEnd+"</title><meta charset=\"utf-8\"/><style type=\"text/css\">p, li { white-space: pre-wrap; }</style></head><body style=\"font-family:'Sans'; font-size:10pt; font-weight:400; font-style:normal;\">");
        html.append("</body></html>");

        QTextStream out(&write);
        out<<html;
        out.flush();
        write.close();

        //QMessageBox::information(this,"Page !","Ui");
        progress.setValue(i+1);
    }
    //progress.setValue(progress.maximum());
    QMessageBox::information(this,"Exporté !","Exportation terminée !");
}

QStringList EditWindow::getLinksList(const QString text) {
    QStringList ret;
    for(int i=0;i<text.size()-1;i++) {
        if(text[i]=='[' && text[i+1]=='[') {
            int ni=text.indexOf("]]",i);
            if(ni!= -1) {
                ret<<text.mid(i+2,ni-i-2);
                i=ni;
            }
        }
    }
    ret.removeDuplicates();
    ret.sort(Qt::CaseInsensitive);
    return ret;
}
QString EditWindow::catFileName(QString cat) {
    return "category_"+cat;
}
QString EditWindow::categoryName(QString fileName) {
    return fileName.mid(QString("category_").size());
}

void EditWindow::generateIndex() {
    QString indexText="<h3>Categories</h3>\n";
    for(int i=0;i<categories.keys().size();i++) {
        indexText.append("[[");
        indexText.append(catFileName(categories.keys().at(i)));
        indexText.append("]]((");
        indexText.append(categories.keys().at(i));
        indexText.append("))\n");
    }
    indexText.append("<h3>Uncategorized pages</h3>\n");
    for(int i=0;i<noCatPages.size();i++) {
        indexText.append("[[");
        indexText.append(noCatPages.at(i));
        indexText.append("]]\n");
    }
    indexText.append("<h3>All pages</h3>\n");
    for(int i=0;i<listePages.size();i++) {
        indexText.append("[[");
        indexText.append(listePages.at(i));
        indexText.append("]]\n");
    }
    QFile file(dir+"/index");
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&file);
    out<<indexText;
    out.flush();
    file.close();
    openPage("index");
    closeTab(ui->tabWidget->currentIndex());
    openPage("index");
    pageSaved("index");
}

void EditWindow::about() {
    QMessageBox::about(this,"À propos de Siwim","Simiw (SImple WIki Manager) est un gestionnaire de wiki basique, écrit en C++ avec Qt 5.x. Il supporte une mise en forme basique du texte par le HTML.\
                                                Un wiki est un simple dossier rempli de fichiers textes brut ou HTML.");
}

void EditWindow::aboutQt() {
    QMessageBox::aboutQt(this);
}
