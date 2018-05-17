#include "utilities.h"
#include <libopenmpt/libopenmpt.hpp>
#include <fstream>

Utilities::Utilities() {

}

void Utilities::traverseDirectories(const QString dirName) {
    QDirIterator *iterator = new QDirIterator(dirName, QDirIterator::Subdirectories);

    while (iterator->hasNext()) {
        QFileInfo *fileInfo = new QFileInfo(iterator->next());
        if (fileInfo->isFile()) {
            qDebug() << "File: " << fileInfo->absoluteFilePath();

            QString filePath = fileInfo->absoluteFilePath();
            const char *fileString = filePath.toUtf8();

            std::ifstream file(fileString, std::ios::binary);

            try {
                openmpt::module mod(file);
                qDebug() << "Good file " << fileString;
            }
            catch(openmpt::exception *e) {
                qDebug() << "BAD FILE " << fileString;
            }




//            (file);


        }
        // Todo: Connect w/ testing audio
    }

}
