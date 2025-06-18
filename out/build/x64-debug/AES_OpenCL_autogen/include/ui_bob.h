/********************************************************************************
** Form generated from reading UI file 'bob.ui'
**
** Created by: Qt User Interface Compiler version 6.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_BOB_H
#define UI_BOB_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Bob
{
public:
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout_2;
    QVBoxLayout *verticalLayout;
    QLineEdit *up_line;
    QPlainTextEdit *publickey_text;
    QPushButton *exchange_button;
    QPushButton *decrypt_button;
    QPushButton *showmessage_button;
    QLineEdit *middle_line;
    QPlainTextEdit *message_text;
    QLineEdit *log_line;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *Bob)
    {
        if (Bob->objectName().isEmpty())
            Bob->setObjectName("Bob");
        Bob->resize(521, 476);
        QFont font;
        font.setFamilies({QString::fromUtf8("Consolas")});
        font.setPointSize(12);
        Bob->setFont(font);
        centralwidget = new QWidget(Bob);
        centralwidget->setObjectName("centralwidget");
        verticalLayout_2 = new QVBoxLayout(centralwidget);
        verticalLayout_2->setObjectName("verticalLayout_2");
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName("verticalLayout");
        up_line = new QLineEdit(centralwidget);
        up_line->setObjectName("up_line");
        up_line->setEnabled(false);
        up_line->setFont(font);

        verticalLayout->addWidget(up_line);

        publickey_text = new QPlainTextEdit(centralwidget);
        publickey_text->setObjectName("publickey_text");
        publickey_text->setEnabled(false);

        verticalLayout->addWidget(publickey_text);

        exchange_button = new QPushButton(centralwidget);
        exchange_button->setObjectName("exchange_button");

        verticalLayout->addWidget(exchange_button);

        decrypt_button = new QPushButton(centralwidget);
        decrypt_button->setObjectName("decrypt_button");
        decrypt_button->setEnabled(false);

        verticalLayout->addWidget(decrypt_button);

        showmessage_button = new QPushButton(centralwidget);
        showmessage_button->setObjectName("showmessage_button");
        showmessage_button->setEnabled(false);

        verticalLayout->addWidget(showmessage_button);

        middle_line = new QLineEdit(centralwidget);
        middle_line->setObjectName("middle_line");
        middle_line->setEnabled(false);
        middle_line->setFont(font);

        verticalLayout->addWidget(middle_line);

        message_text = new QPlainTextEdit(centralwidget);
        message_text->setObjectName("message_text");
        message_text->setEnabled(false);
        message_text->setFont(font);

        verticalLayout->addWidget(message_text);

        log_line = new QLineEdit(centralwidget);
        log_line->setObjectName("log_line");
        log_line->setEnabled(false);
        log_line->setFont(font);

        verticalLayout->addWidget(log_line);


        verticalLayout_2->addLayout(verticalLayout);

        Bob->setCentralWidget(centralwidget);
        menubar = new QMenuBar(Bob);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 521, 21));
        Bob->setMenuBar(menubar);
        statusbar = new QStatusBar(Bob);
        statusbar->setObjectName("statusbar");
        Bob->setStatusBar(statusbar);

        retranslateUi(Bob);

        QMetaObject::connectSlotsByName(Bob);
    } // setupUi

    void retranslateUi(QMainWindow *Bob)
    {
        Bob->setWindowTitle(QCoreApplication::translate("Bob", "MainWindow", nullptr));
        up_line->setText(QCoreApplication::translate("Bob", "Bob : Public DSA key", nullptr));
        publickey_text->setPlainText(QString());
        exchange_button->setText(QCoreApplication::translate("Bob", "Exchange Public Keys", nullptr));
        decrypt_button->setText(QCoreApplication::translate("Bob", "Decrypt Message", nullptr));
        showmessage_button->setText(QCoreApplication::translate("Bob", "Show Message Decrypted", nullptr));
        middle_line->setText(QCoreApplication::translate("Bob", "You received : ", nullptr));
        log_line->setText(QCoreApplication::translate("Bob", "Log messages", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Bob: public Ui_Bob {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_BOB_H
