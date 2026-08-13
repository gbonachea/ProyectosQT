#ifndef WEBPAGEPROXY_H
#define WEBPAGEPROXY_H

#include <qwebenginepage.h>
#include <qwebenginesettings.h>
#include <qwebengineprofile.h>
#include <qnetworkaccessmanager.h>

class WebPageProxy : public QWebEnginePage
{
    Q_OBJECT

public:
    WebPageProxy(QObject *parent = nullptr);

    void load(const QNetworkRequest &request);
    static int pageAttributeId();

    // Emulation of QWebFrame methods
    QUrl baseUrl() const;
    QUrl requestedUrl() const { return m_requestedUrl; }
    void triggerAction(QWebEnginePage::WebAction action, bool checked = false);

signals:
    void aboutToLoadUrl(const QUrl &url);
    void scrollRequested(int dx, int dy, const QRect &scrollRect);

protected:
    friend class NetworkAccessManagerProxy;
    virtual void populateNetworkRequest(QNetworkRequest &request);
    QString m_requestedUrlString;

private:
    QUrl m_requestedUrl;
};

#endif // WEBPAGEPROXY_H
