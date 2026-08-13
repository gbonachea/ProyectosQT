#ifndef COMPLETIONMODEL_H
#define COMPLETIONMODEL_H

#include <historycompleter.h>

class CompletionModel : public HistoryCompletionModel
{
    Q_OBJECT

public:
    CompletionModel(QObject *parent = 0);

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    virtual QModelIndex parent(const QModelIndex &child) const;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;

    void setSuggestions(const QStringList &suggestions);

private:
    int mapToHistoryRow(int row) const;
    bool isSuggestionRow(int row) const;
    bool isSeparatorRow(int row) const;
    int historyRowCount() const;
    QStringList m_suggestions;
};

#endif
