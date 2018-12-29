/*
 * iSL (Subsystem for Linux) for iOS & Android
 *
 * Copyright (C) 2018 Björn Rennfanz (bjoern@fam-rennfanz.de)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include <signal.h>
#include <QDebug>

#include "islappui.h"

int main(int argc, char *argv[])
{
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);

    CiSLAppUi iSLAppUi;
    QQmlApplicationEngine qmlEngine;

    // From QML we have access to CiSLAppUi as iSLApp
    QQmlContext* pQmlContext = qmlEngine.rootContext();
    pQmlContext->setContextProperty("iSLApp", &iSLAppUi);

    // Start QML
    qmlEngine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));
    return app.exec();
}
