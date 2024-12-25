#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QListWidget>
#include <QLabel>
#include <QFile>
#include <QTimer>
#include "tox.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

    void onConnectionStatusChanged(Tox* tox, TOX_CONNECTION connection_status);
    void friendRequestCallback(Tox* tox, const uint8_t* public_key, const uint8_t* message, size_t length);
    void friendMessageCallback(Tox* tox, uint32_t friend_number, TOX_MESSAGE_TYPE type, const uint8_t* message, size_t length);

private slots:
    void on_sendButton_clicked();
    void on_addFriendButton_clicked();
    void on_getFriendIdButton_clicked();
    void on_getUserIdButton_clicked();
    void on_connectButton_clicked();
    void on_initializeButton_clicked();

private:
    Ui::MainWindow* ui;
    Tox* tox;
    QTimer* toxTimer;
    QString username;
    QString password;

    void initTox(bool loadSavedData = true);
    void updateFriendList();
    void logDebugInfo(const QString& message);
    void updateConnectionStatus(const QString& status);
    void saveToxData();
    void loadToxData();
    void saveLoginData();
    bool loadLoginData();
    void displayToxVersion();

    static void onConnectionStatusChangedStatic(Tox* tox, TOX_CONNECTION connection_status, void* user_data);
    static void friendRequestCallbackStatic(Tox* tox, const uint8_t* public_key, const uint8_t* message, size_t length, void* user_data);
    static void friendMessageCallbackStatic(Tox* tox, uint32_t friend_number, TOX_MESSAGE_TYPE type, const uint8_t* message, size_t length, void* user_data);
};

#endif // MAINWINDOW_H