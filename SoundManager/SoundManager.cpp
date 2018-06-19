#include "SoundManager.h"

openmpt::module_ext *modFile = nullptr;
QMutex *mutex;
bool isLoaded = false;

//QList<float> bufferLeft;
//QList<float> bufferRight;

#define bufferSize 1024

int playMode = 0; // 0 stopped, 1 playing, 2 paused;

typedef struct {
    float left_phase;
    float right_phase;
} paTestData;
static paTestData data;

static int currentModData[4];

QJsonObject *modInfoJsonObject;


static int static_paStreamCallback(const void *inputBuffer,
                             void *outputBuffer,
                             unsigned long framesPerBuffer,
                             const PaStreamCallbackTimeInfo* timeInfo,
                             PaStreamCallbackFlags statusFlags,
                             void *userData )  {

    /* Prevent unused variable warning. */
    (void) inputBuffer;
    (void) timeInfo;
    (void) inputBuffer;
    (void) userData;

    if (statusFlags == paOutputUnderflow) {
        printf("Underflow!!\n");
        fflush(stdout);
    }

    float *out = (float*)outputBuffer;

    if (modFile != nullptr) {

        modFile->read_interleaved_stereo(44100, framesPerBuffer, out);

        mutex->lock();

        currentModData[0] = (int)modFile->get_current_order();
        currentModData[1] = (int)modFile->get_current_pattern();
        currentModData[2] = (int)modFile->get_current_row();
        currentModData[3] = (int)modFile->get_pattern_num_rows(currentModData[1]);
    }

    mutex->unlock();


    return 0;
}
PaStream *stream;

int initializePortAudio() {
    PaError err;


    err = Pa_Initialize();
    if( err != paNoError ) goto error;

    /* Open an audio I/O stream. */

    err = Pa_OpenDefaultStream( &stream,
                                0,          /* no input channels */
                                2,          /* stereo output */
                                paFloat32,  /* 32 bit floating point output */
                                44100,      // Sample Rate
                                bufferSize,        /* frames per buffer */
                                static_paStreamCallback,
                                &data );

    if( err != paNoError ) goto error;

    return err;

error:
    Pa_Terminate();
    fprintf(stderr, "An error occured while using the portaudio stream\n");
    fprintf(stderr, "Error number: %d\n", err);
    fprintf(stderr, "Error message: %s\n", Pa_GetErrorText(err));
    return err;
}

SoundManager::SoundManager(QObject *parent) : QObject(parent) {
    mutex = new QMutex();

    if (initializePortAudio() > 0) {
        return;
    }
}


void SoundManager::run() {

    int currentOrder = -1,
        currentPattern = -1,
        currentRow = -1;


    mutex->lock();

    currentModData[0] = -1;
    currentModData[1] = -1;
    currentModData[2] = -1;
    currentModData[3] = -1;

    mutex->unlock();

    while (playMode > 0 && ! QThread::currentThread()->isInterruptionRequested() && modFile) {

        mutex->lock();

        if (currentOrder != currentModData[0] || currentPattern != currentModData[1] || currentRow != currentModData[2]) {

            currentOrder   = currentModData[0];
            currentPattern = currentModData[1];
            currentRow     = currentModData[2];

            modInfoJsonObject->insert("current_order", currentOrder);
            modInfoJsonObject->insert("current_pattern", currentPattern);
            modInfoJsonObject->insert("current_row", currentRow);

            emit modPositionChanged(modInfoJsonObject);
        }

        mutex->unlock();
        this->thread()->msleep(50);
    }

    this->stop();
}


QJsonObject *SoundManager::loadFile(QJsonObject *fileObject) {
    this->stop();

    mutex->lock();

    QString filePath =  fileObject->value("full_path").toString();
    const char *fileString = filePath.toUtf8();

    std::ifstream file(fileString, std::ios::binary);
    modInfoJsonObject = new QJsonObject();

    int goodLoad = openmpt::probe_file_header(openmpt::probe_file_header_flags_default, file);

    if (goodLoad == openmpt::probe_file_header_result_success) {

        modFile = new openmpt::module_ext(file);

        // Todo:: Setup as a configuration option from the UI
        modFile->set_repeat_count(999999);

        QString songName = QString::fromUtf8(modFile->get_metadata("title").c_str());
        qDebug() << "Song Name" << songName;
        modInfoJsonObject->insert("song_name", songName);

        modInfoJsonObject->insert("num_patterns", modFile->get_num_patterns());
        modInfoJsonObject->insert("num_channels",  modFile->get_num_channels());
        modInfoJsonObject->insert("num_samples",  modFile->get_num_samples());
        modInfoJsonObject->insert("num_instruments",  modFile->get_num_instruments());
        modInfoJsonObject->insert("speed",  modFile->get_current_speed());
        modInfoJsonObject->insert("bpm",  modFile->get_current_tempo());
        modInfoJsonObject->insert("length",  modFile->get_duration_seconds());
        modInfoJsonObject->insert("current_pattern",  modFile->get_current_pattern());
        modInfoJsonObject->insert("current_order", modFile->get_current_order());
        modInfoJsonObject->insert("num_orders",  modFile->get_num_orders() - 1);


        modInfoJsonObject->insert("artist",  modFile->get_metadata("artist").c_str());
        modInfoJsonObject->insert("type",  modFile->get_metadata("type").c_str());
        modInfoJsonObject->insert("type_long",  modFile->get_metadata("type_long").c_str());
        modInfoJsonObject->insert("container",  modFile->get_metadata("container").c_str());
        modInfoJsonObject->insert("container_long",  modFile->get_metadata("container_long").c_str());
        modInfoJsonObject->insert("tracker",  modFile->get_metadata("tracker").c_str());
        modInfoJsonObject->insert("title",  modFile->get_metadata("title").c_str());
        modInfoJsonObject->insert("date",  modFile->get_metadata("date").c_str());
        modInfoJsonObject->insert("message",  modFile->get_metadata("message").c_str());
        modInfoJsonObject->insert("warnings",  modFile->get_metadata("warnings").c_str());
    }
    else {
        // Mod file did not load
        modFile = nullptr;
    }

    mutex->unlock();
    this->pause();

    return modInfoJsonObject;
}

