/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.6.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QTextEdit *messagesTextEdit;
    QLineEdit *messageLineEdit;
    QPushButton *sendButton;
    QLineEdit *friendIdLineEdit;
    QPushButton *addFriendButton;
    QPushButton *getFriendIdButton;
    QPushButton *getUserIdButton;
    QListWidget *friendsListWidget;
    QLineEdit *versionLineEdit;
    QLineEdit *errorLineEdit;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(800, 600);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        messagesTextEdit = new QTextEdit(centralwidget);
        messagesTextEdit->setObjectName("messagesTextEdit");
        messagesTextEdit->setGeometry(QRect(20, 20, 560, 280));
        messageLineEdit = new QLineEdit(centralwidget);
        messageLineEdit->setObjectName("messageLineEdit");
        messageLineEdit->setGeometry(QRect(20, 320, 440, 40));
        sendButton = new QPushButton(centralwidget);
        sendButton->setObjectName("sendButton");
        sendButton->setGeometry(QRect(480, 320, 100, 40));
        friendIdLineEdit = new QLineEdit(centralwidget);
        friendIdLineEdit->setObjectName("friendIdLineEdit");
        friendIdLineEdit->setGeometry(QRect(600, 20, 180, 30));
        addFriendButton = new QPushButton(centralwidget);
        addFriendButton->setObjectName("addFriendButton");
        addFriendButton->setGeometry(QRect(600, 60, 180, 30));
        getFriendIdButton = new QPushButton(centralwidget);
        getFriendIdButton->setObjectName("getFriendIdButton");
        getFriendIdButton->setGeometry(QRect(600, 100, 180, 30));
        getUserIdButton = new QPushButton(centralwidget);
        getUserIdButton->setObjectName("getUserIdButton");
        getUserIdButton->setGeometry(QRect(600, 140, 180, 30));
        friendsListWidget = new QListWidget(centralwidget);
        friendsListWidget->setObjectName("friendsListWidget");
        friendsListWidget->setGeometry(QRect(600, 180, 180, 200));
        versionLineEdit = new QLineEdit(centralwidget);
        versionLineEdit->setObjectName("versionLineEdit");
        versionLineEdit->setGeometry(QRect(20, 370, 200, 30));
        versionLineEdit->setReadOnly(true);
        errorLineEdit = new QLineEdit(centralwidget);
        errorLineEdit->setObjectName("errorLineEdit");
        errorLineEdit->setGeometry(QRect(240, 370, 340, 30));
        errorLineEdit->setReadOnly(true);
        MainWindow->setCentralWidget(centralwidget);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        sendButton->setText(QCoreApplication::translate("MainWindow", "Send", nullptr));
        friendIdLineEdit->setPlaceholderText(QCoreApplication::translate("MainWindow", "Friend ID", nullptr));
        addFriendButton->setText(QCoreApplication::translate("MainWindow", "Add Friend", nullptr));
        getFriendIdButton->setText(QCoreApplication::translate("MainWindow", "Get Friend ID", nullptr));
        getUserIdButton->setText(QCoreApplication::translate("MainWindow", "Get User ID", nullptr));
        (void)MainWindow;
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
