#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtWidgets>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QtXml>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QNetworkAccessManager * manager = new QNetworkAccessManager(this);
    QNetworkReply* reply = manager->get(QNetworkRequest(QUrl("https://api.openweathermap.org/data/2.5/weather?id=520555&units=metric&lang=ru&mode=xml&appid=6fb550ae11156e5729fa09c190689581")));

    QEventLoop loop;
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    QByteArray data =reply->readAll();
    QString str = QString::fromUtf8(data);

    reply->deleteLater();
    manager->deleteLater();

    qDebug().noquote() << str;

    //Разбираем полученные (XML) данные
    QXmlStreamReader reader (str); //класс для разбора XML

    while(!reader.atEnd())
    {
        if(reader.isStartElement())
        {
            QString name = reader.name().toString();

            //Город
            if(name == "city")
            {
                QXmlStreamAttributes attributes = reader.attributes();
                if(attributes.hasAttribute("name"))
                {
                    qDebug() << "Город: " << attributes.value("name").toString();
                }
            }

            //Страна
            if(name == "country")
            {
                QString text = reader.readElementText();
                qDebug() << "Страна: " << text;
            }

            //ВОСХОД-ЗАХОД СОЛНЦА
            if(name == "sun")
            {
                QXmlStreamAttributes attributes = reader.attributes();
                if(attributes.hasAttribute("set"))
                {
                    qDebug() << "Дата: " << attributes.value("set").toString().trimmed().left(10);
                    QString set = attributes.value("set").toString().trimmed().right(8);
                    QString GMT = QString::number(set.left(2).toInt()+3);
                    QString sunset = GMT+ attributes.value("set").toString().trimmed().right(6);
                    qDebug() << "Заход солнца: " << sunset;
                }

                if(attributes.hasAttribute("rise"))
                {
                    QString rise = attributes.value("rise").toString().trimmed().right(8);
                    QString GMT = QString::number(rise.left(2).toInt()+3);
                    QString sunrise = GMT+ attributes.value("rise").toString().trimmed().right(6);
                    qDebug() << "Восход солнца: " << sunrise;
                }
            }

            //ТЕМПЕРАТУРА ВОЗДУХА
            if(name == "temperature")
            {
                QXmlStreamAttributes attributes = reader.attributes();
                if(attributes.hasAttribute("max"))
                {
                    qDebug() << "Максимальная температура: " << attributes.value("max").toString();
                }

                if(attributes.hasAttribute("min"))
                {
                    qDebug() << "Минимальная температура: " << attributes.value("min").toString();
                }
            }

            if(name == "feels_like")
            {
                QXmlStreamAttributes attributes = reader.attributes();
                if(attributes.hasAttribute("value"))
                {
                    qDebug() << "Ощущается: " << attributes.value("value").toString();
                }
            }

            //ВЛАЖНОСТЬ ВОЗДУХА
            if(name == "humidity")
            {
                QXmlStreamAttributes attributes = reader.attributes();
                if(attributes.hasAttribute("value"))
                {
                    qDebug() << "Влажность: " << attributes.value("value").toString() + "%";
                }
            }

            //АТМОСФЕРНОЕ ДАВЛЕНИЕ
            if(name == "pressure")
            {
                QXmlStreamAttributes attributes = reader.attributes();
                if(attributes.hasAttribute("value"))
                {
                    qDebug() << "Атмосфреное давление: " << attributes.value("value").toString() + "гПа";
                }
            }

            //ОБЛАЧНОСТЬ
            if(name == "clouds")
            {
                QXmlStreamAttributes attributes = reader.attributes();
                if(attributes.hasAttribute("name"))
                {
                    qDebug() << "Облачность: " << attributes.value("name").toString();
                }
            }
        }

        if(reader.isEndElement())
        {
            QString name = reader.name().toString();
            if(name == "current")
            {
                qDebug() << "============================================";
            }
        }
        reader.readNext();
    }

    if(reader.hasError())
    {
        qDebug() << "Ошибка загрузки XML" << reader.errorString();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
