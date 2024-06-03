// Copyright (c) 2025 KMI Music, Inc.
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
// If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include "userdialog.h"
#include "mainwindow.h"

UserDialog::UserDialog(const QString& dialogText, const QStringList& buttonNames, int minWidth, int minHeight)
{

    qDebug() << "UserDialog created";

    mainWindow = qobject_cast<MainWindow*>(qApp->activeWindow());

    if (mainWindow) {
        mainWindow->disableWidget->raise();
    } else {
        qDebug() << "mainWindow is null!";
    }

    if (mainWindow != nullptr)
    {
        mainWindow->disableWidget->raise();
        mainWindow->disableWidget->show();
    }

//    // load stylesheets
//    QFile dialogStylesFile(":/stylesheets/appDialog_QuNexus_WIN.qss");
//    QString dialogStylesString = QLatin1String(dialogStylesFile.readAll());
//    QFile grayStyleFile("stylesheets/GrayButtonStyleSheet.qss");
//    QString grayStyleString = QLatin1String(grayStyleFile.readAll());


    this->setModal(true);
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setStyleSheet(mainWindow->dialogStylesString);

    // Constants for dialog dimensions and styling
    int DIALOG_MIN_WIDTH = minWidth;
    int DIALOG_MIN_HEIGHT = minHeight;
    int DIALOG_EDGE_PADDING = 40;  // Padding for the edges of the dialog
    int DIALOG_SPACING = 10;  // Vertical spacing between text and buttons
    int DIALOG_BUTTON_PADDING = 10;

    // Determine the button with the longest text
    QFontMetrics buttonFM = getFontMetricsFromStyleSheet(mainWindow->dialogStylesString, "QPushButton");
    int maxButtonTextWidth = 0;
    for (const QString& buttonText : buttonNames) {
        maxButtonTextWidth = std::max(maxButtonTextWidth, buttonFM.horizontalAdvance(buttonText));
    }

    // calculate widths
    int DIALOG_BUTT_H = buttonFM.height() + (DIALOG_BUTTON_PADDING);
    int DIALOG_BUTT_W = maxButtonTextWidth + (DIALOG_BUTTON_PADDING * 2);
    int TOTAL_BUTTON_WIDTH = buttonNames.size() * DIALOG_BUTT_W;
    int DIALOG_WIDTH = std::max(TOTAL_BUTTON_WIDTH + (2 * DIALOG_EDGE_PADDING), DIALOG_MIN_WIDTH);

    // setup text
    QLabel* text = new QLabel(dialogText, this, Qt::WindowFlags());
    text->setAlignment(Qt::AlignCenter);
    QFontMetrics textFM = getFontMetricsFromStyleSheet(mainWindow->dialogStylesString, "QLabel");

    int textWidth = textFM.horizontalAdvance(dialogText);
    int DIALOG_TEXT_W = DIALOG_WIDTH - 2 * DIALOG_EDGE_PADDING;
    bool wrapText = textWidth > TOTAL_BUTTON_WIDTH;

    // If text needs to be wrapped, calculate lines
    int lines = (wrapText ? ceil((double)textWidth / DIALOG_TEXT_W) : 1) + dialogText.count("\n");

    // calculate heights
    int DIALOG_TEXT_H = lines * (textFM.lineSpacing() + 3);

    int DIALOG_HEIGHT = std::max(((DIALOG_EDGE_PADDING * 2) + DIALOG_TEXT_H + DIALOG_SPACING + DIALOG_BUTT_H), DIALOG_MIN_HEIGHT);
    int DIALOG_BUTT_Y = DIALOG_HEIGHT - DIALOG_BUTT_H - DIALOG_SPACING - DIALOG_BUTTON_PADDING;


    this->setFixedSize(DIALOG_WIDTH, DIALOG_HEIGHT);

    // calculate window position
    int mwX = mainWindow->pos().x();
    int mwY = mainWindow->pos().y();
    int mwW = mainWindow->width();
    int mwH = mainWindow->height();
    int dialogX = mwX + ((mwW / 2) - (DIALOG_WIDTH / 2));
    int dialogY = mwY + ((mwH / 2) - (DIALOG_HEIGHT / 2));

    qDebug() << "parent x: " << mwX << " y: " << mwY << " dialogX: " << dialogX << "dialogY: " << dialogY;

    this->move(dialogX, dialogY);

    // setup text box
    text->setWordWrap(wrapText); // Enable word wrap if needed
    text->setMinimumSize(DIALOG_WIDTH - 2 * DIALOG_EDGE_PADDING, DIALOG_TEXT_H);
    text->setFixedSize(DIALOG_TEXT_W, DIALOG_TEXT_H);
    text->move(DIALOG_EDGE_PADDING, DIALOG_EDGE_PADDING);

    // Dynamically add buttons
    int buttonCount = buttonNames.size();
    int startingX = (DIALOG_WIDTH - TOTAL_BUTTON_WIDTH) / 2;

    for (int i = 0; i < buttonCount; ++i) {
        QPushButton* btn = new QPushButton(this);
        btn->setStyleSheet(mainWindow->grayStyleString);
        btn->setText(buttonNames[i]);
        btn->setGeometry(QRect(startingX + i * (DIALOG_BUTT_W + DIALOG_BUTTON_PADDING), DIALOG_BUTT_Y, DIALOG_BUTT_W, DIALOG_BUTT_H));
        connect(btn, &QPushButton::clicked, [this, i]() { setSelectedButtonIndex(i); });
    }
    this->raise();
}


void UserDialog::setSelectedButtonIndex(int index)
{
    selectedButtonIndex = index;
    this->done(index);
}

int UserDialog::exec()
{
    int result = QDialog::exec();
    Q_UNUSED(result);
    mainWindow->disableWidget->hide();
    return selectedButtonIndex;
}


QFontMetrics UserDialog::getFontMetricsFromStyleSheet(const QString &styleSheet, const QString &widgetType)
{
    QRegularExpression fontRegex(widgetType + R"(\s*{[^}]*font:\s*(\d+)px\s*"([^"]+))");
    QRegularExpressionMatch match = fontRegex.match(styleSheet);

    QFont font("Corbel", 14);  // Default font

    if (match.hasMatch()) {
        int fontSize = match.captured(1).toInt();
        QString fontFamily = match.captured(2);
        font = QFont(fontFamily, fontSize);
    }

    return QFontMetrics(font);
}
