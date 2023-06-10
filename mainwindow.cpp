#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <iostream>
#include <sstream>
#include <string>
#include <QTableWidgetItem>
#include <QCheckBox>
#include <QMessageBox>
#include <QMenu>
#include <QPixmap>
#include <QListView>
#include <QFileDialog>
#include <QStringListModel>
#include <QSortFilterProxyModel>
#include <QHeaderView>
#include <QStringLiteral>
#include <QRegularExpression>
#include <QFile>
#include <QTextStream>
#include <QShortcut>
#include "csv.h"
#include "filmlistmodel.h"
#include "filmfilterproxymodel.h"
#include "addfilmwidget.h"

void saveFilmsToFile(QFile* file, const std::vector<FilmListModel::Film>& films);

QString joinVector(const std::vector<QString>& vec, bool spaceAfterComma) {
    QString result;
    for (auto& str : vec) {
        result += str + (spaceAfterComma ? ", " : ",");
    }
    result.chop(spaceAfterComma ? 2 : 1);
    return result;
}

void showAboutDialog() {
    QMessageBox msg;
    msg.setWindowTitle("About");
    msg.setIconPixmap(QPixmap(":/resources/crewmate.png"));
    msg.setText("Qt project application. No rights reserved. I hate it.\n\nUpd: I hate it less.");
    msg.setStandardButtons(QMessageBox::Close);
    msg.setDefaultButton(QMessageBox::Close);
    msg.exec();
}

void showFiltersDialog() {
    QMessageBox msg;
    msg.setIcon(QMessageBox::Icon::Information);
    msg.setWindowTitle("Filters");
    msg.setText("Filter: "
                "<rate|vote|year>:[<|<=|>|>=|=]<value>\n\n"
                "Example: year:2000 - show all films released in the year 2000.\n"
                "Note: Currently, only one filter per property is evaluated in a given query.");
    msg.setStandardButtons(QMessageBox::Close);
    msg.setDefaultButton(QMessageBox::Close);
    msg.exec();
}

void showCertificateLegendDialog() {
    QMessageBox msg;
    msg.setWindowTitle("Certificate legend");
    msg.setIcon(QMessageBox::Icon::Information);
    msg.setText("TV-Y: Designed to be appropriate for all children\n"
                "TV-Y7: Suitable for ages 7 and up\n"
                "G: Suitable for General Audiences\n"
                "TV-G: Suitable for General Audiences\n"
                "PG: Parental Guidance suggested\n"
                "TV-PG: Parental Guidance suggested\n"
                "PG-13: Parents strongly cautioned. May be Inappropriate for ages 12 and under.\n"
                "TV-14: Parents strongly cautioned. May not be suitable for ages 14 and under.\n"
                "R: Restricted. May be inappropriate for ages 17 and under.\n"
                "TV-MA: For Mature Audiences. May not be suitable for ages 17 and under.\n"
                "NC-17: Inappropriate for ages 17 and under\n");
    msg.setStandardButtons(QMessageBox::Close);
    msg.setDefaultButton(QMessageBox::Close);
    msg.exec();
}

void showErrorDialog(QString& msg) {
    QMessageBox dialog;
    dialog.setWindowTitle("Error");
    dialog.setIcon(QMessageBox::Icon::Critical);
    dialog.setText("Error: " + msg);
    dialog.setStandardButtons(QMessageBox::Ok);
    dialog.setDefaultButton(QMessageBox::Ok);
    dialog.exec();
}

void MainWindow::createAndOpenFile() {
    QString fileName = QFileDialog::getSaveFileName(this, "Create new file", "/", "*.csv");
    if(!fileName.isEmpty()) {
        QFile newFile(fileName);
        try {
            newFile.open(QIODevice::WriteOnly | QIODevice::Text);

            std::vector<FilmListModel::Film> emptyVector;
            saveFilmsToFile(&newFile, emptyVector);

            loadDataFromPath(fileName);
        } catch(std::exception ex) {
            auto msg = QString("Error writing to new file: ") + ex.what();
            showErrorDialog(msg);
        }
    }
}

void MainWindow::loadDataFromPath(QString& fileName) {
    auto filmListModel = new FilmListModel;
    try {
        filmListModel->loadFilms(fileName.toStdString());
    } catch(std::exception& ex) {
        auto msg = QString("Failed to read csv data: %0").arg(ex.what());
        showErrorDialog(msg);
        return;
    }

    auto filterModel = new FilmFilterProxyModel;
    filterModel->setSourceModel(filmListModel);
    filterModel->setFilterCaseSensitivity(Qt::CaseInsensitive);

    auto oldModel = ui->listView->model();
    delete oldModel;
    ui->listView->setModel(filterModel);
    ui->lineEdit->clear();

    delete this->fileHandle;

    QFile* file = new QFile(fileName);
    file->open(QIODevice::ReadWrite | QIODevice::Text);
    this->fileHandle = file;

    ui->actionSave->setEnabled(true);
    ui->actionSave_as->setEnabled(true);
    ui->listView->setEnabled(true);
    ui->lineEdit->setEnabled(true);
    ui->addFilmButton->setEnabled(true);
    ui->filmInfoWidget->setEnabled(true);
    ui->filmInfoWidget->verticalHeader()->setVisible(true);

    QString newTitle = QString("%0%1 (%1)").arg(DEFAULT_WINDOW_TITLE).arg(fileName);
    this->setWindowTitle(newTitle);
}

