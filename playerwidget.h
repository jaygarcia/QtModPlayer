#ifndef PLAYERWIDGET_H
#define PLAYERWIDGET_H

#include <QObject>
#include <QWidget>
#include <QtWidgets>
#include <QtAwesome.h>

class PlayerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PlayerWidget(QWidget *parent = nullptr);

    QLabel *songLabel;
    QtAwesome *qtAwesome;

    void configure();
    void addChildren();

    QWidget *buildSongInformationUI();
    QWidget *buildPlayerControlUI();
    QPushButton *buildButton(const char *iconType);
    QWidget *buildBottomControlUI();

signals:

public slots:
};

#endif // PLAYERWIDGET_H
