#ifndef FILMLISTMODEL_H
#define FILMLISTMODEL_H

#include <QAbstractListModel>
#include <QStringListModel>
#include <vector>
#include <type_traits>

#define CSV_COLUMN_NAMES\
    "name","date", "rate",\
    "votes","genre","duration",\
    "type","certificate","nudity",\
    "violence"

class FilmListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    struct Film {
        QString name;
        int year;
        float rate;
        int votes;
        std::vector<QString> genres;
        int duration;
        QString type;
        QString certificate;
        QString nudity;
        QString violence;
    };

    explicit FilmListModel(QObject *parent = nullptr);

    void loadFilms(const std::string&);

    Film& getFilmData(int row);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    void addFilm(Film film);
    void removeFilm(int row);
    void changeFilm(int row, FilmListModel::Film newValue);

    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    std::vector<Film> films;
private:
};

#endif // FILMLISTMODEL_H
