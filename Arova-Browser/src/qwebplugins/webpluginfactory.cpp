/*
 * Copyright 2009 Benjamin C. Meyer <ben@meyerhome.net>
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

#include "webpluginfactory.h"

#include "clicktoflashplugin.h"

#include <qdebug.h>

WebPluginFactory::WebPluginFactory(QObject *parent)
    : QObject(parent)
    , m_loaded(false)
{
}

WebPluginFactory::~WebPluginFactory()
{
    qDeleteAll(m_plugins);
    m_plugins.clear();
}

void WebPluginFactory::refreshPlugins()
{
    init();
}

QList<ArovaWebPlugin*> WebPluginFactory::availablePlugins() const
{
    if (!m_loaded)
        init();
    return m_plugins;
}

void WebPluginFactory::init() const
{
    m_loaded = true;
    qDeleteAll(m_plugins);
    m_plugins.clear();
    m_plugins.append(new ClickToFlashPlugin);
}
