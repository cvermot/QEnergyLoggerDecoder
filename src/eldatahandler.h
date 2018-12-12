#ifndef ELDATAHANDLER_H
#define ELDATAHANDLER_H

#include <QObject>
#include <QDateTime>
#include <QVector>
#include <QString>
#include <QDateTime>

class elDataHandler : public QObject
{
    Q_OBJECT
public:
    explicit elDataHandler(QObject *parent = nullptr);

    void addDatatset(quint16 p_voltage, quint16 p_current, quint8 p_cosphi);
    void setDateForNextDataset(QDateTime p_datetime, QChar id);
    void clear();

    struct DataInfo
    {
        QDateTime date;
        double voltage;
        double current;
        double cosPhi;
        int id;
    };

    QVector<DataInfo> *elData;
    QString dirPath;

private:
    QDateTime dateForNextDataset;

    int nextPositionToInsert;

    int idToInsert;

signals:

public slots:

};

#endif // ELDATAHANDLER_H
