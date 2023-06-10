#ifndef FILMFILTERPROXYMODEL_H
#define FILMFILTERPROXYMODEL_H

#include <QAbstractListModel>
#include <QSortFilterProxyModel>
#include "filmlistmodel.h"

class FilmFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    struct NumericFilter;

    explicit FilmFilterProxyModel(QObject *parent = nullptr);

    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;

    bool shouldDisplayBasedOnFilter(const FilmListModel::Film& film) const;

    void setYearFilter(FilmFilterProxyModel::NumericFilter filter);

    QString updateFiltersFromQuery(const QString& in);

    void updateQueryCache(const QString& query);

//    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

//    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    struct NumericFilter {
        enum Ordering {
            Default,
            Less,
            LessEqual,
            Greater,
            GreaterEqual,
            Equal
        };
        Ordering ordering = Ordering::Default;
        double value_b = 0.0;

        template<
            typename T,
            typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type
            >
        bool apply(T a) const {
            T b = (T)this->value_b;
            if(this->ordering == NumericFilter::Default) {
                return true;
            }
            if(this->ordering == NumericFilter::Equal) {
                return a == b;
            }
            if(this->ordering == NumericFilter::Greater) {
                return a >  b;
            }
            if(this->ordering == NumericFilter::GreaterEqual) {
                return a >= b;
            }
            if(this->ordering == NumericFilter::Less) {
                return a <  b;
            }
            if(this->ordering == NumericFilter::LessEqual) {
                return a <= b;
            }
            return false;
        }

        Ordering static orderingFromString(QString& str) {
            if(str == "<") {
                return NumericFilter::Less;
            }
            if(str == "<=") {
                return NumericFilter::LessEqual;
            }
            if(str == ">") {
                return NumericFilter::Greater;
            }
            if(str == ">=") {
                return NumericFilter::GreaterEqual;
            }
            if(str == "=" || str.isEmpty()) {
                return NumericFilter::Equal;
            }
            return NumericFilter::Default;
        }
    };

private:
    NumericFilter yearFilter;
    NumericFilter rateFilter;
    NumericFilter voteFilter;

};

#endif // FILMFILTERPROXYMODEL_H
