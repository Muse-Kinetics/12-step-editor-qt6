// Copyright (c) 2025 KMI Music, Inc.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "mainwindow.h"
#include <QApplication>

#define JUCE_GLOBAL_MODULE_SETTINGS_INCLUDED 1

int main(int argc, char *argv[])
{

    // set highdpi scale factor rounding
//    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
#ifdef Q_OS_WIN
//    // up-scale the app for windows
//    QApplication* temp = new QApplication(argc, argv);
//    QScreen *screen = QGuiApplication::primaryScreen();
//    QRect  screenGeometry = screen->availableGeometry();
//    double width = screenGeometry.width();
//    // assumes that the default desktop resolution is 720p (scale of 1)
//    int minWidth = 1280;
//    delete temp;

    //double scale = width / minWidth;
    //qDebug() << "scale: " << scale;
    double scale = 1.35;
    std::string scaleAsString = std::to_string(scale);
    QByteArray scaleAsQByteArray(scaleAsString.c_str(), (int)scaleAsString.length());
    qputenv("QT_SCALE_FACTOR", scaleAsQByteArray);
    qputenv("QT_SCALE_FACTOR_ROUNDING_POLICY", "Round");
#endif

    QApplication a(argc, argv);


    // this sets the correct OpenGL api depending on which version of Qt we are using
#if QT_VERSION >= 0x060000
    //QQuickWindow::setGraphicsApi(QSGRendererInterface::OpenGL);
#else
    //QSGRendererInterface::GraphicsApi(QSGRendererInterface::OpenGL);
#endif

    MainWindow w;
    w.show();

    return a.exec();
}
