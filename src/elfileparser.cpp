#include "elfileparser.h"
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QString>
#include <QDataStream>

const quint16 ELFileParser::K_DATA_HEADER_0 = 0xE0C5;
const quint8 ELFileParser::K_DATA_HEADER_1 = 0xEA;
const quint16 ELFileParser::K_INFO_HEADER_0 = 0x494E;
const quint16 ELFileParser::K_INFO_HEADER_1 = 0x464F;
const quint16 ELFileParser::K_NULL = 0xFF;

ELFileParser::ELFileParser(elDataHandler *eldhIn, QObject *parent) : QObject(parent)
{
    eldh = eldhIn;

}

void ELFileParser::runFilesDecoding()
{
    eldh->clear();
    QDir dir;
    qDebug() << eldh->dirPath;
    dir.setPath(eldh->dirPath);
    //ask for a list of files in the current directory, sorted by filename (should garantee that data whould be ordered)
    QFileInfoList files = dir.entryInfoList(QDir::Files,
                                            QDir::Name);

    if(files.size() == 0)
    {
        emit noFile();
    }
    else
    {
    computeTotalFilesSize(files);

    for(int k=0;k<files.size();k++)
    {
        qDebug() << files.at(k).filePath();
        runBinaryDecoding(files.at(k).filePath());
    }

    emit finished();
     }
}

void ELFileParser::computeTotalFilesSize(QFileInfoList files)
{
    int totalSize = 0;
    for(int k=0;k<files.size();k++)
    {
        totalSize = totalSize + files.at(k).size();
    }

    emit bytesToBeProcessed(totalSize);
}

void ELFileParser::runBinaryDecoding(QString filePath)
{
    QFile file(filePath);
       if (!file.open(QIODevice::ReadOnly))
       {
           qDebug() << "echec ouverture";
       }
       else
       {
            QDataStream out(&file);
            out.setByteOrder(QDataStream::BigEndian);

            quint16 voltage = 0;
            quint16 current = 0;
            quint8 cosphi = 0;
            quint8 year = 0;
            quint8 mounth = 0;
            quint8 day = 0;
            quint8 hour = 0;
            quint8 minute = 0;
            elDataHandler::DataInfo info;
            QDateTime datetime;
            DataType dataType = DataType_INVALIDE;

            dataType = findDataType(out);

            while((!out.atEnd()) && (dataType != DataType_EOF) )
            {
                if(dataType == DataType_DATA_HEADER)
                {
                    //We get date and time in order to date next datas
                    out >> mounth ;
                    out >> day ;
                    out >> year ;
                    out >> hour ;
                    out >> minute ;

                    datetime = QDateTime(QDate(year+2000, mounth, day), QTime(hour, minute));
                    eldh->setDateForNextDataset(datetime, QFileInfo(file.fileName()).fileName().at(0));

                    //We processed 1 byte for each Y/M/D/H/M => 5 bytes
                    //           + 3 bytes for header
                    emit bytesProcessed(8);
                }
                else if(dataType == DataType_DATA)
                {
                    //get voltage and current (16 bits each)
                    //    and cosphi (8 bits)
                    out >> voltage ;
                    out >> current;
                    out >> cosphi;
                    eldh->addDatatset(voltage,current,cosphi);

                    //We processed 2 byte for each voltage/current => 4 bytes
                    //           + 1 bytes for cosPhi
                    emit bytesProcessed(5);
                }
                else if(dataType == DataType_INFO_HEADER)
                {
                    //do something to manage INFO FILE !
                    decodeInfoFile(out, QFileInfo(file.fileName()).fileName().at(0));

                    //We have to break because the file is not totaly read by decodeInfoFile
                    break;
                }
                else
                {
                    qDebug() << "dataType NULL";
                }

                dataType = findDataType(out);
        }
    }
}

