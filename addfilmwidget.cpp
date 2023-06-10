#include "addfilmwidget.h"
#include "ui_addfilmwidget.h"
#include "filmlistmodel.h"
#include <QMessageBox>
#include <QRegularExpressionValidator>
#include <QRandomGenerator>
#include <cmath>

void addItemToListWidget(QListWidget* w, QString text) {
    QListWidgetItem* item = new QListWidgetItem(w);
    item->setCheckState(Qt::Unchecked);
    item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
    item->setText(QCoreApplication::translate("AddFilmWidget", text.toStdString().c_str(), nullptr));

//    __qlistwidgetitem->setCheckState(Qt::Unchecked);
//    __qlistwidgetitem->setFlags(Qt::ItemIsDragEnabled|Qt::ItemIsUserCheckable|Qt::ItemIsEnabled);
//    QListWidgetItem *__qlistwidgetitem1 = new QListWidgetItem(genreListWidget);
//    __qlistwidgetitem1->setCheckState(Qt::Unchecked);
//    QListWidgetItem *__qlistwidgetitem2 = new QListWidgetItem(genreListWidget);
//    __qlistwidgetitem2->setCheckState(Qt::Unchecked);
//    genreListWidget->setObjectName("genreListWidget");
//    QSizePolicy sizePolicy2(QSizePolicy::Minimum, QSizePolicy::Expanding);
//    sizePolicy2.setHorizontalStretch(0);
//    sizePolicy2.setVerticalStretch(0);
//    sizePolicy2.setHeightForWidth(genreListWidget->sizePolicy().hasHeightForWidth());
//    genreListWidget->setSizePolicy(sizePolicy2);
//    genreListWidget->setMaximumSize(QSize(1212412, 16777215));
//    genreListWidget->setLayoutMode(QListView::SinglePass);
//    genreListWidget->setViewMode(QListView::ListMode);
//    genreListWidget->setSelectionRectVisible(true);

}

