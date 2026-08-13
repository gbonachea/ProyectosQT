#include "searchsuggestionmanager.h"

#include <qnetworkaccessmanager.h>
#include <qurl.h>
#include <qnetworkreply.h>
#include <qjsonarray.h>
#include <qjsondocument.h>
#include <qjsonobject.h>

SearchSuggestionManager::SearchSuggestionManager(QObject *parent)
    : QObject(parent)
    , m_reply(0)
{
}

void SearchSuggestionManager::fetchSuggestions(const QString &text)
{
    abort();

    if (text.isEmpty())
        return;

    QUrl url(QString(QLatin1String("https://duckduckgo.com/ac/?q=%1&type=list"))
             .arg(QString::fromUtf8(QUrl::toPercentEncoding(text))));

    QNetworkRequest request(url);
    request.setRawHeader("User-Agent", "Arova/1.0");

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    m_reply = manager->get(request);
    connect(m_reply, SIGNAL(finished()), this, SLOT(replyFinished()));
}

void SearchSuggestionManager::abort()
{
    if (m_reply) {
        m_reply->abort();
        m_reply->deleteLater();
        m_reply = 0;
    }
}

void SearchSuggestionManager::replyFinished(QNetworkReply *reply)
{
    reply->deleteLater();
    if (reply != m_reply)
        return;

    QStringList suggestions;
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray data = reply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        if (doc.isObject()) {
            QJsonObject obj = doc.object();
            QJsonValue resultsVal = obj.value(QLatin1String("results"));
            if (resultsVal.isArray()) {
                QJsonArray items = resultsVal.toArray();
                for (int i = 0; i < items.size(); ++i) {
                    QJsonObject item = items[i].toObject();
                    QString phrase = item.value(QLatin1String("text")).toString();
                    if (!phrase.isEmpty())
                        suggestions.append(phrase);
                }
            }
        } else if (doc.isArray()) {
            QJsonArray arr = doc.array();
            if (arr.size() >= 2 && arr[1].isArray()) {
                QJsonArray items = arr[1].toArray();
                for (int i = 0; i < items.size(); ++i)
                    suggestions.append(items[i].toString());
            }
        }
    }

    m_reply = 0;
    emit suggestionsReady(suggestions);
}
