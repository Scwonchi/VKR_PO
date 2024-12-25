#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <sodium.h>
#include <QApplication>
#include <QMainWindow>
#include <QLineEdit>
#include <QString>
#include <tox.h>
#include <winsock.h>
#include <Windows.h>
#include <iphlpapi.h>
#include <QMessageBox>
#include <cstring>
#include <fstream>
#include <sstream>
#include <vector>
#include <QByteArray>
#include <QTimer>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), tox(nullptr), toxTimer(nullptr) {
    ui->setupUi(this);
    logDebugInfo("Starting application...");
    if (loadLoginData()) {
        loadToxData(); // Load saved Tox data on startup
    }
    else {
        logDebugInfo("No saved login data found. Please enter your username and password.");
    }
}

MainWindow::~MainWindow() {
    if (tox) {
        saveToxData(); // Save Tox data on exit
        tox_kill(tox);
    }
    saveLoginData(); // Save login data on exit
    delete ui;
}

void MainWindow::initTox(bool loadSavedData) {
    struct Tox_Options options;
    tox_options_default(&options);

    if (loadSavedData) {
        QFile file("data.tox");
        if (file.exists() && file.open(QIODevice::ReadOnly)) {
            QByteArray data = file.readAll();
            file.close();
            options.savedata_type = TOX_SAVEDATA_TYPE_TOX_SAVE;
            options.savedata_data = reinterpret_cast<uint8_t*>(data.data());
            options.savedata_length = data.size();
            logDebugInfo("Loaded saved Tox data");
        }
        else {
            options.savedata_type = TOX_SAVEDATA_TYPE_NONE;
            logDebugInfo("No saved Tox data found, starting fresh");
        }
    }
    else {
        options.savedata_type = TOX_SAVEDATA_TYPE_NONE;
        logDebugInfo("Starting Tox without loading saved data");
    }

    TOX_ERR_NEW tox_err_new;
    tox = tox_new(&options, &tox_err_new);
    if (!tox) {
        QString err_msg;
        switch (tox_err_new) {
        case TOX_ERR_NEW_OK: err_msg = "OK"; break;
        case TOX_ERR_NEW_NULL: err_msg = "NULL"; break;
        case TOX_ERR_NEW_MALLOC: err_msg = "MALLOC"; break;
        case TOX_ERR_NEW_PORT_ALLOC: err_msg = "PORT_ALLOC"; break;
        case TOX_ERR_NEW_PROXY_BAD_TYPE: err_msg = "PROXY_BAD_TYPE"; break;
        case TOX_ERR_NEW_PROXY_BAD_HOST: err_msg = "PROXY_BAD_HOST"; break;
        case TOX_ERR_NEW_PROXY_BAD_PORT: err_msg = "PROXY_BAD_PORT"; break;
        case TOX_ERR_NEW_PROXY_NOT_FOUND: err_msg = "PROXY_NOT_FOUND"; break;
        case TOX_ERR_NEW_LOAD_ENCRYPTED: err_msg = "LOAD_ENCRYPTED"; break;
        case TOX_ERR_NEW_LOAD_BAD_FORMAT: err_msg = "LOAD_BAD_FORMAT"; break;
        default: err_msg = "UNKNOWN_ERROR"; break;
        }
        logDebugInfo("Failed to initialize Tox: " + err_msg);
        updateConnectionStatus("Failed to initialize Tox: " + err_msg);
        return;
    }

    // Register callbacks
    tox_callback_self_connection_status(tox, onConnectionStatusChangedStatic);
    logDebugInfo("Registered onConnectionStatusChanged callback");

    tox_callback_friend_request(tox, friendRequestCallbackStatic);
    logDebugInfo("Registered friendRequestCallback");

    tox_callback_friend_message(tox, friendMessageCallbackStatic);
    logDebugInfo("Registered friendMessageCallback");

    // Добавим указанную bootstrap-ноду для подключения
    const char* address_ipv4 = "188.225.9.167";
    const char* address_ipv6 = "209:dead:ded:4991:49f3:b6c0:9869:3019";
    uint16_t port = 33445;
    const char* public_key_str = "1911341A83E02503AB1FD6561BD64AF3A9D6C3F12B5FBB656976B2E678644A67";
    uint8_t public_key[TOX_PUBLIC_KEY_SIZE];
    if (sodium_hex2bin(public_key, TOX_PUBLIC_KEY_SIZE, public_key_str, strlen(public_key_str), nullptr, nullptr, nullptr) != 0) {
        logDebugInfo("Failed to convert public key from hex");
        updateConnectionStatus("Failed to convert public key from hex");
        return;
    }

    if (!tox_bootstrap(tox, address_ipv4, port, public_key, nullptr)) {
        logDebugInfo("Failed to bootstrap with IPv4 node");
        updateConnectionStatus("Failed to bootstrap with IPv4 node");
    }
    else {
        logDebugInfo("Bootstrapped with IPv4 node");
    }

    if (!tox_add_tcp_relay(tox, address_ipv4, port, public_key, nullptr)) {
        logDebugInfo("Failed to add IPv4 TCP relay");
        updateConnectionStatus("Failed to add IPv4 TCP relay");
    }
    else {
        logDebugInfo("Added IPv4 TCP relay");
    }

    if (!tox_bootstrap(tox, address_ipv6, port, public_key, nullptr)) {
        logDebugInfo("Failed to bootstrap with IPv6 node");
        updateConnectionStatus("Failed to bootstrap with IPv6 node");
    }
    else {
        logDebugInfo("Bootstrapped with IPv6 node");
    }

    if (!tox_add_tcp_relay(tox, address_ipv6, port, public_key, nullptr)) {
        logDebugInfo("Failed to add IPv6 TCP relay");
        updateConnectionStatus("Failed to add IPv6 TCP relay");
    }
    else {
        logDebugInfo("Added IPv6 TCP relay");
    }

    logDebugInfo("Tox initialized successfully");
    updateConnectionStatus("Tox initialized successfully");
    updateFriendList();

    // Запуск цикла обработки событий Tox
    toxTimer = new QTimer(this);
    connect(toxTimer, &QTimer::timeout, [this]() {
        if (tox) {
            tox_iterate(tox, nullptr);
        }
        });
    toxTimer->start(1000); // Убедимся, что таймер работает каждые 1000 мс

    // Отображение версии Tox
    displayToxVersion();
}

