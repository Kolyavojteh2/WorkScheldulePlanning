#ifndef XML_PARSING_H
#define XML_PARSING_H

#include "XML_Parsing_global.h"
#include <QString>
#include <QList>
#include "Worker.h"

extern "C"
Worker read_WorkerInfoFromFile(const QString filename);

extern "C"
void write_WorkerInfoToFile(const QString filename, const Worker info);

#endif // XML_PARSING_H
