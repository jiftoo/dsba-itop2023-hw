#include "filmfilterproxymodel.h"
#include <QSortFilterProxyModel>
#include "filmlistmodel.h"

FilmFilterProxyModel::FilmFilterProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    this->yearFilter.ordering = NumericFilter::Ordering::Default;
    this->rateFilter.ordering = NumericFilter::Ordering::Default;
    this->voteFilter.ordering = NumericFilter::Ordering::Default;

    this->sort(0, Qt::AscendingOrder);
}

bool FilmFilterProxyModel::shouldDisplayBasedOnFilter(const FilmListModel::Film& film) const {
    bool shouldShow = true;
    shouldShow &= this->yearFilter.apply(film.year);
    shouldShow &= this->rateFilter.apply(film.rate);
    shouldShow &= this->voteFilter.apply(film.votes);
    return shouldShow;
}

void setPtrToNewFilter(FilmFilterProxyModel::NumericFilter& filter, QString order, QString value) {
    bool success = false;
    filter.ordering = FilmFilterProxyModel::NumericFilter::orderingFromString(order);
    filter.value_b = value.toDouble(&success);
}

QString FilmFilterProxyModel::updateFiltersFromQuery(const QString& in) {
    // reset filters
    setPtrToNewFilter(this->yearFilter, "default", "0.0");
    setPtrToNewFilter(this->rateFilter, "default", "0.0");
    setPtrToNewFilter(this->voteFilter, "default", "0.0");
    static QRegularExpression regex("([a-z]+):(<|>|<=|>=|)?([0-9.]+)");
    auto matchesIter = regex.globalMatch(in);
    while(matchesIter.hasNext()) {
        auto match = matchesIter.next();
        auto key   = match.captured(1);
        auto order = match.captured(2);
        auto value = match.captured(3);
        qDebug() << "Matched:" << key << order << value;
        if(key == "year") {
            setPtrToNewFilter(this->yearFilter, order, value);
            qDebug() << "Update year filter:" << order << value;
        }
        if(key == "rate") {
            setPtrToNewFilter(this->rateFilter, order, value);
            qDebug() << "Update rate filter:" << order << value;
        }
        if(key == "votes") {
            setPtrToNewFilter(this->voteFilter, order, value);
            qDebug() << "Update votes filter:" << order << value;
        }
    }

    this->invalidateFilter();

    QString clone = in;
    clone.remove(regex);
    return clone;
}

bool FilmFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const {
    auto source = (FilmListModel*)this->sourceModel();
    return this->shouldDisplayBasedOnFilter(source->films[sourceRow])
           && QSortFilterProxyModel::filterAcceptsRow(sourceRow, sourceParent);
}