void MainWindow::updateFriendList() {
    if (!tox) return;

    ui->friendsListWidget->clear();
    uint32_t friendCount = tox_self_get_friend_list_size(tox);
    uint32_t* friends = new uint32_t[friendCount];
    tox_self_get_friend_list(tox, friends);

    logDebugInfo("Updating friend list");

    for (uint32_t i = 0; i < friendCount; ++i) {
        uint32_t friendId = friends[i];
        QString friendName = "Friend " + QString::number(friendId);
        QListWidgetItem* item = new QListWidgetItem(friendName);
        item->setData(Qt::UserRole, friendId);
        ui->friendsListWidget->addItem(item);
        logDebugInfo("Friend added: " + friendName);
    }

    delete[] friends;
}

void MainWindow::on_sendButton_clicked() {
    if (!tox) return;

    QString message = ui->messageLineEdit->text();
    QListWidgetItem* currentItem = ui->friendsListWidget->currentItem();
    if (currentItem) {
        uint32_t friendId = currentItem->data(Qt::UserRole).toUInt();
        QByteArray msg = message.toUtf8();
        TOX_ERR_FRIEND_SEND_MESSAGE err;
        tox_friend_send_message(tox, friendId, TOX_MESSAGE_TYPE_NORMAL, (const uint8_t*)msg.data(), msg.size(), &err);
        if (err == TOX_ERR_FRIEND_SEND_MESSAGE_OK) {
            ui->messagesTextEdit->append("You: " + message);
            logDebugInfo("Message sent to friend " + QString::number(friendId) + ": " + message);
        }
        else {
            logDebugInfo("Failed to send message: " + QString::number(err));
        }
        ui->messageLineEdit->clear();
    }
}

