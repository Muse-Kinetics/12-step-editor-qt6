// Copyright (c) 2025 KMI Music, Inc.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef USERDIALOG_H
#define USERDIALOG_H

#include <QDialog>
#include <QDialogButtonBox>
#include <QLabel>
#include <QPushButton>
#include <QFont>
#include <QFontMetrics>
#include <QRegularExpression>

class MainWindow;  // Forward declaration

class UserDialog : public QDialog {
    Q_OBJECT

public:
    explicit UserDialog(const QString& dialogText, const QStringList& buttonNames, int minWidth = 500, int minHeight = 300);

    int selectedButtonIndex;

    // functions
    int exec() override;
    void setSelectedButtonIndex(int index);
    QFontMetrics getFontMetricsFromStyleSheet(const QString &styleSheet, const QString &widgetType);

private:
    // Constants for dialog dimensions and styling
    MainWindow *mainWindow;

};

#endif // USERDIALOG_H
