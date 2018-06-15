#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#include <QObject>
#include <QDebug>
#include <libopenmpt/libopenmpt_ext.hpp>
#include <fstream>


class SoundManager
{
private:
    openmpt::module_ext *m_modFile = nullptr;
//    openmpt_module *m_modFile = nullptr;

public:
    SoundManager();
    void pause();
    void play();
    void stop();
    bool loadFile(QString filePath);
};

#endif // SOUNDMANAGER_H