void MainWindow::on_addFriendButton_clicked() {
    if (!tox) return;

    QString friendIdStr = ui->friendIdLineEdit->text();
    QByteArray friendIdBytes = QByteArray::fromHex(friendIdStr.toUtf8());
    const uint8_t* friendId = (const uint8_t*)friendIdBytes.data();
    TOX_ERR_FRIEND_ADD err;
    const char* hello_message = "Hello!";
    uint32_t friendNumber = tox_friend_add(tox, friendId, (const uint8_t*)hello_message, strlen(hello_message), &err);
    if (err != TOX_ERR_FRIEND_ADD_OK) {
        QString errMsg;
        switch (err) {
        case TOX_ERR_FRIEND_ADD_NULL:
            errMsg = "One of the arguments to the function was NULL when it was not expected.";
            break;
        case TOX_ERR_FRIEND_ADD_TOO_LONG:
            errMsg = "The length of the message exceeded TOX_MAX_FRIEND_REQUEST_LENGTH.";
            break;
        case TOX_ERR_FRIEND_ADD_NO_MESSAGE:
            errMsg = "The message was empty.";
            break;
        case TOX_ERR_FRIEND_ADD_OWN_KEY:
            errMsg = "The friend address belonged to the sending client.";
            break;
        case TOX_ERR_FRIEND_ADD_ALREADY_SENT:
            errMsg = "A request has already been sent, but has not yet been answered.";
            break;
        case TOX_ERR_FRIEND_ADD_BAD_CHECKSUM:
            errMsg = "The friend address checksum failed.";
            break;
        case TOX_ERR_FRIEND_ADD_SET_NEW_NOSPAM:
            errMsg = "The friend address was generated with a different nospam.";
            break;
        case TOX_ERR_FRIEND_ADD_MALLOC:
            errMsg = "A memory allocation failed.";
            break;
        default:
            errMsg = "Unknown error.";
            break;
        }
        logDebugInfo("Failed to add friend: " + errMsg);
        updateConnectionStatus("Failed to add friend: " + errMsg);
    }
    else {
        updateFriendList();
        logDebugInfo("Friend request sent to: " + friendIdStr);
        updateConnectionStatus("Friend request sent to: " + friendIdStr);
    }
}

void MainWindow::on_getFriendIdButton_clicked() {
    if (!tox) return;

    QListWidgetItem* currentItem = ui->friendsListWidget->currentItem();
    if (currentItem) {
        uint32_t friendId = currentItem->data(Qt::UserRole).toUInt();
        uint8_t friendAddress[TOX_PUBLIC_KEY_SIZE];
        TOX_ERR_FRIEND_GET_PUBLIC_KEY err;
        tox_friend_get_public_key(tox, friendId, friendAddress, &err);
        if (err == TOX_ERR_FRIEND_GET_PUBLIC_KEY_OK) {
            QString friendIdStr = QByteArray((char*)friendAddress, TOX_PUBLIC_KEY_SIZE).toHex();
            ui->friendIdLineEdit->setText(friendIdStr);
            logDebugInfo("Friend ID retrieved: " + friendIdStr);
        }
        else {
            logDebugInfo("Failed to retrieve friend ID, error: " + QString::number(err));
        }
    }
}

void MainWindow::on_getUserIdButton_clicked() {
    if (!tox) return;

    uint8_t selfAddress[TOX_ADDRESS_SIZE];
    tox_self_get_address(tox, selfAddress);
    QString selfIdStr = QByteArray((char*)selfAddress, TOX_ADDRESS_SIZE).toHex();
    ui->friendIdLineEdit->setText(selfIdStr);
    logDebugInfo("User ID retrieved: " + selfIdStr);
}

void MainWindow::on_connectButton_clicked() {
    initTox();
    if (tox) {
        ui->connectionStatusLabel->setText("Connected");
        logDebugInfo("Connected to Tox network");
        updateConnectionStatus("Connected to Tox network");
    }
    else {
        ui->connectionStatusLabel->setText("Not connected");
        logDebugInfo("Failed to connect to Tox network");
        updateConnectionStatus("Failed to connect to Tox network");
    }
}

void MainWindow::on_initializeButton_clicked() {
    username = ui->usernameLineEdit->text();
    password = ui->passwordLineEdit->text();
    // Сохранение данных для первого входа
    logDebugInfo("Initializing with username: " + username + " and password: " + password);
    saveLoginData();
    initTox(false); // Инициализируем без загрузки сохраненных данных
}

void MainWindow::logDebugInfo(const QString& message) {
    ui->messagesTextEdit->append("[DEBUG] " + message);
}

void MainWindow::updateConnectionStatus(const QString& status) {
    ui->errorLineEdit->setText(status);
}

