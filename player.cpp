/**
  * @file player.cpp
  * @brief audio and video player
  * @author Roman Safin
  */
#include "player.h"
#include "ui_player.h"
#include <QFileDialog>
#include <QFileInfo>
#include <QDir>
#include <QTime>
#include <QSpinBox>
#include <QDataStream>
#include <QDebug>

Player::Player(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Player)
{
    ui->setupUi(this);
    player = new QMediaPlayer(this);
    playlist = new QMediaPlaylist(player);

    this->readMap();
    this->setButtonStyle();
    this->setPlaylist();
    this->videoWidget();
    this->setDuration();

    connect(ui->previous_button, &QToolButton::clicked,playlist, &QMediaPlaylist::previous);
    connect(ui->next_button, &QToolButton::clicked, this, &Player::next);
    connect(ui->play_button, &QToolButton::clicked, this, &Player::play);
    connect(ui->pause_button, &QToolButton::clicked,this, &Player::pause);
    connect(ui->stop_button, &QToolButton::clicked, this, &Player::stop);
    connect(ui->add_button, &QToolButton::clicked,  this, &Player::addFile);
}

Player::~Player()
{
    this->savePosition();
    this->saveMap();
    delete ui;
    delete model;
    delete playlist;
    delete player;
}



void Player::setSound()
{
    player->setVolume(sound);
    ui->spinBox->setValue(50);
}

void Player::changeVolume(QString sound)
{
    if(sound.toInt()!=0)
        this->sound = sound.toInt();
    player->setVolume(sound.toInt());
}

void Player::Mute()
{
    if(ui->spinBox->value() != 0){
        ui->spinBox->setValue(0);
        player->setMuted(true);
    } else {
        ui->spinBox->setValue(this->sound);
        player->setMuted(false);
    }

}

void Player::setDuration()
{
    ui->timeSlider->setRange(0, player->duration()/1000);
    connect(ui->timeSlider, &QSlider::sliderMoved, this, &Player::seek);
    connect(player, &QMediaPlayer::durationChanged, this, &Player::changeDuration);
    connect(player, &QMediaPlayer::positionChanged, this, &Player::changePosition);
}

void Player::changeDuration(qint64 duration)
{
    ui->timeSlider->setMaximum(duration/1000);
}

void Player::changePosition(qint64 progress)
{
    if(!ui->timeSlider->isSliderDown())
        ui->timeSlider->setValue(progress/1000);
}


void Player::seek(int seconds)
{
    player->setPosition(seconds*1000);
}

void Player::savePosition()
{
    map.insert(playlist->currentMedia().canonicalRequest().url(), player->position());
}

void Player::readPosition()
{
    foreach(QUrl key, map.keys()){
        if(key.toString().compare(playlist->currentMedia().canonicalRequest().url().toString()) == 0){
            qint64 value = map.value(key);
            player->setPosition(value);
        }
    }
}

void Player::saveMap()
{
    QFile f ("position.txt");
    if (!f.open(QIODevice::WriteOnly)) return;
    QDataStream str(&f);
    str << map;
}

void Player::readMap()
{
    QFile f ("position.txt");
    if (!f.open(QIODevice::ReadOnly)) return;
    QDataStream str(&f);
    map.clear();
    str >> map;
    f.remove();
}

void Player::play()
{
    player->play();
    ui->play_button->setEnabled(false);
    ui->pause_button->setEnabled(true);
    ui->stop_button->setEnabled(true);
    readPosition();
}

void Player::next()
{
    savePosition();
    playlist->next();
    player->play();
}

void Player::pause()
{
    savePosition();
    ui->pause_button->setEnabled(false);
    ui->play_button->setEnabled(true);
    player->pause();
}

void Player::stop()
{
    ui->stop_button->setEnabled(false);
    ui->play_button->setEnabled(true);
    map.remove(playlist->currentMedia().canonicalRequest().url());
    player->stop();
}
void Player::setButtonStyle()
{
      ui->play_button->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    ui->pause_button->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
    ui->next_button->setIcon(style()->standardIcon(QStyle::SP_MediaSkipForward));
    ui->previous_button->setIcon(style()->standardIcon(QStyle::SP_MediaSkipBackward));
}

void Player::setPlaylist()
{
    model = new QStandardItemModel(this);
    ui->tableView->setModel(model);
    model->setHorizontalHeaderLabels(QStringList()<< tr("File")
                                     << tr("File Path"));
    ui->tableView->hideColumn(1);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->horizontalHeader()->setStretchLastSection(true);

    player->setPlaylist(playlist);
    this->setSound();
    playlist->setPlaybackMode(QMediaPlaylist::Loop);


    connect(ui->tableView, &QTableView::doubleClicked,
            [this](const QModelIndex &index){
            playlist->setCurrentIndex(index.row());
            player->play();
            });
    connect(playlist, &QMediaPlaylist::currentIndexChanged,
            [this](int index){
            ui->currentTrack->setText(model->data(model->index(index, 0)).toString());
            });
}

void Player::videoWidget()
{
    widget = new QVideoWidget(ui->widget);
    player->setVideoOutput(widget);
    widget->resize(ui->widget->size());
    widget->show();
}

void Player::addFile()
{
    QStringList files = QFileDialog::getOpenFileNames(this, tr("Open files"), QString(), tr("Audio Files (*.mp3 *.wav);; ""Video files (*.avi)"));
    foreach (QString filePath, files) {
        QList<QStandardItem *> items;
        items.append(new QStandardItem(QDir(filePath).dirName()));
        items.append(new QStandardItem(filePath));
        model->appendRow(items);
        playlist->addMedia(QUrl::fromLocalFile(filePath));
        widget = new QVideoWidget;
    }
}

