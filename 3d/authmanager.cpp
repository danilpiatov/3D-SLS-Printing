#include "authmanager.h"
#include <QDebug>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QRandomGenerator>

QString AuthManager::generateSalt() {
    QRandomGenerator gen;
    QByteArray salt;
    salt.resize(32);
    for(int i = 0; i < salt.size(); ++i) {
        salt[i] = gen.generate() % 256;
    }
    return QString(salt.toHex());
}

bool AuthManager::registerUser(const QString &username, const QString &password, const QString &role) {
    QString salt = generateSalt();
    QString hash = hashPassword(password, salt);

    QSqlQuery query;
    query.prepare("INSERT INTO users(username, password_hash, salt, role) "
                  "VALUES(:a, :b, :c, :d)");
    query.bindValue(":a", username);
    query.bindValue(":b",hash);
    query.bindValue(":c",salt);
    query.bindValue(":d",role);

    return query.exec();
}

AuthManager::AuthManager(QObject *parent) : QObject(parent) {
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("auth.db");
    initDatabase();
}

void AuthManager::initDatabase() {
    if (!db.open()) {
        qCritical() << "Failed to open database:" << db.lastError().text();
        return;
    }
    qDebug() << "Database opened successfully";

    QSqlQuery query;
    query.exec("CREATE TABLE IF NOT EXISTS users ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT,"
               "username TEXT UNIQUE NOT NULL,"
               "password_hash TEXT NOT NULL,"
               "salt TEXT NOT NULL,"
               "role TEXT NOT NULL)");

    qCritical() << "Query failed:" << query.lastError().text();
    qCritical() << "Executed SQL:" << query.executedQuery();
    registerUser("admin", "admin", "admin");
}

QString AuthManager::hashPassword(const QString &password, const QString &salt) {
    QByteArray data = (password + salt).toUtf8();
    return QCryptographicHash::hash(data, QCryptographicHash::Sha256).toHex();
}


UserInfo AuthManager::authenticate(const QString &username, const QString &password) {

    UserInfo result;
    QSqlQuery query;
    if (!db.isOpen()) {
        qCritical() << "Database connection is not open!";
        return result;
    }
    query.prepare("SELECT password_hash, salt, role FROM users WHERE username = ?");
    query.addBindValue(username);
    if (!query.exec()) {
        qCritical() << "Query failed:" << query.lastError().text();
        qCritical() << "Executed SQL:" << query.executedQuery();
        return result;
    }
    if(query.next()) {
        QString storedHash = query.value(0).toString();
        QString salt = query.value(1).toString();
        QString calculatedHash = hashPassword(password, salt);

        if(storedHash == calculatedHash) {
            result.isAuthenticated = true;
            result.username = username;
            result.role = query.value(2).toString();
        }
    }
    return result;
}

QList<QPair<QString, QString>> AuthManager::getAllUsers() {
    QList<QPair<QString, QString>> users;
    QSqlQuery query("SELECT username, role FROM users");
    while(query.next()) {
        users.append({query.value(0).toString(), query.value(1).toString()});
    }
    return users;
}

bool AuthManager::deleteUser(const QString &username) {
    QSqlQuery query;
    query.prepare("DELETE FROM users WHERE username = ?");
    query.addBindValue(username);
    return query.exec();
}