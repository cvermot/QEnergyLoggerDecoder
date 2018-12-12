#include "eldatahandler.h"
#include <QDebug>

elDataHandler::elDataHandler(QObject *parent) : QObject(parent)
{
    elData = new QVector<DataInfo>;
    nextPositionToInsert = 0;
    idToInsert = 0;
    //dirPath = QString("./dataset/20171217-1090Wh/");
}

void elDataHandler::clear()
{
    elData->clear();
    nextPositionToInsert = 0;
    idToInsert = 0 ;
}

void elDataHandler::addDatatset(quint16 p_voltage, quint16 p_current, quint8 p_cosphi)
{
    DataInfo info;

    //If we have a valid value
    if(((QByteArray::number(p_voltage).toUInt()) != 0xFFFF)
        && ((QByteArray::number(p_current).toUInt()) != 0xFFFF)
        && ((QByteArray::number(p_cosphi).toShort()) != 0xFF))
    {
        info.date = dateForNextDataset;
        info.voltage = QByteArray::number(p_voltage).toDouble()/10;
        info.current = QByteArray::number(p_current).toDouble()/1000;
        info.cosPhi = QByteArray::number(p_cosphi).toShort();
        info.id = idToInsert;
        elData->insert(nextPositionToInsert, info);
    }
    dateForNextDataset = dateForNextDataset.addSecs(60) ;

}

void elDataHandler::setDateForNextDataset(QDateTime p_datetime, QChar id)
{
    //1) look for the first date which is superior to provided time
    //2) stock indice of the date found
    //3) insert new data before this value

    nextPositionToInsert = 0;
    if(elData->size() > 0)
    {
        for (int k = 0; k < elData->size(); k++)
        {
            if (p_datetime < elData->at(nextPositionToInsert).date)
            {
                nextPositionToInsert++;
            }
            else
            {
                k = elData->size();
            }
        }
    //TODO optimize
    }
    dateForNextDataset = p_datetime;


    if(id == 'A')
    {
        idToInsert=0;
    }
    else if(id == 'B')
    {
        idToInsert=1;
    }
    else if(id == 'C')
    {
        idToInsert=2;
    }
    else if(id == 'D')
    {
        idToInsert=3;
    }
    else if(id == 'E')
    {
        idToInsert=4;
    }
    else if(id == 'F')
    {
        idToInsert=5;
    }
    else if(id == 'G')
    {
        idToInsert=6;
    }
    else if(id == 'H')
    {
        idToInsert=7;
    }
    else if(id == 'I')
    {
        idToInsert=8;
    }
    else if(id == 'J')
    {
        idToInsert=9;
    }
}
