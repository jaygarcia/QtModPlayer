#include "utilities.h"
#include <libopenmpt/libopenmpt.hpp>
#include <fstream>

Utilities::Utilities() {

}

// Todo:
//    - Move to separate thread.
//    - Add to user db?
//    - Add to playlist dataview
void Utilities::traverseDirectoriesAndTestFiles(const QString dirName) {
    QDirIterator *iterator = new QDirIterator(dirName, QDirIterator::Subdirectories);

    QVector<QString> badFiles;
    QVector<QString> goodFiles;

    int64_t totalFiles = 0;



    while (iterator->hasNext()) {
        totalFiles++;

        QFileInfo *fileInfo = new QFileInfo(iterator->next());

        if (fileInfo->isFile()) {

            QString filePath = fileInfo->absoluteFilePath();
            const char *fileString = filePath.toUtf8();
//            const char *fileName   = fileInfo->fileName().toUtf8();

            std::ifstream file(fileString, std::ios::binary);

            int goodLoad = openmpt::probe_file_header(openmpt::probe_file_header_flags_default, file);
            if (goodLoad == openmpt::probe_file_header_result_failure) {
                badFiles.push_back(filePath);
            }

            if (goodLoad == openmpt::probe_file_header_result_success) {
                goodFiles.push_back(filePath);
            }
        }

        // Todo: Connect w/ testing audio
    }

    qDebug() << "Total ALL files" << totalFiles;

    qDebug() << "Total good files" << goodFiles.count();
//    for (int i = 0; i < badFiles.size(); ++i) {
//        qDebug() << "Bad file :: " << badFiles.at(i);
//    }
    qDebug() << "Total bad files" << badFiles.count();  // Could be done via a simple int++

}
