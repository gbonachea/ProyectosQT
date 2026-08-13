#ifndef SEARCHSUGGESTIONMANAGER_H
#define SEARCHSUGGESTIONMANAGER_H

#include <qobject.h>
#include <qstringlist.h>
#include <qnetworkreply.h>

class SearchSuggestionManager : public QObject
{
    Q_OBJECT

public:
    SearchSuggestionManager(QObject *parent = 0);

    void fetchSuggestions(const QString &text);
    void abort();

signals:
    void suggestionsReady(const QStringList &suggestions);

private slots:
    void replyFinished(QNetworkReply *reply);

private:
    QNetworkReply *m_reply;
};

#endif
