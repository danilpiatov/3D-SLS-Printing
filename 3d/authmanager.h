#pragma once
#include <QObject>
#include <QtSql/QSqlDatabase>
#include <QCryptographicHash>

struct UserInfo {
    bool isAuthenticated = false;
    QString username;
    QString role;
};

class AuthManager : public QObject {
Q_OBJECT
public:
    explicit AuthManager(QObject *parent = nullptr);
    void initDatabase();
    UserInfo authenticate(const QString &username, const QString &password);
    bool registerUser(const QString &username, const QString &password, const QString &role);
    QList<QPair<QString, QString>> getAllUsers();
    bool deleteUser(const QString &username);

private:
    QSqlDatabase db;
    QString hashPassword(const QString &password, const QString &salt);
    QString generateSalt();
};