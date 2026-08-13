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

#include "clicktoflash.h"
#include "clicktoflashplugin.h"

#include <qwebenginepage.h>

ClickToFlash::ClickToFlash(ClickToFlashPlugin *plugin, QWebEnginePage *page, QObject *parent)
    : QObject(parent)
    , m_plugin(plugin)
    , m_page(page)
{
}

void ClickToFlash::inject()
{
    if (!m_page)
        return;

    const QString js = QLatin1String(
        "(function() {"
        "    var flashTypes = ['application/x-shockwave-flash', 'application/futuresplash'];"
        "    var elements = document.querySelectorAll('object[type], embed[type]');"
        "    for (var i = 0; i < elements.length; i++) {"
        "        var el = elements[i];"
        "        var type = el.getAttribute('type') || '';"
        "        if (flashTypes.indexOf(type) === -1) continue;"
        "        var placeholder = document.createElement('div');"
        "        placeholder.style.cssText = 'border:2px dashed #ccc;background:#f5f5f5;text-align:center;padding:20px;margin:5px;cursor:pointer;font-family:sans-serif;font-size:14px;color:#666;';"
        "        placeholder.innerHTML = '<div style=\"font-size:48px;margin-bottom:10px;\">&#9654;</div><div>Click to load Adobe Flash Player</div>';"
        "        placeholder.dataset.flashHtml = encodeURIComponent(el.outerHTML);"
        "        placeholder.onclick = function() {"
        "            this.outerHTML = decodeURIComponent(this.dataset.flashHtml);"
        "        };"
        "        el.parentNode.replaceChild(placeholder, el);"
        "    }"
        "})();"
    );

    m_page->runJavaScript(js);
}
