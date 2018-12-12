#ifndef ELFILEPARSER_H
#define ELFILEPARSER_H

#include <QObject>
#include <QFileInfoList>
#include "eldatahandler.h"

class ELFileParser : public QObject
{
    Q_OBJECT
public:
    explicit ELFileParser(elDataHandler *eldh, QObject *parent = 0);

signals:
    void finished();
    void bytesToBeProcessed(int numberOfBytes);
    void bytesProcessed(int numberOfBytes);
    void noFile();

public slots:
    void runFilesDecoding();

private:

    elDataHandler *eldh;

    static const quint16 K_DATA_HEADER_0;
    static const quint8 K_DATA_HEADER_1;
    static const quint16 K_INFO_HEADER_0;
    static const quint16 K_INFO_HEADER_1;
    static const quint16 K_NULL;

    enum DataType
    {
        DataType_DATA_HEADER,
        DataType_INFO_HEADER,
        DataType_DATA,
        DataType_EOF,
        DataType_INVALIDE
    };

    void runBinaryDecoding(QString filePath);
    ELFileParser::DataType findDataType(QDataStream &inputStream);
    void decodeInfoFile(QDataStream &inputStream, QChar id);
    quint32 getNext3ByteAsInt(QDataStream &inputStream);
    double getTariff(QDataStream &inputStream);
    void computeTotalFilesSize(QFileInfoList files);

};

#endif // ELFILEPARSER_H
