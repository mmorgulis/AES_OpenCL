/********************************************************************************
** Form generated from reading UI file 'alice.ui'
**
** Created by: Qt User Interface Compiler version 6.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ALICE_H
#define UI_ALICE_H

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

class Ui_Alice
{
public:
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout_2;
    QVBoxLayout *verticalLayout;
    QLineEdit *up_line;
    QPlainTextEdit *publickey_text;
    QPushButton *exchange_button;
    QPushButton *sendmessage_button;
    QLineEdit *middle_line;
    QPlainTextEdit *message_text;
    QLineEdit *log_line;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *Alice)
    {
        if (Alice->objectName().isEmpty())
            Alice->setObjectName("Alice");
        Alice->resize(490, 376);
        centralwidget = new QWidget(Alice);
        centralwidget->setObjectName("centralwidget");
        verticalLayout_2 = new QVBoxLayout(centralwidget);
        verticalLayout_2->setObjectName("verticalLayout_2");
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName("verticalLayout");
        up_line = new QLineEdit(centralwidget);
        up_line->setObjectName("up_line");
        up_line->setEnabled(false);

        verticalLayout->addWidget(up_line);

        publickey_text = new QPlainTextEdit(centralwidget);
        publickey_text->setObjectName("publickey_text");
        publickey_text->setEnabled(false);

        verticalLayout->addWidget(publickey_text);

        exchange_button = new QPushButton(centralwidget);
        exchange_button->setObjectName("exchange_button");

        verticalLayout->addWidget(exchange_button);

        sendmessage_button = new QPushButton(centralwidget);
        sendmessage_button->setObjectName("sendmessage_button");
        sendmessage_button->setEnabled(false);

        verticalLayout->addWidget(sendmessage_button);

        middle_line = new QLineEdit(centralwidget);
        middle_line->setObjectName("middle_line");
        middle_line->setEnabled(false);

        verticalLayout->addWidget(middle_line);

        message_text = new QPlainTextEdit(centralwidget);
        message_text->setObjectName("message_text");

        verticalLayout->addWidget(message_text);

        log_line = new QLineEdit(centralwidget);
        log_line->setObjectName("log_line");
        log_line->setEnabled(false);

        verticalLayout->addWidget(log_line);


        verticalLayout_2->addLayout(verticalLayout);

        Alice->setCentralWidget(centralwidget);
        menubar = new QMenuBar(Alice);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 490, 21));
        Alice->setMenuBar(menubar);
        statusbar = new QStatusBar(Alice);
        statusbar->setObjectName("statusbar");
        Alice->setStatusBar(statusbar);

        retranslateUi(Alice);

        QMetaObject::connectSlotsByName(Alice);
    } // setupUi

    void retranslateUi(QMainWindow *Alice)
    {
        Alice->setProperty("alice", QVariant(QCoreApplication::translate("Alice", "Alice", nullptr)));
        up_line->setText(QCoreApplication::translate("Alice", "Alice : Public Key", nullptr));
        publickey_text->setPlainText(QString());
        exchange_button->setText(QCoreApplication::translate("Alice", "Exchange Public Keys ", nullptr));
        sendmessage_button->setText(QCoreApplication::translate("Alice", "Send Message", nullptr));
        middle_line->setText(QCoreApplication::translate("Alice", "Insert message down here", nullptr));
        message_text->setPlainText(QString());
        log_line->setText(QCoreApplication::translate("Alice", "Log messages", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Alice: public Ui_Alice {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ALICE_H