AddFilmWidget::AddFilmWidget(const FilmListModel::Film* base, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddFilmWidget)
{
    ui->setupUi(this);

    connect(ui->addButton,    &QPushButton::clicked, this, &AddFilmWidget::checkDataAndClose);
    connect(ui->cancelButton, &QPushButton::clicked, this, &AddFilmWidget::reject);

    ui->genreListWidget->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);

    addItemToListWidget(ui->genreListWidget, "Talk-Show");
    addItemToListWidget(ui->genreListWidget, "Thriller");
    addItemToListWidget(ui->genreListWidget, "News");
    addItemToListWidget(ui->genreListWidget, "Sport");
    addItemToListWidget(ui->genreListWidget, "Short");
    addItemToListWidget(ui->genreListWidget, "Comedy");
    addItemToListWidget(ui->genreListWidget, "Fantasy");
    addItemToListWidget(ui->genreListWidget, "Crime");
    addItemToListWidget(ui->genreListWidget, "Animation");
    addItemToListWidget(ui->genreListWidget, "Sci-Fi");
    addItemToListWidget(ui->genreListWidget, "Reality-TV");
    addItemToListWidget(ui->genreListWidget, "Biography");
    addItemToListWidget(ui->genreListWidget, "Adventure");
    addItemToListWidget(ui->genreListWidget, "Drama");
    addItemToListWidget(ui->genreListWidget, "Romance");
    addItemToListWidget(ui->genreListWidget, "Mystery");
    addItemToListWidget(ui->genreListWidget, "Musical");
    addItemToListWidget(ui->genreListWidget, "Action");
    addItemToListWidget(ui->genreListWidget, "Family");
    addItemToListWidget(ui->genreListWidget, "Western");
    addItemToListWidget(ui->genreListWidget, "Music");
    addItemToListWidget(ui->genreListWidget, "Documentary");
    addItemToListWidget(ui->genreListWidget, "Game-Show");
    addItemToListWidget(ui->genreListWidget, "Horror");
    addItemToListWidget(ui->genreListWidget, "War");
    addItemToListWidget(ui->genreListWidget, "Film-Noir");
    addItemToListWidget(ui->genreListWidget, "History");

    ui->yearEdit->setValidator(new QRegularExpressionValidator(QRegularExpression("[0-9]+")));
    auto dv = new QDoubleValidator(0, 10, 1);
    dv->setNotation(QDoubleValidator::StandardNotation);
    ui->rateEdit->setValidator(dv);
    ui->votesEdit->setValidator(new QRegularExpressionValidator(QRegularExpression("[0-9]+")));
    ui->durationEdit->setValidator(new QRegularExpressionValidator(QRegularExpression("[0-9]+")));

    connect(ui->nameEdit,          &QLineEdit::textEdited, this, &AddFilmWidget::updateAddButtonLockState);
    connect(ui->yearEdit,          &QLineEdit::textEdited, this, &AddFilmWidget::updateAddButtonLockState);
    connect(ui->rateEdit,          &QLineEdit::textEdited, this, &AddFilmWidget::updateAddButtonLockState);
    connect(ui->votesEdit,         &QLineEdit::textEdited, this, &AddFilmWidget::updateAddButtonLockState);
    connect(ui->genreListWidget,   &QListWidget::itemClicked, this, &AddFilmWidget::updateAddButtonLockState);
    connect(ui->durationEdit,      &QLineEdit::textEdited, this, &AddFilmWidget::updateAddButtonLockState);
    connect(ui->typeSelect,        &QComboBox::currentTextChanged, this, &AddFilmWidget::updateAddButtonLockState);
    connect(ui->certificateSelect, &QComboBox::currentTextChanged, this, &AddFilmWidget::updateAddButtonLockState);
    connect(ui->nuditySelect,      &QComboBox::currentTextChanged, this, &AddFilmWidget::updateAddButtonLockState);
    connect(ui->violenceSelect,    &QComboBox::currentTextChanged, this, &AddFilmWidget::updateAddButtonLockState);

    if(base) {
        this->newFilm = *base;
        this->updateAddButtonLockState();

        qDebug() << "Use custom" << base;
        ui->nameEdit->setText(base->name);
        qDebug() << "1";
        ui->yearEdit->setText(QString::number(base->year));
        qDebug() << "1";
        ui->rateEdit->setText(QString::number(base->rate));
        qDebug() << "1";
        ui->votesEdit->setText(QString::number(base->votes));
        qDebug() << "1";
        auto gl = ui->genreListWidget;
        for(int i = 0; i < gl->count(); i++) {
            auto item = gl->item(i);
            for(const QString& genreInBase : base->genres) {
                if(genreInBase == item->text()) {
                    item->setCheckState(Qt::Checked);
                }
            }
        }
        qDebug() << "1";
        ui->durationEdit->setText(QString::number(base->duration));
        qDebug() << "1";
        ui->typeSelect->setCurrentText(base->type);
        qDebug() << "1";
        ui->certificateSelect->setCurrentText(base->certificate);
        qDebug() << "1";
        ui->nuditySelect->setCurrentText(base->nudity);
        qDebug() << "1";
        ui->violenceSelect->setCurrentText(base->nudity);
        qDebug() << "1";
        qDebug() << "End custom" << base;
    }

}

void AddFilmWidget::updateAddButtonLockState() {
    this->ui->addButton->setEnabled(this->checkDataValid());
}

void showBadFormatInfo() {
    QMessageBox msg;
    msg.setWindowTitle("Bad format");
    msg.setIcon(QMessageBox::Icon::Critical);
    msg.setText("The following fields are incorrect:\ntodo");
    msg.setStandardButtons(QMessageBox::Close);
    msg.setDefaultButton(QMessageBox::Close);
    msg.exec();
}

