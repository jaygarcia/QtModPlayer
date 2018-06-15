#include "SoundManager.h"

SoundManager::SoundManager()
{

}


bool SoundManager::loadFile(QString filePath) {
    const char *fileString = filePath.toUtf8();

    std::ifstream file(fileString, std::ios::binary);

    int goodLoad = openmpt::probe_file_header(openmpt::probe_file_header_flags_default, file);

    if (goodLoad == openmpt::probe_file_header_result_success) {

        m_modFile = new openmpt::module_ext(file);


        QString songName = QString::fromUtf8(m_modFile->get_metadata("title").c_str());

        qDebug() << "Loaded song " << songName << "from file" << fileString;
        return true;
    }

    return false;
}
