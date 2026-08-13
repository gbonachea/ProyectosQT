#ifndef HISTORYCOMPLETER_H
#define HISTORYCOMPLETER_H

#include "history.h"

#include <qcompleter.h>
#include <qregularexpression.h>
#include <qsortfilterproxymodel.h>
#include <qtableview.h>
#include <qtimer.h>

class QResizeEvent;
class HistoryCompletionView : public QTableView
{
public:
    HistoryCompletionView(QWidget *parent = 0);
    int sizeHintForRow(int row) const;

protected:
    void resizeEvent(QResizeEvent *event);
};

class HistoryCompletionModel : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(QString searchString READ searchString WRITE setSearchString)

public:
    HistoryCompletionModel(QObject *parent = 0);

    enum Roles { HistoryCompletionRole = HistoryFilterModel::MaxRole + 1 };

    QString searchString() const;
    void setSearchString(const QString &str);

    bool isValid() const;
    void setValid(bool b);

    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

protected:
    virtual bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;
    virtual bool lessThan(const QModelIndex &left, const QModelIndex &right) const;

private:
    QString m_searchString;
    QRegularExpression m_searchMatcher;
    QRegularExpression m_wordMatcher;
    bool m_isValid;
};

class HistoryCompleter : public QCompleter
{
    Q_OBJECT

public:
    HistoryCompleter(QObject *parent = 0);
    HistoryCompleter(QAbstractItemModel *model, QObject *parent = 0);

    virtual QString pathFromIndex(const QModelIndex &index) const;
    virtual QStringList splitPath(const QString &path) const;

protected:
    bool eventFilter(QObject *obj, QEvent *event);

private slots:
    void updateFilter();

private:
    void init();
    mutable QString m_searchString;
    mutable QTimer m_filterTimer;
};

#endif
