/**
 * Copyright (c) 2009, Benjamin C. Meyer
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Benjamin Meyer nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "clicktoflashplugin.h"

#include "ui_clicktoflashsettings.h"
#include <qsettings.h>
#include <qstringlistmodel.h>

ClickToFlashPlugin::ClickToFlashPlugin()
    : ArovaWebPlugin()
    , m_loaded(false)
    , m_enabled(false)
{
}

void ClickToFlashPlugin::load()
{
    if (m_loaded)
        return;
    m_loaded = true;
    QSettings settings;
    settings.beginGroup(QLatin1String("webplugin/clicktoflash"));
    m_whitelist = settings.value(QLatin1String("whitelist")).toStringList();
    settings.endGroup();
    settings.beginGroup(QLatin1String("websettings"));
    m_enabled = settings.value(QLatin1String("enableClickToFlash"), false).toBool();
}

void ClickToFlashPlugin::save()
{
    QSettings settings;
    settings.beginGroup(QLatin1String("webplugin/clicktoflash"));
    settings.setValue(QLatin1String("whitelist"), m_whitelist);
}

void ClickToFlashPlugin::configure()
{
    load();
    QDialog dialog;
    Ui_ClickToFlashSettings ui;
    ui.setupUi(&dialog);
    QStringListModel *model = new QStringListModel(m_whitelist, ui.whitelist);
    ui.whitelist->setModel(model);
    if (dialog.exec() == QDialog::Accepted) {
        m_whitelist = model->stringList();
        save();
    }
}

bool ClickToFlashPlugin::isAnonymous() const
{
    return true;
}

bool ClickToFlashPlugin::isEnabled() const
{
    const_cast<ClickToFlashPlugin*>(this)->load();
    return m_enabled;
}

bool ClickToFlashPlugin::onWhitelist(const QString &host) const
{
    return m_whitelist.contains(host);
}

void ClickToFlashPlugin::addToWhitelist(const QString &host)
{
    m_whitelist.append(host);
    save();
}

void ClickToFlashPlugin::removeFromWhitelist(const QString &host)
{
    m_whitelist.removeOne(host);
    save();
}
