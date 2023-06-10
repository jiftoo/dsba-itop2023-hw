#ifndef ADDFILMWIDGET_H
#define ADDFILMWIDGET_H

#include <QDialog>
#include "filmlistmodel.h"

namespace Ui {
class AddFilmWidget;
}

class AddFilmWidget : public QDialog
{
    Q_OBJECT

public:
    FilmListModel::Film newFilm;

    explicit AddFilmWidget(const FilmListModel::Film* base = nullptr, QWidget *parent = nullptr);

    void updateAddButtonLockState();

    bool checkDataValid() const;

    void checkDataAndClose();

    ~AddFilmWidget();

private:
    Ui::AddFilmWidget *ui;

private slots:
    void on_dial_sliderMoved(int position);
};

#endif // ADDFILMWIDGET_H
