#ifndef CONFIGURATOR_H
#define CONFIGURATOR_H

#include <QtWidgets/QMainWindow>
#include "ui_configurator.h"
#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QFile>
#include <QTextStream>

class Configurator : public QWidget {
    Q_OBJECT

public:
    Configurator(QWidget* parent = nullptr);

private slots:
    void saveConfig();

private:
    QLineEdit* ldapInput;
    QLineEdit* powerdnsInput;
    QLineEdit* dhcpInput;
    QLineEdit* adminInput;

    QLineEdit* ansibleUserInput;
    QLineEdit* ansiblePassInput;
    QLineEdit* becomePassInput;

    bool validateIP(const QString& ip);
};

#endif // CONFIGURATOR_H