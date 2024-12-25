#include "configurator.h"
#include <QMessageBox>
#include <QRegularExpression>

Configurator::Configurator(QWidget* parent) : QWidget(parent) {
    auto* layout = new QVBoxLayout(this);

    QLabel* ldapLabel = new QLabel("LDAP Server IP:");
    ldapInput = new QLineEdit();
    layout->addWidget(ldapLabel);
    layout->addWidget(ldapInput);

    QLabel* powerdnsLabel = new QLabel("PowerDNS Server IP:");
    powerdnsInput = new QLineEdit();
    layout->addWidget(powerdnsLabel);
    layout->addWidget(powerdnsInput);

    QLabel* dhcpLabel = new QLabel("DHCP Server IP:");
    dhcpInput = new QLineEdit();
    layout->addWidget(dhcpLabel);
    layout->addWidget(dhcpInput);

    QLabel* adminLabel = new QLabel("Administration Station IP:");
    adminInput = new QLineEdit();
    layout->addWidget(adminLabel);
    layout->addWidget(adminInput);

    QLabel* userLabel = new QLabel("User:");
    ansibleUserInput = new QLineEdit();
    layout->addWidget(userLabel);
    layout->addWidget(ansibleUserInput);

    QLabel* passLabel = new QLabel("SSH Password:");
    ansiblePassInput = new QLineEdit();
    ansiblePassInput->setEchoMode(QLineEdit::Password);
    layout->addWidget(passLabel);
    layout->addWidget(ansiblePassInput);

    QLabel* becomePassLabel = new QLabel("Become Password:");
    becomePassInput = new QLineEdit();
    becomePassInput->setEchoMode(QLineEdit::Password);
    layout->addWidget(becomePassLabel);
    layout->addWidget(becomePassInput);

    QPushButton* saveButton = new QPushButton("Save Configuration");
    layout->addWidget(saveButton);

    connect(saveButton, &QPushButton::clicked, this, &Configurator::saveConfig);
}

bool Configurator::validateIP(const QString& ip) {
    QRegularExpression ipRegex(R"((\d{1,3}\.){3}\d{1,3})");
    if (!ipRegex.match(ip).hasMatch())
        return false;

    QStringList parts = ip.split('.');
    for (const QString& part : parts) {
        int value = part.toInt();
        if (value < 0 || value > 255)
            return false;
    }
    return true;
}

void Configurator::saveConfig() {
    QString ldapIP = ldapInput->text();
    QString powerdnsIP = powerdnsInput->text();
    QString dhcpIP = dhcpInput->text();
    QString adminIP = adminInput->text();
    QString ansibleUser = ansibleUserInput->text();
    QString ansiblePass = ansiblePassInput->text();
    QString becomePass = becomePassInput->text();

    if (!validateIP(ldapIP) || !validateIP(powerdnsIP) || !validateIP(dhcpIP) || !validateIP(adminIP)) {
        QMessageBox::warning(this, "Validation Error", "One or more IP addresses are invalid.");
        return;
    }

    QFile file("hosts.yml");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Error", "Failed to save the configuration.");
        return;
    }

    QTextStream out(&file);
    out << "---\n";
    out << "all:\n";
    out << "  children:\n";

    auto writeBlock = [&](const QString& role, const QString& hostAlias, const QString& ip) {
        out << "    " << role << ":\n";
        out << "      hosts:\n";
        out << "        " << hostAlias << ":\n";
        out << "          ansible_host: " << ip << "\n";
        out << "          ansible_user: " << ansibleUser << "\n";
        out << "\n";
        out << "      vars:\n";
        out << "        ansible_ssh_pass: " << ansiblePass << "\n";
        out << "        ansible_become_pass: " << becomePass << "\n";
        out << "\n";
    };

    writeBlock("ldap", "ldap1", ldapIP);
    writeBlock("dns", "dns1", powerdnsIP);
    writeBlock("dhcp", "dhcph1", dhcpIP);
    writeBlock("adminstaion", "adminstation1", adminIP);

    file.close();

    QMessageBox::information(this, "Success", "Configuration saved successfully!");
}