void MainWindow::saveToxData() {
    if (tox) {
        size_t size = tox_get_savedata_size(tox);
        uint8_t* data = new uint8_t[size];
        tox_get_savedata(tox, data);

        QFile file("data.tox");
        if (file.open(QIODevice::WriteOnly)) {
            file.write(reinterpret_cast<char*>(data), size);
            logDebugInfo("Tox data saved");
        }
        else {
            logDebugInfo("Failed to save Tox data");
        }
        file.close();
        delete[] data;
    }
}

void MainWindow::loadToxData() {
    QFile file("data.tox");
    if (file.exists() && file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.readAll();
        file.close();

        struct Tox_Options options;
        tox_options_default(&options);
        options.savedata_type = TOX_SAVEDATA_TYPE_TOX_SAVE;
        options.savedata_data = reinterpret_cast<uint8_t*>(data.data());
        options.savedata_length = data.size();

        TOX_ERR_NEW tox_err_new;
        tox = tox_new(&options, &tox_err_new);
        if (!tox) {
            QString err_msg;
            switch (tox_err_new) {
            case TOX_ERR_NEW_OK: err_msg = "OK"; break;
            case TOX_ERR_NEW_NULL: err_msg = "NULL"; break;
            case TOX_ERR_NEW_MALLOC: err_msg = "MALLOC"; break;
            case TOX_ERR_NEW_PORT_ALLOC: err_msg = "PORT_ALLOC"; break;
            case TOX_ERR_NEW_PROXY_BAD_TYPE: err_msg = "PROXY_BAD_TYPE"; break;
            case TOX_ERR_NEW_PROXY_BAD_HOST: err_msg = "PROXY_BAD_HOST"; break;
            case TOX_ERR_NEW_PROXY_BAD_PORT: err_msg = "PROXY_BAD_PORT"; break;
            case TOX_ERR_NEW_PROXY_NOT_FOUND: err_msg = "PROXY_NOT_FOUND"; break;
            case TOX_ERR_NEW_LOAD_ENCRYPTED: err_msg = "LOAD_ENCRYPTED"; break;
            case TOX_ERR_NEW_LOAD_BAD_FORMAT: err_msg = "LOAD_BAD_FORMAT"; break;
            default: err_msg = "UNKNOWN_ERROR"; break;
            }
            logDebugInfo("Failed to initialize Tox with saved data: " + err_msg);
            updateConnectionStatus("Failed to initialize Tox with saved data: " + err_msg);
            initTox(false); // Инициализируем без загрузки сохраненных данных
            return;
        }

        logDebugInfo("Tox initialized successfully with saved data");
        updateConnectionStatus("Tox initialized successfully with saved data");
        updateFriendList();

        // Отображение версии Tox
        displayToxVersion();
    }
    else {
        logDebugInfo("No saved Tox data found, starting fresh");
        updateConnectionStatus("No saved Tox data found, starting fresh");
        initTox(false); // Инициализируем без загрузки сохраненных данных
    }
}

void MainWindow::saveLoginData() {
    QFile file("login_data.txt");
    if (file.open(QIODevice::WriteOnly)) {
        QTextStream out(&file);
        out << "Username: " << username << "\n";
        out << "Password: " << password << "\n";
        logDebugInfo("Login data saved");
    }
    else {
        logDebugInfo("Failed to save login data");
    }
    file.close();
}

bool MainWindow::loadLoginData() {
    QFile file("login_data.txt");
    if (file.exists() && file.open(QIODevice::ReadOnly)) {
        QTextStream in(&file);
        QString line = in.readLine();
        username = line.section(' ', 1, 1);
        line = in.readLine();
        password = line.section(' ', 1, 1);
        ui->usernameLineEdit->setText(username);
        ui->passwordLineEdit->setText(password);
        logDebugInfo("Loaded Username: " + username);
        return true;
    }
    file.close();
    return false;
}

void MainWindow::onConnectionStatusChangedStatic(Tox* tox, TOX_CONNECTION connection_status, void* user_data) {
    MainWindow* mainWindow = static_cast<MainWindow*>(user_data);
    if (mainWindow) {
        mainWindow->onConnectionStatusChanged(tox, connection_status);
    }
}

