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

bool AuthManager::registerUser(const QString &username, const QString &password) {
    QString salt = generateSalt();
    QString hash = hashPassword(password, salt);

    QSqlQuery query;
    query.prepare("INSERT INTO users(username, password_hash, salt) "
                  "VALUES(?, ?, ?)");
    query.addBindValue(username);
    query.addBindValue(hash);
    query.addBindValue(salt);

    return query.exec();
}

AuthManager::AuthManager(QObject *parent) : QObject(parent) {
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("auth.db");
    initDatabase();
}

void AuthManager::initDatabase() {
    if(!db.open()) {
        qCritical() << "Database error:" << db.lastError().text();
        return;
    }

    QSqlQuery query;
    query.exec("CREATE TABLE IF NOT EXISTS users ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT,"
               "username TEXT UNIQUE NOT NULL,"
               "password_hash TEXT NOT NULL,"
               "salt TEXT NOT NULL)");

    registerUser("admin", "admin");
}

QString AuthManager::hashPassword(const QString &password, const QString &salt) {
    QByteArray data = (password + salt).toUtf8();
    return QCryptographicHash::hash(data, QCryptographicHash::Sha256).toHex();
}


bool AuthManager::authenticate(const QString &username, const QString &password) {
    QSqlQuery query;
    query.prepare("SELECT password_hash, salt FROM users WHERE username = ?");
    query.addBindValue(username);

    if(!query.exec() || !query.next()) {
        return false;
    }

    QString storedHash = query.value(0).toString();
    QString salt = query.value(1).toString();
    QString calculatedHash = hashPassword(password, salt);

    return (storedHash == calculatedHash);
}