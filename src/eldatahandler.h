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

    enum Id
    {
        Id_A = 0x0,
        Id_B = 0x1,
        Id_C = 0x2,
        Id_D = 0x3,
        Id_E = 0x4,
        Id_F = 0x5,
        Id_G = 0x6,
        Id_H = 0x7,
        Id_I = 0x8,
        Id_J = 0x9,
        Id_NbId = 0xA
    };

    struct DataInfo
    {
        QDateTime date;
        double voltage;
        double current;
        double cosPhi;
        double consumedPower;
        Id id;
    };

    QVector<DataInfo> *elData;
    QVector<double> *cumulatedConsumptionById;
    QString dirPath;

private:
    QDateTime dateForNextDataset;

    int nextPositionToInsert;

    Id idToInsert;

signals:

public slots:

};

#endif // ELDATAHANDLER_H