void MainWindow::openFile() {
    try {
        QString fileName = QFileDialog::getOpenFileName(this, "Open file", "/", "*.csv");
        qDebug() << fileName;

        if(fileName.isEmpty()) {
            return;
        }

        this->loadDataFromPath(fileName);

    } catch(std::exception& ex) {
        auto msg = QString::fromStdString(ex.what());
        showErrorDialog(msg);
    }
}

void saveFilmsToFile(QFile* file, const std::vector<FilmListModel::Film>& films) {
    if(file) {
        const char* columns[] = { CSV_COLUMN_NAMES };
        int rowLength = sizeof(columns)/sizeof(*columns);

        file->seek(0);
        file->resize(0);
        QTextStream output(file);

        for(int i = 0; i < rowLength ; i++) {
            output << columns[i];
            if(i + 1 != rowLength) {
                output << ",";
            }
        }
        output << "\n";

        for(auto& film : films) {
            output << '"' << film.name << '"' << ','
                   << film.year << ','
                   << film.rate << ','
                   << film.votes << ','
                   << '"' << joinVector(film.genres, false) << '"' << ','
                   << film.duration << ','
                   << film.type << ','
                   << film.certificate << ','
                   << film.nudity << ','
                   << film.violence
                   << "\n";
        }

        output.flush();

        qDebug() << "Saved file";
    } else {
        qDebug("File pointer is null");
    }
}

void MainWindow::saveFile() {
    qDebug() << "save";
    auto proxyModel = (FilmFilterProxyModel*)this->ui->listView->model();
    auto filmListModel = (FilmListModel*)proxyModel->sourceModel();

    QFile* file = this->fileHandle;
    if(file) {
        saveFilmsToFile(file, filmListModel->films);
    }

    QString title = this->windowTitle();
    if(title.startsWith('*')) {
        this->setWindowTitle(title.mid(1)); // remove asterisk
    }
}

void MainWindow::saveFileAs() {
    qDebug() << "save as";
    auto proxyModel = (FilmFilterProxyModel*)this->ui->listView->model();
    auto filmListModel = (FilmListModel*)proxyModel->sourceModel();

    QString fileName = QFileDialog::getSaveFileName(this, "Save file", "/", "*.csv");
    if(!fileName.isEmpty()) {
        QFile newFile(fileName);
        try {
            newFile.open(QIODevice::WriteOnly | QIODevice::Text);
            saveFilmsToFile(&newFile, filmListModel->films);
        } catch(std::exception ex) {
            auto msg = QString("Error opening file: ") + ex.what();
            showErrorDialog(msg);
        }
    }

    QString title = this->windowTitle();
    if(title.startsWith('*')) {
        this->setWindowTitle(title.mid(1)); // remove asterisk
    }
}

void MainWindow::addNewFilmEntry() {
    AddFilmWidget afw;
    int result = afw.exec();
    qDebug() << "Add result:" << result << afw.newFilm.name;

    if(result == AddFilmWidget::Accepted) {
        auto proxyModel = (FilmFilterProxyModel*)this->ui->listView->model();
        auto filmListModel = (FilmListModel*)proxyModel->sourceModel();

        qDebug() << &afw.newFilm;
        filmListModel->addFilm(afw.newFilm);
    }

    QString title = this->windowTitle();
    if(!title.startsWith('*')) {
        this->setWindowTitle(QString("*") + title); // add asterisk
    }
}

void MainWindow::editFilmEntry(int row) {
    auto proxyModel = (FilmFilterProxyModel*)this->ui->listView->model();
    auto filmListModel = (FilmListModel*)proxyModel->sourceModel();

    FilmListModel::Film film = filmListModel->getFilmData(row);
    AddFilmWidget afw(&film);
    afw.newFilm = film;

    int result = afw.exec();
    if(result == AddFilmWidget::Accepted) {
        filmListModel->changeFilm(row, afw.newFilm);
        qDebug() << "Edited film" << row;
    }

    QString title = this->windowTitle();
    if(!title.startsWith('*')) {
        this->setWindowTitle(QString("*") + title); // add asterisk
    }
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Set up menu bar //

    connect(ui->actionNew,          &QAction::triggered,   this, &MainWindow::createAndOpenFile);
    connect(ui->actionOpen,         &QAction::triggered,   this, &MainWindow::openFile);
    connect(ui->actionSave,         &QAction::triggered,   this, &MainWindow::saveFile);
    connect(ui->actionSave_as,      &QAction::triggered,   this, &MainWindow::saveFileAs);
    connect(ui->actionExit,         &QAction::triggered,   this, [](){ std::exit(0); });

    connect(ui->actionCertificates, &QAction::triggered,   this, &showCertificateLegendDialog);
    connect(ui->actionFilters,      &QAction::triggered,   this, &showFiltersDialog);
    connect(ui->actionAbout,        &QAction::triggered,   this, &showAboutDialog);

    ui->actionSave->setEnabled(false);
    ui->actionSave_as->setEnabled(false);

    // Set up film list //

    ui->listView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // Set up info panel //

    ui->filmInfoWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
}


