#ifndef MCUDETAILSVIEWMODEL_H
#define MCUDETAILSVIEWMODEL_H

#include <QObject>

#include "models/pack/packdescription.h"

class McuDetailsViewModel : public QObject
{
    Q_OBJECT

private:
    PackDescription* m_pack;

    // Кэшированные данные текущего MCU
    Mcu* m_currentMcuObj;
    Memory* m_codeMemory;
    Memory* m_dataMemory;

    DebugAlgorithm * m_debugAlgorithm;

    QString m_currentFlashAlgorithm;
    QStringList m_flashAlgorithms;

public:
    explicit McuDetailsViewModel(QObject *parent = 0);

    void setPack(PackDescription* pack) {m_pack = pack;}

    QString currentFlashAlgorithm() const {return m_currentFlashAlgorithm;}

    QStringList flashAlgorithms() const {return m_flashAlgorithms;}
    QString debugAlgorithm() const;

    QString flashStart() const;
    QString flashSize() const;
    QString ramStart() const;
    QString ramSize() const;
    QString features() const;
    QString description() const;
    QString webPageUrl() const;
    QString datasheetUrl() const;
    QString svdLocalPath() const;

public slots:
    void selectMcu(QString vendor,
                   QString family,
                   QString series,
                   QString mcu);

private:

signals:
    void mcuChanged();
    void loadDataStarted();
    void loadDataFinished();
    void loadDataFailed();
};

#endif // MCUDETAILSVIEWMODEL_H
