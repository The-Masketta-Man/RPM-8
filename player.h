/**
  * @file player.h
  * @brief audio and video player
  * @author Roman Safin
  */
#ifndef PLAYER_H
#define PLAYER_H

#include <QWidget>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QStandardItemModel>
#include <QVideoWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class Player; }
QT_END_NAMESPACE

class Player : public QWidget
{
    Q_OBJECT

public:
    Player(QWidget *parent = nullptr);
    ~Player();

private:
    Ui::Player *ui;
    QStandardItemModel *model;
    QVideoWidget *widget;
    QMediaPlayer *player;
    QMediaPlaylist *playlist;
    int sound = 50;
    QMap <QUrl, qint64> map;

private:
    /**
     * @brief set sound
     */
    void setSound();
    /**
     * @brief set duration
     */
    void setDuration();
    /**
     * @brief save position to map
     */
    void savePosition();
    /**
     * @brief read position from map
     */
    void readPosition();
    /**
     * @brief save map to file
     */
    void saveMap();
    /**
     * @brief read map from file
     */
    void readMap();
    /**
     * @brief set button style
     */
    void setButtonStyle();
    /**
     * @brief set playlist
     */
    void setPlaylist();
    /**
     * @brief set video widget
     */
    void videoWidget();


private slots:
    /**
     * @brief play file
     */
    void play();
    /**
     * @brief next file
     */
    void next();
    /**
     * @brief pause file
     */
    void pause();
    /**
     * @brief stop file
     */
    void stop();
    /**
     * @brief add file
     */
    void addFile();
    /**
     * @brief change volume
     * @param volume
     */
    void changeVolume(QString volume);
    /**
     * @brief soundMute
     */
    void Mute();
    /**
     * @brief change duration
     * @param duration
     */
    void changeDuration(qint64 duration);
    /**
     * @brief positionChanged
     * @param progress
     */
    void changePosition(qint64 progress);
    /**
     * @brief seek
     * @param seconds
     */
    void seek(int seconds);
    /**
     * @brief updateDurationInfo
     * @param currentInfo
     */
};
#endif // PLAYER_H
