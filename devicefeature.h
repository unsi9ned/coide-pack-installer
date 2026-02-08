#ifndef DEVICEFEATURE_H
#define DEVICEFEATURE_H

#include <QString>
#include <QStringList>

class DeviceFeature
{
private:
    static const QStringList validCategories;

    QString _type;          // Тип характеристики (обязательный)
    QString _pname;         // Имя периферийного блока (опциональный)
    double  _n = 0.0;       // Числовой параметр 1 (например, размер, количество)
    double  _m = 0.0;       // Числовой параметр 2 (например, разрядность)
    QString _name;          // Человеко-читаемое описание (опциональный)
    int     _count = 0;     // Устаревшее поле для обратной совместимости

    // Вспомогательные методы
    QString formatValueByType() const;
    QString formatFrequency(double freq) const;
    QString formatMemorySize(double size) const;
    QString getCategoryDescription(const QString &category) const;

public:
    DeviceFeature();

    // Сеттеры (с Fluent interface для setType)
    DeviceFeature& setType(const QString &type);
    void setPname(const QString &pname);
    void setN(double n);
    void setM(double m);
    void setName(const QString &name);
    void setCount(int count);

    // Геттеры
    QString type() const;
    QString pname() const;
    double n() const;
    double m() const;
    QString name() const;
    int count() const;

    // Проверки наличия значений
    bool hasN() const;
    bool hasM() const;
    bool hasPname() const;
    bool hasName() const;
    bool hasCount() const;

    // Основные методы
    QString getFormattedDescription() const;
    bool isValid() const;

    // Статические методы
    static QStringList getValidCategories();
    static bool isValidCategory(const QString &category);
};

#endif // DEVICEFEATURE_H
