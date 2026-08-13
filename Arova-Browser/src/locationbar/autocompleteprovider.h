#ifndef AUTOCOMPLETEPROVIDER_H
#define AUTOCOMPLETEPROVIDER_H

#include <qobject.h>
#include <qstringlist.h>
#include <qnetworkreply.h>

class NetworkAccessManager;

class AutocompleteProvider : public QObject
{
    Q_OBJECT

public:
    AutocompleteProvider(QObject *parent = 0);

    void fetchSuggestions(const QString &text);
    void abort();

signals:
    void suggestionsReady(const QString &query, const QStringList &suggestions);

private slots:
    void replyFinished();

private:
    NetworkAccessManager *m_manager;
    QNetworkReply *m_reply;
    QString m_pendingQuery;
};

#endif
