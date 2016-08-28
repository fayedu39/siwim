#include <QApplication>
#include <QtWidgets>
#include <QTranslator>
#include <QLocale>
#include <QLibraryInfo>

#include <QtDebug>

#include "editwindow.h"
#include "wtextzone.h"

int main(int argc, char *argv[])

{

    QApplication app(argc, argv);


    QString locale = QLocale::system().name().section('_', 0, 0);
    QTranslator translator;
    translator.load(QString("qt_") + locale, QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    app.installTranslator(&translator);

    app.setOrganizationName("LuxEra");
    app.setApplicationName("Siwim");

    EditWindow Fenestre;


    Fenestre.show();



    return app.exec();

}
