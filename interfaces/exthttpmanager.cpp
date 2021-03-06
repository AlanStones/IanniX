/*
    IanniX — a graphical real-time open-source sequencer for digital art
    Copyright (C) 2010-2012 — IanniX Association

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "exthttpmanager.h"

ExtHttpManager::ExtHttpManager(NxObjectFactoryInterface *_factory)
    : QTcpServer(_factory), ExtMessageManager(_factory) {
    //Initialization
    factory = _factory;

    //Html template
    QFile htmlTemplateFile("Tools" + QString(QDir::separator()) + "HTML Template.html");
    if(htmlTemplateFile.open(QFile::ReadOnly)) {
        htmlTemplate = htmlTemplateFile.readAll();
        htmlTemplateFile.close();
    }

    //Manager
    http = new QNetworkAccessManager(this);
    connect(http, SIGNAL(finished(QNetworkReply*)), SLOT(parse(QNetworkReply*)));
}

void ExtHttpManager::openPort(quint16 port) {
    //Initialization
    close();
    if(listen(QHostAddress::Any, port))
        emit(openPortStatus(true));
    else
        emit(openPortStatus(false));
}

void ExtHttpManager::send(const ExtMessage & message) {
    //Send request
    http->get(QNetworkRequest(message.getUrlMessage()));

    //Log in the OSC console
    factory->logOscSend(message.getVerboseMessage());
}


void ExtHttpManager::parse(QNetworkReply *reply) {
    QStringList commandItems = QString(reply->readAll()).split(COMMAND_END, QString::SkipEmptyParts);;
    foreach(const QString & command, commandItems) {
        //Fire events (log, message and script mapping)
        factory->logOscReceive(reply->url().toString() + " " + command);
        factory->execute(command);
        factory->onOscReceive("http", reply->url().host(), QString::number(reply->url().port()), reply->url().path(), command.split(" ", QString::SkipEmptyParts));
    }
}

void ExtHttpManager::parseService(const QUrl &url) {
    qDebug("=ASK=> %s", qPrintable(url.toString()));
}


//HTTP reception
void ExtHttpManager::incomingConnection(int socketDescriptor) {
    QTcpSocket *socket = new QTcpSocket(this);
    connect(socket, SIGNAL(readyRead()),    this, SLOT(readClient()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(discardClient()));
    socket->setSocketDescriptor(socketDescriptor);
}
void ExtHttpManager::readClient() {
    QTcpSocket *socket = (QTcpSocket*)sender();
    if(socket->canReadLine()) {
        QStringList tokens = QString(socket->readLine()).split(QRegExp("[ \r\n][ \r\n]*"));
        if((tokens.count() > 1) && (tokens.at(0) == "GET")) {
            QUrl url(tokens.at(1));

            QList<QString> commands;
            QPair<QString,qint8> picFormat;
            picFormat.first = "png";
            picFormat.second = -1;
            bool isPic = false, isSync = false;
            for(quint16 index = 0 ; index < url.queryItems().count() ; index++) {
                QString first = url.queryItems().at(index).first.toLower();
                if((first == "png") || (first == "jpg") || (first == "mjpg")) {
                    isPic = true;
                    picFormat.first  = first;
                    picFormat.second = url.queryItems().at(index).second.toInt();
                }
                else if(first == "sync") {
                    isSync = true;
                }
                else
                    commands.append(url.queryItems().at(index).second);
            }

            QTextStream os(socket);
            if((commands.count() > 0) && (!isPic) && (!isSync)) {
                os.setAutoDetectUnicode(true);
                os << "HTTP/1.0 200 Ok\r\n"
                      "Content-Type: text/plain; charset=\"utf-8\"\r\n"
                      "Access-Control-Allow-Origin: *\r\n"
                      "\r\n";

                QString response = "";
                foreach(const QString & command, commands) {
                    //Fire events (log, message and script mapping)
                    factory->logOscReceive(url.toString());
                    response += factory->execute(command).toString();
                    QString responseOsc = factory->onOscReceive("http", socket->peerAddress().toString(), QString::number(socket->peerPort()), url.path(), command.split(" ", QString::SkipEmptyParts));
                    if((responseOsc != "undefined") && (!responseOsc.isEmpty()))
                        response += "\n" + responseOsc;
                }
                os << response;
            }
            else if(isSync) {
                os.setAutoDetectUnicode(true);
                os << "HTTP/1.0 200 Ok\r\n"
                      "Content-Type: text/plain; charset=\"utf-8\"\r\n"
                      "Access-Control-Allow-Origin: *\r\n"
                      "\r\n";

                QApplication::postEvent(factory->getMainWindow(), new QMouseEvent(QEvent::MouseButtonPress,   QPoint(50, 50), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier));
                QApplication::postEvent(factory->getMainWindow(), new QMouseEvent(QEvent::MouseButtonRelease, QPoint(50, 50), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier));

                os << factory->serialize();
            }
            else if(isPic) {
                if((picFormat.first == "png") || (picFormat.first == "jpg")) {
                    os << "HTTP/1.0 200 Ok\r\n"
                          "Content-Type: image/jpeg\r\n"
                          "Access-Control-Allow-Origin: *\r\n"
                          "\r\n";

                    QByteArray byteArray;
                    QBuffer buffer(&byteArray);
                    if(picFormat.second == 0)
                        picFormat.second = -1;
                    factory->takeScreenshot().save(&buffer, qPrintable(picFormat.first), picFormat.second);
                    os.flush();
                    socket->write(byteArray);
                }
                else if(picFormat.first == "mjpg") {
                }
            }
            else {
                os.setAutoDetectUnicode(true);
                os << "HTTP/1.0 200 Ok\r\n"
                      "Content-Type: text/html; charset=\"utf-8\"\r\n"
                      "Access-Control-Allow-Origin: *\r\n"
                      "\r\n";

                os << htmlTemplate;
            }

            socket->close();
            if(socket->state() == QTcpSocket::UnconnectedState)
                delete socket;
        }
    }
}
void ExtHttpManager::discardClient() {
    QTcpSocket* socket = (QTcpSocket*)sender();
    socket->deleteLater();
}
