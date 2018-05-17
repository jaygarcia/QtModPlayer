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
//            qDebug() << "File: " << fileInfo->absoluteFilePath();

            QString filePath = fileInfo->absoluteFilePath();
            const char *fileString = filePath.toUtf8();

            std::ifstream file(fileString, std::ios::binary);

            int goodLoad = openmpt::probe_file_header(openmpt::probe_file_header_flags_default, file);
            if (goodLoad == openmpt::probe_file_header_result_success) {
//                qDebug() << "Good file " << fileString;
            }
            else {
//                qDebug() << "BAD FILE " << fileString;

                printf("Bad file :: %s\n", fileString);
                fflush(stdout);
            }



//            (file);


        }
        // Todo: Connect w/ testing audio
    }

}
