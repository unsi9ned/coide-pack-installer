#include "devicefeature.h"
#include <QMap>
#include <cmath>

//------------------------------------------------------------------------------
// Валидные типы
//------------------------------------------------------------------------------
const QStringList DeviceFeature::validCategories = {
    "Crypto", "NVIC", "DMA", "RNG", "CoreOther",
    "ExtBus", "Memory", "MemoryOther", "XTAL", "IntRC",
    "PLL", "RTC", "ClockOther", "PowerMode", "VCC",
    "Consumption", "PowerOther", "BGA", "CSP", "PLCC",
    "QFN", "QFP", "SOP", "DIP", "PackageOther", "IOs",
    "ExtInt", "Temp", "ADC", "DAC", "TempSens",
    "AnalogOther", "PWM", "Timer", "WDT", "TimerOther",
    "MPSerial", "CAN", "ETH", "I2C", "I2S", "LIN",
    "SDIO", "SPI", "UART", "USART", "USBD", "USBH",
    "USBOTG", "ComOther", "Camera", "GLCD", "LCD",
    "Touch", "Other", "I/O", "D/A", "A/D", "Com",
    "USB", "Package", "Backup"
};

//------------------------------------------------------------------------------
// Конструктор
//------------------------------------------------------------------------------
DeviceFeature::DeviceFeature()
{
}

//------------------------------------------------------------------------------
// Сеттеры
//------------------------------------------------------------------------------
DeviceFeature& DeviceFeature::setType(const QString &type)
{
    _type = type;
    return *this;
}

void DeviceFeature::setPname(const QString &pname)
{
    _pname = pname;
}

void DeviceFeature::setN(double n)
{
    _n = n;
}

void DeviceFeature::setM(double m)
{
    _m = m;
}

void DeviceFeature::setName(const QString &name)
{
    _name = name;
}

void DeviceFeature::setCount(int count)
{
    _count = count;
}

//------------------------------------------------------------------------------
// Геттеры
//------------------------------------------------------------------------------
QString DeviceFeature::type() const
{
    return _type;
}

QString DeviceFeature::pname() const
{
    return _pname;
}

double DeviceFeature::n() const
{
    return _n;
}

double DeviceFeature::m() const
{
    return _m;
}

QString DeviceFeature::name() const
{
    return _name;
}

int DeviceFeature::count() const
{
    return _count;
}

//------------------------------------------------------------------------------
// Проверки наличия значений
//------------------------------------------------------------------------------
bool DeviceFeature::hasN() const
{
    return _n != 0.0;
}

bool DeviceFeature::hasM() const
{
    return _m != 0.0;
}

bool DeviceFeature::hasPname() const
{
    return !_pname.isEmpty();
}

bool DeviceFeature::hasName() const
{
    return !_name.isEmpty();
}

bool DeviceFeature::hasCount() const
{
    return _count != 0;
}

//------------------------------------------------------------------------------
// Вывод отформатированного описания свойства в зависимости от его типа
//------------------------------------------------------------------------------
QString DeviceFeature::getFormattedDescription() const
{
    if (!isValid())
    {
        return QString();
    }

    QString desc;

#if 0
    if (!_name.isEmpty())
    {
        desc = QString("%1 (%2)").arg(_name).arg(_type);
    }
    else
    {
        QString categoryDesc = getCategoryDescription(_type);
        if (!categoryDesc.isEmpty()) {
            desc = QString("%1 (%2)").arg(categoryDesc).arg(_type);
        } else {
            desc = _type;
        }
    }
#else
    desc = _type;
#endif

    QString valueStr = formatValueByType();

    if (!valueStr.isEmpty()) {
        desc += ": " + valueStr;
    }

    return desc;
}

//------------------------------------------------------------------------------
// Проверка валидности фичи
//------------------------------------------------------------------------------
bool DeviceFeature::isValid() const
{
    return (!_type.isEmpty() && DeviceFeature::validCategories.contains(_type, Qt::CaseInsensitive));
}

//------------------------------------------------------------------------------
// Статические методы
//------------------------------------------------------------------------------
QStringList DeviceFeature::getValidCategories()
{
    return validCategories;
}

