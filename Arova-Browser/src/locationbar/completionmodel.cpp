#include "completionmodel.h"

#include <qicon.h>

CompletionModel::CompletionModel(QObject *parent)
    : HistoryCompletionModel(parent)
{
}

int CompletionModel::historyRowCount() const
{
    return HistoryCompletionModel::rowCount();
}

bool CompletionModel::isSuggestionRow(int row) const
{
    int suggestions = m_suggestions.size();
    return suggestions > 0 && row < suggestions;
}

bool CompletionModel::isSeparatorRow(int row) const
{
    int suggestions = m_suggestions.size();
    return suggestions > 0 && row == suggestions;
}

int CompletionModel::mapToHistoryRow(int row) const
{
    int suggestions = m_suggestions.size();
    return row - suggestions - (suggestions > 0 ? 1 : 0);
}

int CompletionModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    int suggestions = m_suggestions.size();
    int history = historyRowCount();
    if (suggestions > 0)
        return suggestions + 1 + history;
    return history;
}

QVariant CompletionModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    int row = index.row();

    if (role == HistoryCompletionRole) {
        if (isValid())
            return QLatin1String("a");
        return QLatin1String("b");
    }

    if (isSuggestionRow(row)) {
        if (index.column() != 0)
            return QVariant();
        if (role == Qt::DisplayRole)
            return m_suggestions.at(row);
        if (role == Qt::DecorationRole)
            return QIcon::fromTheme(QLatin1String("edit-find"));
        return QVariant();
    }

    if (isSeparatorRow(row)) {
        if (role == Qt::DisplayRole)
            return QString(75, QChar(0x2501));
        return QVariant();
    }

    int historyRow = mapToHistoryRow(row);
    QModelIndex historyIndex = HistoryCompletionModel::index(historyRow, index.column());
    if (!historyIndex.isValid())
        return QVariant();
    return HistoryCompletionModel::data(historyIndex, role);
}

QModelIndex CompletionModel::index(int row, int column, const QModelIndex &parent) const
{
    if (parent.isValid())
        return QModelIndex();
    if (row < 0 || row >= rowCount())
        return QModelIndex();
    if (column < 0 || column >= 2)
        return QModelIndex();
    return createIndex(row, column);
}

QModelIndex CompletionModel::parent(const QModelIndex &child) const
{
    Q_UNUSED(child)
    return QModelIndex();
}

Qt::ItemFlags CompletionModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    int row = index.row();

    if (isSuggestionRow(row))
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled;

    if (isSeparatorRow(row))
        return Qt::ItemIsEnabled;

    int historyRow = mapToHistoryRow(row);
    QModelIndex historyIndex = HistoryCompletionModel::index(historyRow, index.column());
    if (!historyIndex.isValid())
        return Qt::NoItemFlags;
    return HistoryCompletionModel::flags(historyIndex);
}

void CompletionModel::setSuggestions(const QStringList &suggestions)
{
    beginResetModel();
    m_suggestions = suggestions;
    endResetModel();
}
