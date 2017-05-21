
#include "urllockchecker.h"
#include <QEventLoop>
#include <QVariant>

QString UrlLockChecker::baseUrl("https://api.antizapret.info");

UrlLockChecker::UrlLockChecker()
: mng_m(new QNetworkAccessManager(this))
, checkUrl_m()
{

}

UrlLockChecker::UrlLockChecker(const QString& checkurl)
: mng_m(new QNetworkAccessManager(this))
{
    setCheckUrl(checkurl);
}

UrlLockChecker::~UrlLockChecker()
{
    delete mng_m;
}

void UrlLockChecker::setCheckUrl(const QString& checkurl)
{
    QUrl newheckurl(checkurl);

    checkUrl_m = newheckurl;

    if( checkUrl_m.isValid() )
    {
        hostName_m = checkUrl_m.host().replace("www.", "");
    }
    else
    {
        hostName_m = "";
    }
}

QUrl UrlLockChecker::getCheckUrl(void) const
{
    return checkUrl_m;
}

QString UrlLockChecker::getHostName(void) const
{
    return hostName_m;
}

bool UrlLockChecker::isLock()
{
    bool UrlLock = false;

    if( !checkUrl_m.isValid() )
    {
        throw std::invalid_argument("Ссылка не корректна!"
                                    " Запрос невозможен.");
    }

    QString queryStr = buildQueryOneObject(typeFormat::small);
    QUrl urlRequest(queryStr);
    QNetworkRequest request(urlRequest);
    QNetworkReply* reply = mng_m->get(request);

    // реализуем ожидание конца загрузки
    QEventLoop loop;
    connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();

    // выводим содержимое

    if (reply->error() == QNetworkReply::NoError)
    {
        QVariant statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

        if ( statusCode.isValid() && statusCode.toInt() != 200)
        {
            qDebug() << "Код возврата: " << statusCode.toString();
            reply->deleteLater();
            return UrlLock;
        }

        QByteArray content = reply->readAll();
        QTextCodec *codec = QTextCodec::codecForName("utf8");
        QString encReply = codec->toUnicode(content.data());
        if( encReply.at(0) == '1' )
        {
            UrlLock = true;
        }
        else if( 0 == QString::compare("piff-paff", encReply, Qt::CaseInsensitive) )
        {
            qDebug() << "Запрос не корректный: "
                     << queryStr << "\n"
                     << checkUrl_m.toString();
            UrlLock = false;
        }
        else
        {
            UrlLock = false;
        }
    }
    else
    {
        qDebug() << reply->errorString();
    }

    reply->deleteLater();

    return UrlLock;
}

bool UrlLockChecker::isValid(void) const
{
    return checkUrl_m.isValid();
}

QString UrlLockChecker::buildQueryOneObject(const typeFormat format)
{
    return UrlLockChecker::baseUrl +
            QStringLiteral("/get.php?item=%1&type=%2")
            .arg(hostName_m)
            .arg(getStringNameFormat(format));
}

QString UrlLockChecker::getStringNameFormat(const typeFormat format)
{
    QString formatStr;
    switch (format) {
    case typeFormat::small: formatStr = "small";    break;
    case typeFormat::csv:   formatStr = "csv";      break;
    case typeFormat::json:  formatStr = "json";     break;
    case typeFormat::xml:   formatStr = "xml";      break;
    default:                formatStr = "csv";      break;
    }

    return formatStr;
}