ELFileParser::DataType ELFileParser::findDataType(QDataStream &inputStream)
{
    DataType ret = DataType_INVALIDE;
    quint16 headerToCheck = 0;
    quint8  complementaryCheckData = 0;
    quint16 complementaryCheckInfo = 0;


    //set a return point
    inputStream.startTransaction();
    inputStream >> headerToCheck;

    if((QByteArray::number(headerToCheck).toUInt()) == K_DATA_HEADER_0)
    {
        //qDebug() << "header checked";
        //confirm that this is well a data header by checking next byte
        inputStream >> complementaryCheckData;
        if((QByteArray::number(complementaryCheckData).toUInt()) == K_DATA_HEADER_1)
        {
            //qDebug() << "header checked OK";
            ret = DataType_DATA_HEADER;
            inputStream.commitTransaction();
        }

    }
    else if((QByteArray::number(headerToCheck).toUInt()) == K_INFO_HEADER_0)
    {
        //confirm that this is well a info header by checking next byte
        inputStream >> complementaryCheckInfo;
        if((QByteArray::number(complementaryCheckInfo).toUInt()) == K_INFO_HEADER_1)
        {
            //qDebug() << "info checked OK";
            ret = DataType_INFO_HEADER;
            inputStream.commitTransaction();
        }

    }
    else if((QByteArray::number(headerToCheck).toUInt()) == K_NULL)
    {
        ret = DataType_EOF;
    }
    else
    {
        //else, we have a data field, we have read the voltage field
        ret = DataType_DATA;
        //rollback transaction in order to allow the decoding method to access voltage field
        inputStream.rollbackTransaction();
    }

    return ret;
}

void ELFileParser::decodeInfoFile(QDataStream &inputStream, QChar id)
{
    quint8 byte1 = 0;
    quint8 devId = 0 ;
    quint32 consumed = 0;
    double tariff1 = 0;
    double tariff2 = 0;
    quint8 year = 0;
    quint8 mounth = 0;
    quint8 day = 0;
    quint8 hour = 0;
    quint8 minute = 0;
    QDateTime datetime;


    QDataStream outputStream;
    outputStream.setByteOrder(QDataStream::BigEndian);
    QByteArray buffer;

    //get ID
    inputStream >> byte1;
    //get total consumed power
    consumed = getNext3ByteAsInt(inputStream);
    qDebug() << "total cons power" << consumed ;
    //get total record time => minutes
    consumed = getNext3ByteAsInt(inputStream);
    qDebug() << "total record time" << consumed/100 ;
    //get total on time => minutes
    consumed = getNext3ByteAsInt(inputStream);
    qDebug() << "total on time" << consumed/100 ;

    //get unit id => go to 84rd byte
    inputStream >> devId;
    qDebug() << "devId" << devId;

    //get tariff 1
    tariff1 = getTariff(inputStream);

    //get tariff 2
    tariff2 = getTariff(inputStream);
    qDebug() << "id tariff 1/2" << devId << tariff1 << tariff2 ;

    inputStream >> hour ;
    inputStream >> minute ;
    inputStream >> mounth ;
    inputStream >> day ;
    inputStream >> year ;

    datetime = QDateTime(QDate(year+2000, mounth, day), QTime(hour, minute));
    eldh->setDateForNextDataset(datetime, id);
    //qDebug() << "Date info file" << datetime.toString(Qt::ISODate);

    emit bytesProcessed(102);
}

double ELFileParser::getTariff(QDataStream &inputStream)
{
    double value = 0;
    quint8 tariffUnit = 0;
    quint8 tariffTenth = 0 ;
    quint8 tariffHundredths = 0;
    quint8 tariffThousandths = 0;
    inputStream >> tariffUnit;
    inputStream >> tariffTenth;
    inputStream >> tariffHundredths;
    inputStream >> tariffThousandths;

    value = tariffUnit + double(tariffTenth)/10 + double(tariffHundredths)/100 + double(tariffThousandths)/1000;
    qDebug() << tariffUnit << tariffTenth << tariffHundredths <<  tariffThousandths;

    return value;
}

quint32 ELFileParser::getNext3ByteAsInt(QDataStream &inputStream)
{
    quint32 value = 0;
    quint16 byte2 = 0;
    quint8 byte3 = 0;
    inputStream >> byte2;
    inputStream >> byte3;

    bool ok;
    value = (QString::number(byte2,16) + QString::number(byte3,16)).toInt(&ok, 16);

    return value;
}
