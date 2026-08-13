/*
 * Copyright 2009 Benjamin C. Meyer <ben@meyerhome.net>
 * Copyright 2009 Jakub Wieczorek <faw217@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301  USA
 */

#include "webpage.h"

#include "browserapplication.h"
#include "downloadmanager.h"
#include "historymanager.h"
#include "networkaccessmanager.h"
#include "opensearchengine.h"
#include "opensearchmanager.h"
#include "tabwidget.h"
#include "toolbarsearch.h"
#include "webview.h"

#include <qbuffer.h>
#include <qdesktopservices.h>
#include <qjsondocument.h>
#include <qjsonarray.h>
#include <qjsonobject.h>
#include <qmessagebox.h>
#include <qnetworkreply.h>
#include <qnetworkrequest.h>
#include <qsettings.h>
#include <qurlquery.h>
#include <qwebchannel.h>

QString WebPage::s_userAgent;

JavaScriptExternalObject::JavaScriptExternalObject(QObject *parent)
    : QObject(parent)
{
}

void JavaScriptExternalObject::AddSearchProvider(const QString &url)
{
    ToolbarSearch::openSearchManager()->addEngine(QUrl(url));
}

Q_DECLARE_METATYPE(OpenSearchEngine*)
JavaScriptArovaObject::JavaScriptArovaObject(QObject *parent)
    : QObject(parent)
{
    static const char *translations[] = {
        QT_TR_NOOP("Welcome to Arova!"),
        QT_TR_NOOP("Arova Start"),
        QT_TR_NOOP("Search!"),
        QT_TR_NOOP("Search results provided by"),
        QT_TR_NOOP("About Arova")
    };
    Q_UNUSED(translations);

    qRegisterMetaType<OpenSearchEngine*>("OpenSearchEngine*");
}

QString JavaScriptArovaObject::translate(const QString &string)
{
    QString translatedString = tr(string.toUtf8().constData());

    if (translatedString != string)
        return translatedString;
    else
        return qApp->translate("QApplication", string.toUtf8().constData());
}

QObject *JavaScriptArovaObject::currentEngine() const
{
    OpenSearchManager *manager = ToolbarSearch::openSearchManager();
    OpenSearchEngine *engine = manager->engine(QLatin1String("DuckDuckGo"));
    return engine ? engine : manager->currentEngine();
}

QString JavaScriptArovaObject::searchUrl(const QString &string) const
{
    QUrl url(QLatin1String("https://duckduckgo.com/"));
    QUrlQuery query;
    query.addQueryItem(QLatin1String("q"), string);
    url.setQuery(query);
    return QString::fromUtf8(url.toEncoded());
}

WebPage::WebPage(QObject *parent)
    : WebPageProxy(parent)
    , m_openTargetBlankLinksIn(TabWidget::NewWindow)
    , m_javaScriptExternalObject(0)
    , m_javaScriptArovaObject(0)
    , m_webChannel(0)
{
    addExternalBinding();
    loadSettings();
}

WebPage::~WebPage()
{
}

QList<WebPageLinkedResource> WebPage::linkedResources(const QString &relation)
{
    QList<WebPageLinkedResource> resources;

    QUrl baseUrl = url();

    QString script = QStringLiteral(
        "(function() {"
        "    var links = document.querySelectorAll('link');"
        "    var result = [];"
        "    for (var i = 0; i < links.length; i++) {"
        "        result.push({"
        "            rel: links[i].getAttribute('rel') || '',"
        "            type: links[i].getAttribute('type') || '',"
        "            href: links[i].getAttribute('href') || '',"
        "            title: links[i].getAttribute('title') || ''"
        "        });"
        "    }"
        "    return JSON.stringify(result);"
        "})()"
    );

    m_linkedResources.clear();
    runJavaScript(script, [this, baseUrl](const QVariant &result) {
        QString json = result.toString();
        QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
        QJsonArray arr = doc.array();
        for (int i = 0; i < arr.size(); ++i) {
            QJsonObject obj = arr[i].toObject();
            WebPageLinkedResource resource;
            resource.rel = obj[QStringLiteral("rel")].toString();
            resource.type = obj[QStringLiteral("type")].toString();
            resource.href = baseUrl.resolved(QUrl(obj[QStringLiteral("href")].toString()));
            resource.title = obj[QStringLiteral("title")].toString();
            if (!resource.href.isEmpty() && !resource.type.isEmpty())
                m_linkedResources.append(resource);
        }
    });

    for (int i = 0; i < m_linkedResources.size(); ++i) {
        const WebPageLinkedResource &resource = m_linkedResources.at(i);
        if (!relation.isEmpty() && resource.rel != relation)
            continue;
        resources.append(resource);
    }
    return resources;
}

