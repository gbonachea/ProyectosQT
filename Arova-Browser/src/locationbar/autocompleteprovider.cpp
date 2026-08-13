#include "autocompleteprovider.h"

#include "browserapplication.h"
#include "networkaccessmanager.h"

#include <qjsonarray.h>
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qnetworkrequest.h>
#include <qurlquery.h>

static QString phraseFromSuggestionValue(const QJsonValue &val)
{
    if (val.isString())
        return val.toString();
    if (val.isObject()) {
        const QJsonObject obj = val.toObject();
        const QString phrase = obj.value(QLatin1String("phrase")).toString();
        if (!phrase.isEmpty())
            return phrase;
        return obj.value(QLatin1String("text")).toString();
    }
    return QString();
}


AutocompleteProvider::AutocompleteProvider(QObject *parent)
    : QObject(parent)
    , m_manager(BrowserApplication::networkAccessManager())
    , m_reply(0)
    , m_pendingQuery()
{
}

void AutocompleteProvider::fetchSuggestions(const QString &text)
{
    abort();

    if (text.isEmpty())
        return;

    m_pendingQuery = text;

    QUrl apiUrl(QLatin1String("https://duckduckgo.com/ac/?"));
    QUrlQuery query;
    query.addQueryItem(QLatin1String("q"), text);
    query.addQueryItem(QLatin1String("type"), QLatin1String("list"));
    apiUrl.setQuery(query);

    QNetworkRequest request(apiUrl);
    request.setRawHeader("User-Agent", "Mozilla/5.0 (X11; Linux x86_64) Arova/1.0");
    request.setHeader(QNetworkRequest::ContentTypeHeader, QLatin1String("application/json"));

    m_reply = m_manager->get(request);
    connect(m_reply, SIGNAL(finished()), this, SLOT(replyFinished()));
}

void AutocompleteProvider::abort()
{
    if (m_reply) {
        m_reply->abort();
        m_reply->deleteLater();
        m_reply = 0;
    }
}

void AutocompleteProvider::replyFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply)
        return;

    reply->deleteLater();
    if (reply != m_reply)
        return;

    m_reply = 0;

    QStringList suggestions;
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray data = reply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        if (doc.isArray()) {
            QJsonArray arr = doc.array();
            if (arr.size() >= 2 && arr.at(1).isArray()) {
                QJsonArray items = arr.at(1).toArray();
                for (int i = 0; i < items.size(); ++i) {
                    const QString phrase = phraseFromSuggestionValue(items.at(i));
                    if (!phrase.isEmpty())
                        suggestions.append(phrase);
                }
            }
        } else if (doc.isObject()) {
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
        }
    }

    emit suggestionsReady(m_pendingQuery, suggestions);
}
