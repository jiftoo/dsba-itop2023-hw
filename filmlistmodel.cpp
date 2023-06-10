#include "filmlistmodel.h"
#include "csv.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <sstream>
#include <QAbstractListModel>
#include <QStringListModel>

std::vector<QString> splitString(std::string& text, char delim) {
    std::string line;
    std::vector<QString> vec;
    std::stringstream ss(text);
    while(std::getline(ss, line, delim)) {
        vec.push_back(QString::fromStdString(line));
    }
    return vec;
}

FilmListModel::FilmListModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

void FilmListModel::loadFilms(const std::string& path) {
    this->beginResetModel();

    io::CSVReader<10> csv(path);
    csv.read_header(io::ignore_extra_column, CSV_COLUMN_NAMES);

    std::string name;
    int year;
    float rate;
    int votes;
    std::string genreList;
    int duration;
    std::string type;
    std::string certificate;
    std::string nudity;
    std::string violence;
    try {
        while(csv.read_row(name, year, rate, votes, genreList, duration, type, certificate, nudity, violence)) {
            QString nameQstr = QString::fromStdString(name);
            std::vector<QString> genres = splitString(genreList, ',');
            QString typeQstr = QString::fromStdString(type);
            QString certificateQstr = QString::fromStdString(certificate);
            QString nudityQstr = QString::fromStdString(nudity);
            QString violenceQstr = QString::fromStdString(violence);
            this->films.push_back(Film {
                nameQstr,
                year,
                rate,
                votes,
                genres,
                duration,
                typeQstr,
                certificateQstr,
                nudityQstr,
                violenceQstr,
            });
        }
    } catch(std::exception& ex) {
        qDebug() << ex.what();
        throw ex;
    }

    std::sort(films.begin(), films.end(), [](Film& a, Film& b) {
        return a.name < b.name;
    });

    qDebug() << "Loaded .csv:" << films.size() << "films.";

    this->endResetModel();
}

FilmListModel::Film& FilmListModel::getFilmData(int row) {
    return this->films[row];
}

int FilmListModel::rowCount(const QModelIndex &parent) const
{
    return films.size();
}

QVariant FilmListModel::data(const QModelIndex &index, int role) const
{
    if(index.isValid() && role == Qt::DisplayRole)
    {
//        qDebug() << "Data" << index.row() << this->films[index.row()].name << role;
        return this->films[index.row()].name;
    }

    return QVariant();
}

void FilmListModel::addFilm(FilmListModel::Film film) {
    int c = this->rowCount();
    beginInsertRows(QModelIndex(), c, c);
    this->films.push_back(film);
    endInsertRows();
}

void FilmListModel::removeFilm(int row) {
    beginRemoveRows(QModelIndex(), row, row);
    this->films.erase(this->films.begin() + row);
    endRemoveRows();
}

void FilmListModel::changeFilm(int row, FilmListModel::Film newValue) {
    this->films[row] = newValue;

    emit dataChanged(index(row), index(row));
}

bool FilmListModel::insertRows(int row, int count, const QModelIndex &parent) {
//    beginInsertRows(parent, row, row + count);
//    endInsertRows();
    return true;
}