void WebPage::populateNetworkRequest(QNetworkRequest &request)
{
    WebPageProxy::populateNetworkRequest(request);
}

void WebPage::addExternalBinding()
{
    if (!m_webChannel) {
        m_webChannel = new QWebChannel(this);

        if (!m_javaScriptExternalObject)
            m_javaScriptExternalObject = new JavaScriptExternalObject(this);

        if (!m_javaScriptArovaObject)
            m_javaScriptArovaObject = new JavaScriptArovaObject(this);

        m_webChannel->registerObject(QStringLiteral("external"), m_javaScriptExternalObject);
        m_webChannel->registerObject(QStringLiteral("arova"), m_javaScriptArovaObject);

        setWebChannel(m_webChannel);
    }
}

QString WebPage::userAgent()
{
    return s_userAgent;
}

void WebPage::setUserAgent(const QString &userAgent)
{
    if (userAgent == s_userAgent)
        return;

    QSettings settings;
    if (userAgent.isEmpty()) {
        settings.remove(QLatin1String("userAgent"));
    } else {
        settings.setValue(QLatin1String("userAgent"), userAgent);
    }

    s_userAgent = userAgent;
    QWebEngineProfile::defaultProfile()->setHttpUserAgent(s_userAgent);
}

bool WebPage::acceptNavigationRequest(const QUrl &url, NavigationType type, bool isMainFrame)
{
    QString scheme = url.scheme();
    if (scheme == QLatin1String("mailto")
        || scheme == QLatin1String("ftp")) {
        BrowserApplication::instance()->askDesktopToOpenUrl(url);
        return false;
    }

    if (type == NavigationTypeFormSubmitted) {
        QMessageBox::StandardButton button = QMessageBox::warning(qobject_cast<QWidget*>(parent()), tr("Resending POST request"),
                             tr("In order to display the site, the request along with all the data must be sent once again, "
                                "which may lead to some unexpected behaviour of the site e.g. the same action might be "
                                "performed once again. Do you want to continue anyway?"), QMessageBox::Yes | QMessageBox::No);
        if (button != QMessageBox::Yes)
            return false;
    }

    if (isMainFrame) {
        TabWidget::OpenUrlIn openIn = TabWidget::CurrentTab;
        openIn = TabWidget::modifyWithUserBehavior(openIn);

        if (openIn == TabWidget::NewSelectedTab
            || openIn == TabWidget::NewNotSelectedTab
            || openIn == TabWidget::NewWindow) {
            if (WebView *webView = qobject_cast<WebView*>(parent())) {
                TabWidget *tabWidget = webView->tabWidget();
                if (tabWidget) {
                    WebView *newView = tabWidget->getView(openIn, webView);
                    QWebEnginePage *page = 0;
                    if (newView)
                        page = newView->page();
                    if (page)
                        page->setUrl(url);
                }
            }
            return false;
        }

        bool accepted = QWebEnginePage::acceptNavigationRequest(url, type, isMainFrame);
        if (accepted) {
            m_requestedUrl = url;
            emit aboutToLoadUrl(url);
        }
        return accepted;
    }

    return QWebEnginePage::acceptNavigationRequest(url, type, isMainFrame);
}

void WebPage::loadSettings()
{
    QSettings settings;
    settings.beginGroup(QLatin1String("tabs"));
    m_openTargetBlankLinksIn = (TabWidget::OpenUrlIn)settings.value(QLatin1String("openTargetBlankLinksIn"),
                                                                    TabWidget::NewSelectedTab).toInt();
    settings.endGroup();
    s_userAgent = settings.value(QLatin1String("userAgent")).toString();
}

QWebEnginePage *WebPage::createWindow(QWebEnginePage::WebWindowType type)
{
    Q_UNUSED(type);
    if (WebView *webView = qobject_cast<WebView*>(parent())) {
        TabWidget *tabWidget = webView->tabWidget();
        if (tabWidget) {
            TabWidget::OpenUrlIn openIn = m_openTargetBlankLinksIn;
            openIn = TabWidget::modifyWithUserBehavior(openIn);
            return tabWidget->getView(openIn, webView)->page();
        }
    }
    return 0;
}