bool DeviceFeature::isValidCategory(const QString &category)
{
    return validCategories.contains(category, Qt::CaseInsensitive);
}

//------------------------------------------------------------------------------
// Форматирует значение в зависимости от типа характеристики
//------------------------------------------------------------------------------
QString DeviceFeature::formatValueByType() const
{
    // Обработка типов с диапазоном напряжения/температуры
    if (_type == "VCC" && hasN() && hasM()) {
        return QString("%1...%2 V").arg(_n).arg(_m);
    }
    else if (_type == "Temp" && hasN() && hasM()) {
        return QString("%1...%2 °C").arg(_n).arg(_m);
    }

    // Обработка частот
    else if ((_type == "XTAL" || _type == "IntRC") && hasN()) {
        if (hasM()) {
            return QString("%1...%2").arg(formatFrequency(_n)).arg(formatFrequency(_m));
        }
        return formatFrequency(_n);
    }
    else if (_type == "RTC" && hasN()) {
        return QString("%1 Hz").arg(_n);
    }

    // Обработка памяти
    else if ((_type == "Memory" || _type == "MemoryOther") && hasN()) {
        return formatMemorySize(_n);
    }

    // Обработка таймеров
    else if (_type == "Timer" && hasN()) {
        if (hasM()) {
            return QString("%1 × %2-bit").arg(_n).arg(_m);
        }
        return QString("%1 unit%2").arg(_n).arg(_n > 1 ? "s" : "");
    }

    // Обработка WDT/RNG/TempSens
    else if ((_type == "WDT" || _type == "RNG" || _type == "TempSens") && hasN()) {
        return QString("%1 unit%2").arg(_n).arg(_n > 1 ? "s" : "");
    }

    // Обработка крипто-блоков
    else if (_type == "Crypto" && hasN()) {
        return QString("%1-bit").arg(_n);
    }

    // Обработка интерфейсов
    else if ((_type == "SPI" || _type == "UART" || _type == "USART") && hasN()) {
        if (hasM()) {
            return QString("%1 unit%2, %3 max").arg(_n).arg(_n > 1 ? "s" : "").arg(formatFrequency(_m));
        }
        return QString("%1 unit%2").arg(_n).arg(_n > 1 ? "s" : "");
    }
    else if ((_type == "I2C" || _type == "I2S") && hasN()) {
        return QString("%1 unit%2").arg(_n).arg(_n > 1 ? "s" : "");
    }
    else if ((_type == "USBD" || _type == "USBH" || _type == "USBOTG") && hasN()) {
        return QString("%1 unit").arg(_n);
    }

    // Обработка ADC/DAC
    else if ((_type == "ADC" || _type == "DAC") && hasN()) {
        if (hasM()) {
            return QString("%1 channel%2, %3-bit").arg(_m).arg(_m > 1 ? "s" : "").arg(_n);
        }
        return QString("%1-bit").arg(_n);
    }

    // Обработка портов ввода-вывода
    else if (_type == "IOs" && hasN()) {
        return QString("%1 pin%2").arg(_n).arg(_n > 1 ? "s" : "");
    }

    // Обработка аналоговых блоков
    else if (_type == "AnalogOther" && hasN()) {
        return QString("%1 unit").arg(_n);
    }

    // Обработка корпусов
    else if ((_type == "QFN" || _type == "CSP" || _type == "BGA" ||
              _type == "PLCC" || _type == "QFP" || _type == "SOP" || _type == "DIP") && hasN()) {
        return QString("%1 pin%2").arg(_n).arg(_n > 1 ? "s" : "");
    }

    // Обработка других типов
    else if (_type == "ComOther" && hasN()) {
        return QString("%1 unit").arg(_n);
    }
    else if (_type == "PowerOther" && hasN()) {
        return QString("%1 unit").arg(_n);
    }
    else if (_type == "ExtInt" && hasN()) {
        return QString("%1 line%2").arg(_n).arg(_n > 1 ? "s" : "");
    }

    // Общий случай
    else if (hasN() && hasM()) {
        return QString("%1...%2").arg(_n).arg(_m);
    }
    else if (hasN()) {
        return QString("%1").arg(_n);
    }

    return QString();
}

