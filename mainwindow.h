#pragma once
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

#define DEFAULT_WINDOW_TITLE "Film viewer"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public:
    void addNewFilmEntry();
    void editFilmEntry(int row);

private slots:
    void createAndOpenFile();
    void openFile();
    void loadDataFromPath(QString& path);
    void saveFile();
    void saveFileAs();

    void on_lineEdit_textChanged(const QString &arg1);

    void on_listView_clicked(const QModelIndex &index);

    void on_addFilmButton_clicked();

    void on_listView_customContextMenuRequested(const QPoint &pos);

private:
    Ui::MainWindow *ui;

    QFile* fileHandle = nullptr;
};

#endif // MAINWINDOW_H