MainWindow::~MainWindow()
{
    delete ui;
}

void updateFilmInfoWidget(QTableWidget* widget, FilmListModel::Film& film) {
    auto model = widget->model();

    model->setData(model->index(0, 0), film.name, Qt::ToolTipRole);
    model->setData(model->index(0, 0), film.name, Qt::DisplayRole);
    model->setData(model->index(1, 0), film.year, Qt::ToolTipRole);
    model->setData(model->index(1, 0), film.year, Qt::DisplayRole);
    model->setData(model->index(2, 0), film.rate, Qt::ToolTipRole);
    model->setData(model->index(2, 0), film.rate, Qt::DisplayRole);
    model->setData(model->index(3, 0), film.votes, Qt::ToolTipRole);
    model->setData(model->index(3, 0), film.votes, Qt::DisplayRole);
    model->setData(model->index(4, 0), joinVector(film.genres, true), Qt::ToolTipRole);
    model->setData(model->index(4, 0), joinVector(film.genres, true), Qt::DisplayRole);
    model->setData(model->index(5, 0), QString("%0 minutes").arg(film.duration), Qt::ToolTipRole);
    model->setData(model->index(5, 0), QString("%0 minutes").arg(film.duration), Qt::DisplayRole);
    model->setData(model->index(6, 0), film.type, Qt::ToolTipRole);
    model->setData(model->index(6, 0), film.type, Qt::DisplayRole);
    model->setData(model->index(7, 0), film.certificate, Qt::ToolTipRole);
    model->setData(model->index(7, 0), film.certificate, Qt::DisplayRole);
    model->setData(model->index(8, 0), film.nudity, Qt::ToolTipRole);
    model->setData(model->index(8, 0), film.nudity, Qt::DisplayRole);
    model->setData(model->index(9, 0), film.violence, Qt::ToolTipRole);
    model->setData(model->index(9, 0), film.violence, Qt::DisplayRole);
}

void MainWindow::on_lineEdit_textChanged(const QString &text)
{
    auto proxyModel = (FilmFilterProxyModel*)this->ui->listView->model();
    if(!proxyModel) {
        return;
    }
//    auto filmListModel = (FilmListModel*)proxyModel->sourceModel();

    QString queryNoFilters = proxyModel->updateFiltersFromQuery(text);

    qDebug() << "No filters:" << queryNoFilters;

    proxyModel->setFilterFixedString(queryNoFilters.trimmed());
}

void MainWindow::on_listView_clicked(const QModelIndex &index)
{
    auto proxyModel = (FilmFilterProxyModel*)this->ui->listView->model();
    auto filmListModel = (FilmListModel*)proxyModel->sourceModel();

    auto mappedIndex = proxyModel->mapToSource(index);

    if(!mappedIndex.isValid()) {
        return;
    }

    auto film = filmListModel->getFilmData(mappedIndex.row());

    updateFilmInfoWidget(ui->filmInfoWidget, film);
}

void MainWindow::on_addFilmButton_clicked()
{
    this->addNewFilmEntry();
}


void MainWindow::on_listView_customContextMenuRequested(const QPoint &pos)
{
    auto proxyModel = (FilmFilterProxyModel*)this->ui->listView->model();
    auto filmListModel = (FilmListModel*)proxyModel->sourceModel();

    QPoint globalPos = this->ui->listView->mapToGlobal(pos);
    auto idx = proxyModel->mapToSource(this->ui->listView->indexAt(pos));

    QMenu cm(this->ui->listView);
    QAction* editAction = cm.addAction("Edit");
    QAction* deleteAction = cm.addAction("Delete");

    QAction* selected = cm.exec(globalPos);

    if(selected == editAction) {
        this->editFilmEntry(idx.row());
    }
    if(selected == deleteAction) {
        qDebug() << idx.row();
        filmListModel->removeFilm(idx.row());

        QString title = this->windowTitle();
        if(!title.startsWith('*')) {
            this->setWindowTitle(QString("*") + title); // add asterisk
        }
    }
}