//------------------------------------------------------------------------------
// Форматирует частоту в удобочитаемый вид
//------------------------------------------------------------------------------
QString DeviceFeature::formatFrequency(double freq) const
{
    if (freq >= 1000000000) {
        double ghz = freq / 1000000000;
        return QString("%1 GHz").arg(ghz, 0, 'f', 2);
    } else if (freq >= 1000000) {
        double mhz = freq / 1000000;
        if (std::abs(mhz - std::round(mhz)) < 0.01) {
            return QString("%1 MHz").arg(std::round(mhz));
        }
        return QString("%1 MHz").arg(mhz, 0, 'f', 1);
    } else if (freq >= 1000) {
        double khz = freq / 1000;
        if (std::abs(khz - std::round(khz)) < 0.01) {
            return QString("%1 kHz").arg(std::round(khz));
        }
        return QString("%1 kHz").arg(khz, 0, 'f', 1);
    }
    return QString("%1 Hz").arg(freq);
}

//------------------------------------------------------------------------------
// Форматирует размер памяти в удобочитаемый вид
//------------------------------------------------------------------------------
QString DeviceFeature::formatMemorySize(double size) const
{
    if (size >= 1073741824) { // 1 GB
        double gb = size / 1073741824;
        return QString("%1 GB").arg(gb, 0, 'f', (gb < 10) ? 2 : 1);
    } else if (size >= 1048576) { // 1 MB
        double mb = size / 1048576;
        return QString("%1 MB").arg(mb, 0, 'f', (mb < 10) ? 2 : 1);
    } else if (size >= 1024) { // 1 KB
        double kb = size / 1024;
        return QString("%1 KB").arg(kb, 0, 'f', (kb < 10) ? 2 : 1);
    }
    return QString("%1 B").arg(size);
}

//------------------------------------------------------------------------------
// Возвращает описание категории
//------------------------------------------------------------------------------
QString DeviceFeature::getCategoryDescription(const QString &category) const
{
    static QMap<QString, QString> descriptions = {
        {"Crypto", "Cryptographic module"},
        {"NVIC", "Nested Vectored Interrupt Controller"},
        {"DMA", "Direct Memory Access controller"},
        {"RNG", "Random Number Generator"},
        {"ADC", "Analog-to-Digital Converter"},
        {"DAC", "Digital-to-Analog Converter"},
        {"UART", "Universal Asynchronous Receiver/Transmitter"},
        {"USART", "Universal Synchronous/Asynchronous Receiver/Transmitter"},
        {"SPI", "Serial Peripheral Interface"},
        {"I2C", "Inter-Integrated Circuit"},
        {"I2S", "Inter-IC Sound"},
        {"CAN", "Controller Area Network"},
        {"USB", "Universal Serial Bus"},
        {"USBD", "USB Device"},
        {"USBH", "USB Host"},
        {"USBOTG", "USB On-The-Go"},
        {"ETH", "Ethernet controller"},
        {"Timer", "Timer"},
        {"PWM", "Pulse Width Modulation"},
        {"WDT", "Watchdog Timer"},
        {"RTC", "Real-Time Clock"},
        {"PLL", "Phase-Locked Loop"},
        {"XTAL", "External crystal"},
        {"IntRC", "Internal RC oscillator"},
        {"Memory", "Memory"},
        {"MemoryOther", "Memory"},
        {"VCC", "Supply voltage"},
        {"Temp", "Temperature range"},
        {"TempSens", "Temperature sensor"},
        {"IOs", "I/O pins"},
        {"ExtInt", "External interrupts"},
        {"AnalogOther", "Analog peripheral"},
        {"PowerOther", "Power management"},
        {"ComOther", "Communication interface"},
        {"BGA", "BGA package"},
        {"QFN", "QFN package"},
        {"CSP", "CSP package"},
        {"QFP", "QFP package"},
        {"Touch", "Touch controller"},
        {"LCD", "LCD controller"},
        {"Camera", "Camera interface"}
    };

    return descriptions.value(category, "");
}
