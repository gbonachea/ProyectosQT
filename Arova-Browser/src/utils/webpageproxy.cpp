#include "webpageproxy.h"

#include <qnetworkrequest.h>
#include <qvariant.h>
#include <qwebenginepage.h>
#include <qwebengineprofile.h>

#define ATTRIBUTE_ID QNetworkRequest::User + 100

WebPageProxy::WebPageProxy(QObject *parent)
    : QWebEnginePage(parent)
{
}

void WebPageProxy::load(const QNetworkRequest &request)
{
    QWebEnginePage::setUrl(request.url());
}

int WebPageProxy::pageAttributeId()
{
    return ATTRIBUTE_ID;
}

void WebPageProxy::populateNetworkRequest(QNetworkRequest &request)
{
    QVariant variant = QVariant::fromValue((void *)this);
    request.setAttribute((QNetworkRequest::Attribute)(pageAttributeId()), variant);
}

QUrl WebPageProxy::baseUrl() const
{
    return url();
}

void WebPageProxy::triggerAction(QWebEnginePage::WebAction action, bool checked)
{
    QWebEnginePage::triggerAction(action, checked);
}
