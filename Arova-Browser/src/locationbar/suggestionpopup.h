#ifndef SUGGESTIONPOPUP_H
#define SUGGESTIONPOPUP_H

#include <qlistwidget.h>
#include <qstringlist.h>

class SuggestionPopup : public QListWidget
{
    Q_OBJECT

public:
    SuggestionPopup(QWidget *parent = 0);

    void appendSuggestions(const QStringList &suggestions);
    void showBelow(QWidget *widget, int maxHeight = 300);

signals:
    void suggestionSelected(const QString &url);

protected:
    bool eventFilter(QObject *obj, QEvent *event);

    void keyPressEvent(QKeyEvent *event);

private:
    void hideOnDeactivate();

private slots:
    void handleActivated(QListWidgetItem *item);
};

#endif
