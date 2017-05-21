#ifndef URLLOCKCHECKER_H
#define URLLOCKCHECKER_H

#include <QString>
#include <QDateTime>
#include <QObject>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QUrl>
#include <QTextCodec>

class UrlLockChecker: public QObject
{

    Q_OBJECT

public:

    UrlLockChecker();
    explicit UrlLockChecker(const QString& checkurl);
    ~UrlLockChecker();

    void setCheckUrl(const QString& checkurl);
    QUrl getCheckUrl(void) const;

    QString getHostName(void) const;

    bool isLock();
    bool isValid(void) const;

    static QString baseUrl;

private:
    enum class typeFormat : int
    {
        small,
        csv,
        json,
        xml
    };

    QNetworkAccessManager* mng_m;
    QUrl checkUrl_m;
    QString hostName_m{""};

    QString buildQueryOneObject(const typeFormat format);
    QString getStringNameFormat(const typeFormat format);
};


#endif // URLLOCKCHECKER_H