QJsonObject *SoundManager::getModMetaData() {
    return modInfoJsonObject;
}

void SoundManager::setModPosition(int order) {
    if (! modFile) {
        return;
    }

    mutex->lock();

    modFile->set_position_order_row(order, 0);


    mutex->unlock();
};

void SoundManager::pause() {
    qDebug() << Q_FUNC_INFO;
    mutex->lock();
    playMode = 2;
    Pa_StopStream(stream);

    mutex->unlock();
}

void SoundManager::play() {
    qDebug() << Q_FUNC_INFO;
    mutex->lock();
    playMode = 1;

    Pa_StartStream(stream);

    mutex->unlock();
}

void SoundManager::stop() {
    qDebug() << Q_FUNC_INFO;

    mutex->lock();
    // Todo : set mod to zero position
    playMode = 0;
    Pa_StopStream(stream);


    mutex->unlock();
}

/* From OMPT.m

- (NSDictionary *)getAllPatterns:(NSString *)path {
     FILE *file =  fopen([path cStringUsingEncoding:NSASCIIStringEncoding], "rb");

    if (file == NULL) {
        return nil;
    }

    fseek(file, 0L, SEEK_END);
    long loadedFileSize = ftell(file);

    rewind(file);
    char * loadedFileData = (char*) malloc(loadedFileSize);

    fread(loadedFileData, loadedFileSize, sizeof(char), file);

    openmpt_module * myLoadedMpFile = 0;
    myLoadedMpFile = openmpt_module_create_from_memory(loadedFileData, loadedFileSize, NULL, NULL, NULL);

    fclose(file);

    if (!myLoadedMpFile) {
        return nil;
    }

    NSDictionary *retObj = [self getPatterns:myLoadedMpFile];

    openmpt_module_destroy(myLoadedMpFile);
    free(loadedFileData);

    return retObj;
}



- (NSMutableDictionary *) getPatterns:(openmpt_module *)modFile {
    NSMutableDictionary *data = [[NSMutableDictionary alloc] init];

    int numOrders   = openmpt_module_get_num_orders(modFile),
        numChannels = openmpt_module_get_num_channels(modFile),
        orderNum    = 0,
        chanWidth   = 13,
        orderPattern,
        patternNumRows;


    for (; orderNum < numOrders; orderNum++) {
        orderPattern   = openmpt_module_get_order_pattern(modFile, orderNum);
        patternNumRows = openmpt_module_get_pattern_num_rows(modFile, orderPattern);

        NSString *patternNumber = [NSString  stringWithFormat:@"%i", orderPattern];

        // We've already got this pattern in Memory. So, let's just skip it.
        NSMutableArray *alreadyGotPattern = [data valueForKey:patternNumber] ;

        if (patternNumRows > 0 && alreadyGotPattern == nil) {
//            printf("Order# %i :: Pattern# %i :: numRows# %i\n", orderNum, orderPattern, patternNumRows);

            NSMutableArray *patternRows = [[NSMutableArray alloc] init];
//            NSString *lessThanSixteen = @"0%X|",
//                     *moreThanSixteen = @"%X|";

            for (int rowNumber = 0; rowNumber < patternNumRows; rowNumber++) {
                NSString *rowNsString = @"";


                for (int chanNumber = 0; chanNumber < numChannels; chanNumber++) {
                    const char * rowInfo = openmpt_module_format_pattern_row_channel(modFile, orderPattern, rowNumber, chanNumber, chanWidth, 1);

                    rowNsString = [rowNsString stringByAppendingString:[NSString stringWithCString:rowInfo]];

                    if (chanNumber < numChannels - 1) {
                        rowNsString = [rowNsString stringByAppendingString:@"|"];
                    }

                }

              [patternRows addObject:rowNsString];

            }

            [data setValue:patternRows forKey:patternNumber];

        }


    }


    return data;
}


- (NSArray *) getPatternData:(NSNumber *)patternNumber {

    openmpt_module *modFile = self.mod;


    int numChannels = openmpt_module_get_num_channels(modFile),
        orderNum  = 0,
        chanWidth   = 13,
        orderPattern,
        patternNumRows;


    NSMutableArray *patternRows = [[NSMutableArray alloc] init];


    orderPattern   = openmpt_module_get_order_pattern(modFile, orderNum);
    patternNumRows = openmpt_module_get_pattern_num_rows(modFile, [patternNumber intValue]);

    // We've already got this pattern in Memory. So, let's just skip it.

    for (int rowNumber = 0; rowNumber < patternNumRows; rowNumber++) {
        NSString *rowNsString = @"";

        for (int chanNumber = 0; chanNumber < numChannels; chanNumber++) {
            const char * rowInfo = openmpt_module_format_pattern_row_channel(modFile, orderPattern, rowNumber, chanNumber, chanWidth, 1);

            rowNsString = [rowNsString stringByAppendingString:[NSString stringWithCString:rowInfo]];

            if (chanNumber < numChannels - 1) {
                rowNsString = [rowNsString stringByAppendingString:@"|"];
            }

        }

        [patternRows addObject:rowNsString];

    }




    return patternRows;


}




*/
