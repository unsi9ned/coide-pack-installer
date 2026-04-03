#include <QUuid>
#include "debugalgorithm.h"
#include "services/paths.h"

DebugAlgorithm::DebugAlgorithm(const QString &name)
{
    this->_coId = -1;
    this->_name = name;
    this->_createDate = QDateTime::currentDateTime();
    this->_updateDate = QDateTime::currentDateTime();
    this->_timeUUID = generateTimeUUID();
    this->_documentId = static_cast<int>(CoDocument::DOC_DEBUG_ALGO);
}

void DebugAlgorithm::setCoId(int id)
{
    this->_coId = id;
}

void DebugAlgorithm::setName(QString name)
{
    this->_name = name;
}

void DebugAlgorithm::setProcessor(const QString &core)
{
    QString debugAlgotrithmFile = Paths::instance()->coIdeDebugAlgorithmFileName(core);

    if(!debugAlgotrithmFile.isEmpty())
    {
        _name = debugAlgotrithmFile;
    }
}

void DebugAlgorithm::setDescription(const QString &description)
{
    this->_description = description;
}

void DebugAlgorithm::setCreateDate(const QString &dt)
{
    this->_createDate = QDateTime::fromString(dt, "yyyy-MM-dd HH:mm:ss.z");
}

void DebugAlgorithm::setUpdateDate(const QString &dt)
{
    this->_updateDate = QDateTime::fromString(dt, "yyyy-MM-dd HH:mm:ss.z");
}

void DebugAlgorithm::setTimeUUID(const QString &uuid)
{
    this->_timeUUID = uuid;
}

void DebugAlgorithm::setDocumentId(int id)
{
    this->_documentId = id;
}

int DebugAlgorithm::coId() const
{
    return this->_coId;
}

QString DebugAlgorithm::name() const
{
    return this->_name;
}

QString DebugAlgorithm::description() const
{
    return this->_description;
}

QString DebugAlgorithm::creationDate(QString dtFormat) const
{
    QDateTime dt = this->_createDate;

    if(!dt.isValid())
        dt = QDateTime::currentDateTime();

    return dt.toString(dtFormat);
}

QString DebugAlgorithm::updateDate(QString dtFormat) const
{
    QDateTime dt = this->_updateDate;

    if(!dt.isValid())
        dt = QDateTime::currentDateTime();

    return dt.toString(dtFormat);
}

QString DebugAlgorithm::timeUUID() const
{
    return this->_timeUUID;
}

int DebugAlgorithm::documentId() const
{
    return this->_documentId;
}

bool DebugAlgorithm::isNull() const
{
    return this->_name.isEmpty();
}

QString DebugAlgorithm::generateTimeUUID()
{
    QString str = QUuid::createUuid().toString();

    // Удаляем фигурные скобки в начале и конце
    if (str.startsWith('{') && str.endsWith('}'))
    {
        str = str.mid(1, str.length() - 2);
    }

    return str;
}