void MainWindow::onConnectionStatusChanged(Tox* tox, TOX_CONNECTION connection_status) {
    QString status;
    switch (connection_status) {
    case TOX_CONNECTION_NONE:
        status = "Disconnected";
        break;
    case TOX_CONNECTION_TCP:
        status = "Connected via TCP";
        break;
    case TOX_CONNECTION_UDP:
        status = "Connected via UDP";
        break;
    }
    logDebugInfo("Connection status changed: " + status);
    updateConnectionStatus(status);

    if (connection_status != TOX_CONNECTION_NONE) {
        QString nodeAddress = QString("%1:%2").arg("188.225.9.167").arg(33445);  // Replace with actual logic if needed
        logDebugInfo("Node IP: " + nodeAddress);
        updateConnectionStatus("Node IP: " + nodeAddress);
    }
}

void MainWindow::friendRequestCallbackStatic(Tox* tox, const uint8_t* public_key, const uint8_t* message, size_t length, void* user_data) {
    MainWindow* mainWindow = static_cast<MainWindow*>(user_data);
    if (mainWindow) {
        mainWindow->friendRequestCallback(tox, public_key, message, length);
    }
}

void MainWindow::friendRequestCallback(Tox* tox, const uint8_t* public_key, const uint8_t* message, size_t length) {
    QString friendIdStr = QByteArray((char*)public_key, TOX_PUBLIC_KEY_SIZE).toHex();
    QString msgStr = QString::fromUtf8((char*)message, length);

    logDebugInfo("Received friend request from: " + friendIdStr + " with message: " + msgStr);

    TOX_ERR_FRIEND_ADD err;
    uint32_t friendNumber = tox_friend_add_norequest(tox, public_key, &err);
    if (err != TOX_ERR_FRIEND_ADD_OK) {
        QString errMsg;
        switch (err) {
        case TOX_ERR_FRIEND_ADD_NULL:
            errMsg = "One of the arguments to the function was NULL when it was not expected.";
            break;
        case TOX_ERR_FRIEND_ADD_TOO_LONG:
            errMsg = "The length of the message exceeded TOX_MAX_FRIEND_REQUEST_LENGTH.";
            break;
        case TOX_ERR_FRIEND_ADD_NO_MESSAGE:
            errMsg = "The message was empty.";
            break;
        case TOX_ERR_FRIEND_ADD_OWN_KEY:
            errMsg = "The friend address belonged to the sending client.";
            break;
        case TOX_ERR_FRIEND_ADD_ALREADY_SENT:
            errMsg = "A request has already been sent, but has not yet been answered.";
            break;
        case TOX_ERR_FRIEND_ADD_BAD_CHECKSUM:
            errMsg = "The friend address checksum failed.";
            break;
        case TOX_ERR_FRIEND_ADD_SET_NEW_NOSPAM:
            errMsg = "The friend address was generated with a different nospam.";
            break;
        case TOX_ERR_FRIEND_ADD_MALLOC:
            errMsg = "A memory allocation failed.";
            break;
        default:
            errMsg = "Unknown error.";
            break;
        }
        logDebugInfo("Failed to accept friend request: " + errMsg);
    }
    else {
        updateFriendList();
        logDebugInfo("Automatically accepted friend request from: " + friendIdStr);
        logDebugInfo("Message from friend: " + msgStr);
    }
}

void MainWindow::friendMessageCallbackStatic(Tox* tox, uint32_t friend_number, TOX_MESSAGE_TYPE type, const uint8_t* message, size_t length, void* user_data) {
    MainWindow* mainWindow = static_cast<MainWindow*>(user_data);
    if (mainWindow) {
        mainWindow->friendMessageCallback(tox, friend_number, type, message, length);
    }
}

void MainWindow::friendMessageCallback(Tox* tox, uint32_t friend_number, TOX_MESSAGE_TYPE type, const uint8_t* message, size_t length) {
    QString msgStr = QString::fromUtf8((char*)message, length);
    QString friendName = "Friend " + QString::number(friend_number);

    logDebugInfo(friendName + ": " + msgStr);
    ui->messagesTextEdit->append(friendName + ": " + msgStr);
}

void MainWindow::displayToxVersion() {
    QString version = QString("%1.%2.%3").arg(tox_version_major()).arg(tox_version_minor()).arg(tox_version_patch());
    ui->versionLineEdit->setText(version);
}