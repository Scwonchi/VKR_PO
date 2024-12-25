/********************************************************************************
** Form generated from reading UI file 'configurator.ui'
**
** Created by: Qt User Interface Compiler version 6.6.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CONFIGURATOR_H
#define UI_CONFIGURATOR_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ConfiguratorClass
{
public:
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QWidget *centralWidget;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *ConfiguratorClass)
    {
        if (ConfiguratorClass->objectName().isEmpty())
            ConfiguratorClass->setObjectName("ConfiguratorClass");
        ConfiguratorClass->resize(600, 400);
        menuBar = new QMenuBar(ConfiguratorClass);
        menuBar->setObjectName("menuBar");
        ConfiguratorClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(ConfiguratorClass);
        mainToolBar->setObjectName("mainToolBar");
        ConfiguratorClass->addToolBar(mainToolBar);
        centralWidget = new QWidget(ConfiguratorClass);
        centralWidget->setObjectName("centralWidget");
        ConfiguratorClass->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(ConfiguratorClass);
        statusBar->setObjectName("statusBar");
        ConfiguratorClass->setStatusBar(statusBar);

        retranslateUi(ConfiguratorClass);

        QMetaObject::connectSlotsByName(ConfiguratorClass);
    } // setupUi

    void retranslateUi(QMainWindow *ConfiguratorClass)
    {
        ConfiguratorClass->setWindowTitle(QCoreApplication::translate("ConfiguratorClass", "Configurator", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ConfiguratorClass: public Ui_ConfiguratorClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CONFIGURATOR_H