bool AddFilmWidget::checkDataValid() const {
    bool b1 = false;
    bool b2 = false;
    bool b3 = false;
    bool b4 = false;
    auto name        = ui->nameEdit->text();
    auto year        = ui->yearEdit->text().toInt(&b1);
    auto rate        = ui->rateEdit->text().toDouble(&b2);
    b2 &= rate <= 10.0;
    auto votes       = ui->votesEdit->text().toInt(&b3);
    auto genreList   = ui->genreListWidget;
    auto duration    = ui->durationEdit->text().toInt(&b4);
    auto type        = ui->typeSelect->currentText();
    auto certificate = ui->certificateSelect->currentText();
    auto nudity      = ui->nuditySelect->currentText();
    auto violence    = ui->violenceSelect->currentText();

    int selectedGenres = 0;
    for(int i = 0; i < genreList->count(); i++) {
        auto item = genreList->item(i);
        if(item->checkState() == Qt::Checked) {
            selectedGenres += 1;
        }
    }

    return selectedGenres > 0 && b1 && b2 && b3 && b4
           && !name.isEmpty() && !type.isEmpty()
           && !certificate.isEmpty() && !nudity.isEmpty()
           && !violence.isEmpty();
}

void AddFilmWidget::checkDataAndClose() {
    FilmListModel::Film film;

    auto name        = ui->nameEdit->text();
    auto year        = ui->yearEdit->text().toInt();
    auto rate        = ui->rateEdit->text().toDouble();
    auto votes       = ui->votesEdit->text().toInt();
    auto genreList   = ui->genreListWidget;
    auto duration    = ui->durationEdit->text().toInt();
    auto type        = ui->typeSelect->currentText();
    auto certificate = ui->certificateSelect->currentText();
    auto nudity      = ui->nuditySelect->currentText();
    auto violence    = ui->violenceSelect->currentText();

    for(int i = 0; i < genreList->count(); i++) {
        auto item = genreList->item(i);
        if(item->checkState() == Qt::Checked) {
            film.genres.push_back(item->text());
        }
    }

    film.name        = name;
    film.year        = year;
    film.rate        = rate;
    film.votes       = votes;
    film.duration    = duration;
    film.type        = type;
    film.certificate = certificate;
    film.nudity      = nudity;
    film.violence    = violence;

    this->newFilm = film;

    this->accept();
}

AddFilmWidget::~AddFilmWidget()
{
    delete ui;
}

void AddFilmWidget::on_dial_sliderMoved(int position)
{
    auto genreList   = ui->genreListWidget;

    auto& rng = *QRandomGenerator::global();
    ui->nameEdit->setText(QString::number(rng.generate64(), 16));
    ui->yearEdit->setText(QString::number(rng.bounded(1900, 2023)));
    ui->rateEdit->setText(QString::number( std::floor(rng.bounded((double)10.0) * 10.0) / 10.0 ));
    ui->votesEdit->setText(QString::number(rng.bounded(100, 9000000)));

    int checked = 0;
    for(int i = 0; i < genreList->count(); i++) {
        auto item = genreList->item(i);
        if(rng.generateDouble() < 0.16) {
            checked += 1;
            item->setCheckState(Qt::Checked);
        }
    }
    if(checked == 0) {
        genreList->item(0)->setCheckState(Qt::Checked);
    }

    ui->durationEdit->setText(QString::number(rng.bounded(30, 210)));
    ui->typeSelect->setCurrentText(rng.generateDouble() < 0.5
                                       ? ui->typeSelect->itemText(0)
                                       : ui->typeSelect->itemText(1));

    ui->certificateSelect->setCurrentText(
        ui->certificateSelect->itemText(rng.bounded(0, ui->certificateSelect->count()))
    );
    ui->nuditySelect->setCurrentText(
        ui->nuditySelect->itemText(rng.bounded(0, ui->nuditySelect->count()))
    );
    ui->violenceSelect->setCurrentText(
        ui->violenceSelect->itemText(rng.bounded(0, ui->violenceSelect->count()))
    );

}
