#include "eldatahandler.h"
#include <QDebug>

elDataHandler::elDataHandler(QObject *parent) : QObject(parent)
{
    elData = new QVector<DataInfo>;
    nextPositionToInsert = 0;
    idToInsert = Id_A;
    cumulatedConsumptionById = new QVector<double>(Id_NbId, 0.0);
}

void elDataHandler::clear()
{
    elData->clear();
    delete cumulatedConsumptionById;
    cumulatedConsumptionById = new QVector<double>(Id_NbId, 0.0);
    nextPositionToInsert = 0;
    idToInsert = Id_A ;
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
        info.consumedPower = (info.voltage*info.current*((info.cosPhi+1)/100))/60;
        info.id = idToInsert;

        //insert data
        elData->insert(nextPositionToInsert, info);

        //compute cumulated consumption by Id
        cumulatedConsumptionById->replace(info.id, (cumulatedConsumptionById->at(info.id) + info.consumedPower));
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
        idToInsert=Id_A;
    }
    else if(id == 'B')
    {
        idToInsert=Id_B;
    }
    else if(id == 'C')
    {
        idToInsert=Id_C;
    }
    else if(id == 'D')
    {
        idToInsert=Id_D;
    }
    else if(id == 'E')
    {
        idToInsert=Id_E;
    }
    else if(id == 'F')
    {
        idToInsert=Id_F;
    }
    else if(id == 'G')
    {
        idToInsert=Id_G;
    }
    else if(id == 'H')
    {
        idToInsert=Id_H;
    }
    else if(id == 'I')
    {
        idToInsert=Id_I;
    }
    else if(id == 'J')
    {
        idToInsert=Id_J;
    }
}
