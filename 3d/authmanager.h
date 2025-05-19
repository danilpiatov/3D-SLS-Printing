#include <QObject>
#include <QtSql/QSqlDatabase>
#include <QCryptographicHash>

class AuthManager : public QObject {
Q_OBJECT
public:
    explicit AuthManager(QObject *parent = nullptr);
    bool authenticate(const QString &username, const QString &password);
    void initDatabase();

private:
    QSqlDatabase db;
    QString hashPassword(const QString &password, const QString &salt);
    bool registerUser(const QString &username, const QString &password);
    QString generateSalt();
};