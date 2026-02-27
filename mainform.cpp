#include "mainform.h"
#include "ui_mainform.h"
#include "paths.h"
#include "ziparchive.h"
#include "packmanager.h"
#include "dbgarbagecollector.h"

MainForm::MainForm(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainForm)
{
    ui->setupUi(this);

#define  DANGEROUS_TEST  0

#if 0
    pack.setPathToArchive(QApplication::applicationDirPath() + "/" + "NordicSemiconductor.nRF_DeviceFamilyPack.8.15.0.pack");
#elif 1
    pack.setPathToArchive(QApplication::applicationDirPath() + "/" + "NordicSemiconductor.nRF_DeviceFamilyPack.8.11.1.pack");
#elif 0 && DANGEROUS_TEST
    pack.setPathToArchive(QApplication::applicationDirPath() + "/" + "NordicSemiconductor.nRF_DeviceFamilyPack.8.44.1.pack");
#elif 0 && DANGEROUS_TEST
    pack.setPathToArchive(QApplication::applicationDirPath() + "/" + "Keil.STM32F1xx_DFP.2.1.0.pack");
#elif 0 && DANGEROUS_TEST
    pack.setPathToArchive(QApplication::applicationDirPath() + "/" + "Keil.STM32F1xx_DFP.2.2.0.pack");
#elif 1 && DANGEROUS_TEST
    pack.setPathToArchive(QApplication::applicationDirPath() + "/" + "Keil.STM32F4xx_DFP.2.11.0.pack");
#elif 1
    pack.setPathToArchive(QApplication::applicationDirPath() + "/" + "Keil.SAMD21_DFP.1.2.0.pack");
#elif 1 && DANGEROUS_TEST
    pack.setPathToArchive(QApplication::applicationDirPath() + "/" + "Microchip.SAMD21_DFP.3.7.262.atpack");
#endif

#undef DANGEROUS_TEST

    packMgr.readPackDescription(pack);

    //Иконка
    this->setWindowIcon(QIcon(":coide_project.ico"));

    //Версия программы
    ui->labelAppVersion->setText(compilationVersion());
    ui->lineEditIdePath->setText(Paths::instance()->coIdeDir());

    //Выбор производителя
    connect(ui->listWidgetManufacturer,
            SIGNAL(clicked(QModelIndex)),
            this,
            SLOT(showFamilyList(QModelIndex)));

    connect(ui->listWidgetManufacturer,
            SIGNAL(clicked(QModelIndex)),
            this,
            SLOT(showDFPInfo(QModelIndex)));

    //Выбор семейства
    connect(ui->listWidgetFamily,
            SIGNAL(clicked(QModelIndex)),
            this,
            SLOT(showSeriesList(QModelIndex)));

    //Выбор серии
    connect(ui->listWidgetSeries,
            SIGNAL(clicked(QModelIndex)),
            this,
            SLOT(showMcuList(QModelIndex)));

    //Выбор процессора
    connect(ui->listWidgetMcu,
            SIGNAL(clicked(QModelIndex)),
            this,
            SLOT(showFeatures(QModelIndex)));

    //Реакция на выбор вкладки
    connect(ui->tabWidget,
            SIGNAL(currentChanged(int)),
            this,
            SLOT(selectNewTab(int)));

    //Посетить вебсайт автора
    connect(ui->labelWebSite,
            SIGNAL(clicked()),
            this,
            SLOT(visitWebsite()));

    //Перезаполнение списков в зависимости от выбора проивзодителя/семейства/Серии/чипа

    connect(ui->comboBoxManufacturer,
            SIGNAL(currentIndexChanged(int)),
            this,
            SLOT(showFamilyList(int)));

    connect(ui->comboBoxFamily,
            SIGNAL(currentIndexChanged(int)),
            this,
            SLOT(showSeriesList(int)));

    connect(ui->comboBoxSerie,
            SIGNAL(currentIndexChanged(int)),
            this,
            SLOT(showMcuList(int)));

    connect(ui->comboBoxMcu,
            SIGNAL(currentIndexChanged(int)),
            this,
            SLOT(showFilteredComponents(int)));
    //--------------------------------------------------------------------------

    //Обновление данных на форме. Данные были загружены ранее из БД
    //Это произошло при создании объекта mcuInfo
    refreshData();
}

//------------------------------------------------------------------------------
// Деструктор
//------------------------------------------------------------------------------
MainForm::~MainForm()
{
    delete ui;
}

//------------------------------------------------------------------------------
// Дата компиляции программы
//------------------------------------------------------------------------------
QString MainForm::compilationVersion()
{
    QString compilation_date = QString(__DATE__);
    QStringList months;
    QString date_part;
    QString time_part = QString(__TIME__);
    int month = 0;

    months << "Jan" << "Feb" << "Mar" << "Apr" << "May" << "Jun" <<
              "Jul" << "Aug" << "Sep" << "Oct" << "Nov" << "Dec";

    for(int m = 0; m < months.count(); m++)
    {
        if(months.at(m) == compilation_date.mid(0, 3))
        {
            month = m + 1;
            break;
        }
    }

    if(month == 0)
    {
        date_part = "19000101";
    }
    else
    {
        date_part = compilation_date.mid(7, 4);
        date_part += QString("%1").arg(month, 2, 10, QChar('0'));
        date_part += compilation_date.mid(4, 2);
    }

    time_part = time_part.mid(0, 2) + time_part.mid(3, 2);

    return date_part + "." + time_part;
}

//------------------------------------------------------------------------------
// Отобразить ошибку
//------------------------------------------------------------------------------
void MainForm::showError(QString e)
{
    QMessageBox::critical(this, tr("Ошибка"), e, QMessageBox::Ok);
}

//------------------------------------------------------------------------------
// Отобразить информацию
//------------------------------------------------------------------------------
void MainForm::showInfo(QString i)
{
    QMessageBox::information(this, tr("Информация"), i, QMessageBox::Ok);
}

//------------------------------------------------------------------------------
// Обновить списки
//------------------------------------------------------------------------------
void MainForm::refreshData()
{
    QString itemText = "";

    ui->listWidgetManufacturer->clear();
    ui->listWidgetFamily->clear();
    ui->listWidgetSeries->clear();
    ui->listWidgetMcu->clear();
    ui->lineEditUrl->clear();
    ui->lineEditDatasheetUrl->clear();

    //Очистка списков компонентов
//    ui->comboBoxMcu->clear();
//    ui->comboBoxSerie->clear();
//    ui->comboBoxFamily->clear();
//    ui->comboBoxManufacturer->clear();
//    ui->listWidgetComponents->clear();
    //----------------------------------------

    ui->comboBoxManufacturer->addItem(tr("- Не выбран -"));
    //ui->comboBoxFamily->addItem(tr("- Не выбрано -"));

    foreach (QString vendorName, pack.vendors().keys())
    {
        ui->listWidgetManufacturer->addItem(vendorName);
        ui->comboBoxManufacturer->addItem(vendorName);
    }

    //Загрузка алгоритмов отладки и программирования
    refreshAlgorithms();

//    //Загрузка списка компонентов
//    QMap<int, Component>::iterator compIterator = componInfo.components()->begin();

//    while(compIterator != componInfo.components()->end())
//    {
//        Component currentComponent = compIterator.value();
//        QString itemText = QString("%1 - %2").
//                            arg(currentComponent.getId()).
//                            arg(currentComponent.getName());

//        ui->listWidgetComponents->addItem(itemText);
//        ++compIterator;
//    }
    showFilteredComponents();

    ui->labelComponentsCount->setNum(ui->listWidgetComponents->count());
}

//------------------------------------------------------------------------------
// Обновить список алгоритмов
//------------------------------------------------------------------------------
void MainForm::refreshAlgorithms()
{
    int debugAlgIndex = ui->comboBoxDebugAlg->currentIndex();
    int flashAlgIndex = ui->comboBoxFlashAlg->currentIndex();

    QString itemText;

    //Загрузка алгоритмов отладки
    ui->comboBoxDebugAlg->clear();
    ui->listWidgetDebAlg->clear();

#if 0
    QList<int> daKeys = mcuInfo.getDebugAlgKeys();
    QList<int> faKeys = mcuInfo.getFlashAlgKeys();

    foreach(int daKey, daKeys)
    {
        DebugAlgorithm da = mcuInfo.getDebugAlg(daKey);

        itemText = QString("%1 - %2").arg(da.getId()).arg(da.getName());

        ui->comboBoxDebugAlg->addItem(itemText);
        ui->listWidgetDebAlg->addItem(itemText);
    }

    //Загрузка алгоритмов программирования
    ui->comboBoxFlashAlg->clear();
    ui->listWidgetFlashAlg->clear();

    foreach(int faKey, faKeys)
    {
        FlashAlgorithm fa = mcuInfo.getFlashAlg(faKey);

        itemText = QString("%1 - %2").arg(fa.getId()).arg(fa.getName());

        ui->comboBoxFlashAlg->addItem(itemText);
        ui->listWidgetFlashAlg->addItem(itemText);
    }
    ui->comboBoxFlashAlg->addItem(tr("Отсутствует"));

    //Выбираем алгоритм тот, который был выбран ранее
    if(debugAlgIndex > -1 &&
       debugAlgIndex< ui->comboBoxDebugAlg->count())
    {
        ui->comboBoxDebugAlg->setCurrentIndex(debugAlgIndex);
    }

    if(flashAlgIndex > -1 &&
       flashAlgIndex< ui->comboBoxFlashAlg->count())
    {
        ui->comboBoxFlashAlg->setCurrentIndex(flashAlgIndex);
    }
#endif

    refreshNewDebAlgorithm();
    refreshNewFlashAlgorithm();
}

//------------------------------------------------------------------------------
// Обновить список новых алгоритмов отладки
//------------------------------------------------------------------------------
void MainForm::refreshNewDebAlgorithm()
{
    ui->listWidgetNewDebAlg->clear();

#if 0
    mcuInfo.searchNewDebugAlgorithm();
    QList<int> newDaKeys = mcuInfo.getNewDebugAlgKeys();
    QString itemText;

    foreach(int daKey, newDaKeys)
    {
        DebugAlgorithm newDA = mcuInfo.getNewDebugAlg(daKey);
        itemText = QString("%1 - %2").arg(newDA.getId()).arg(newDA.getName());
        ui->listWidgetNewDebAlg->addItem(itemText);
    }
#endif
}

//------------------------------------------------------------------------------
// Обновить список новых алгоритмов программирования
//------------------------------------------------------------------------------
void MainForm::refreshNewFlashAlgorithm()
{
    ui->listWidgetNewFlashAlg->clear();
#if 0
    mcuInfo.searchNewFlashAlgorithm();

    QList<int> newFaKeys = mcuInfo.getNewFlashAlgKeys();
    QString itemText;

    foreach(int faKey, newFaKeys)
    {
        FlashAlgorithm newFA = mcuInfo.getNewFlashAlg(faKey);
        itemText = QString("%1 - %2").arg(newFA.getId()).arg(newFA.getName());
        ui->listWidgetNewFlashAlg->addItem(itemText);
    }
#endif
}

//------------------------------------------------------------------------------
// обработка события открытия другой вкладки
//------------------------------------------------------------------------------
void MainForm::selectNewTab(int t)
{
    //Открыта вкладка алгоритмов
//    if(ui->tabWidget->currentWidget() == ui->tabAlgorithm)
//    {
//        mcuInfo.loadAlgorithmFromDb();
    //    }

    //Открыта вкладка компонентов
//    if(ui->tabWidget->currentWidget() == ui->tabComponents)
//    {
//        componInfo.loadDataFromDb();
//    }
}

//------------------------------------------------------------------------------
// Посетить веб-сайт автора
//------------------------------------------------------------------------------
void MainForm::visitWebsite()
{
    QDesktopServices::openUrl(QUrl(ui->labelWebSite->text()));
    qDebug() << "web";
}

//------------------------------------------------------------------------------
// Отображение информации о пакете от производителя
//------------------------------------------------------------------------------
void MainForm::showDFPInfo(QModelIndex index)
{
    ui->lineEditUrl->clear();
    ui->lineEditDatasheetUrl->clear();
    ui->plainTextEditDescription->clear();

    QListWidgetItem * currentItem = ui->listWidgetManufacturer->item(index.row());

    if(currentItem == NULL)
    {
        return;
    }

    ui->lineEditUrl->setText(pack.url());
    ui->lineEditRelease->setText(pack.release());
    ui->plainTextEditDescription->setPlainText(pack.description());
}

//------------------------------------------------------------------------------
// Показать список семейств
//------------------------------------------------------------------------------
void MainForm::showFamilyList(QModelIndex index)
{
    Q_UNUSED(index)

    ui->listWidgetFamily->clear();
    ui->listWidgetSeries->clear();
    ui->listWidgetMcu->clear();

    QListWidgetItem * currentItem = ui->listWidgetManufacturer->item(index.row());

    if(currentItem == NULL)
    {
        return;
    }

    QMap<QString, Family>& families = pack.vendor(currentItem->text()).families();

    for(auto it = families.begin(); it != families.end(); ++it)
    {
        ui->listWidgetFamily->addItem(it.key());
    }
}

//------------------------------------------------------------------------------
// Показать список семейств для выпадающего списка
//------------------------------------------------------------------------------
void MainForm::showFamilyList(int index)
{
    Q_UNUSED(index);

    ui->comboBoxFamily->clear();
    ui->comboBoxFamily->addItem("- Не выбрано -");

    //Список производителей пуст. Нет смысла пытаться загрузить список семейств
    if(ui->comboBoxManufacturer->count() <= 1)
    {
        return;
    }

    QMap<QString, Family>& families = pack.vendor(ui->comboBoxManufacturer->currentText()).families();

    for(auto it = families.begin(); it != families.end(); ++it)
    {
        ui->comboBoxFamily->addItem(it.key());
    }

    //if(ui->comboBoxManufacturer->currentIndex() > 0)
    {
        showFilteredComponents();
    }
}

//------------------------------------------------------------------------------
// Показать список серий для выпадающего списка
//------------------------------------------------------------------------------
void MainForm::showSeriesList(int index)
{
    Q_UNUSED(index);

    ui->comboBoxSerie->clear();
    ui->comboBoxSerie->addItem("- Не выбрана -");

    //Список семейств пуст. Нет смысла пытаться загрузить список серий
    if(ui->comboBoxFamily->count() <= 1)
    {
        return;
    }

    QString vendor = ui->comboBoxManufacturer->currentText();
    QString armCore = ui->comboBoxFamily->currentText();
    QMap<QString, Series>& series = pack.vendor(vendor).family(armCore).seriesMap();

    for(auto it = series.begin(); it != series.end(); ++it)
    {
        ui->comboBoxSerie->addItem(it.key());
    }

    showFilteredComponents();
}

//------------------------------------------------------------------------------
// Показать список процессоров для выпадающего списка
//------------------------------------------------------------------------------
void MainForm::showMcuList(int index)
{
    Q_UNUSED(index);

    ui->comboBoxMcu->clear();
    ui->comboBoxMcu->addItem("- Не выбран -");

    //Список серий пуст. Нет смысла пытаться загрузить список процессоров
    if(ui->comboBoxSerie->count() <= 1)
    {
        return;
    }

    QString vendor = ui->comboBoxManufacturer->currentText();
    QString armCore = ui->comboBoxFamily->currentText();
    QString series = ui->comboBoxSerie->currentText();
    QMap<QString, Mcu>& mcuMap = pack.vendor(vendor).family(armCore).series(series).mcuMap();

    for(auto it = mcuMap.begin(); it != mcuMap.end(); ++it)
    {
        ui->comboBoxMcu->addItem(it.key());
    }

    showFilteredComponents();
}

//------------------------------------------------------------------------------
// Показать список серий
//------------------------------------------------------------------------------
void MainForm::showSeriesList(QModelIndex index)
{
    Q_UNUSED(index)

    QListWidgetItem * currentItem = ui->listWidgetFamily->item(index.row());
    QListWidgetItem * currentFamilyItem = ui->listWidgetFamily->currentItem();
    QListWidgetItem * currentManItem = ui->listWidgetManufacturer->currentItem();

    if(currentItem == NULL || currentFamilyItem == NULL || currentManItem == NULL)
    {
        return;
    }

    ui->listWidgetSeries->clear();
    ui->listWidgetMcu->clear();

    QString vendor = currentManItem->text();
    QString armCore = currentFamilyItem->text();
    QMap<QString, Series>& seriesMap = pack.vendor(vendor).family(armCore).seriesMap();

    for(auto it = seriesMap.begin(); it != seriesMap.end(); ++it)
    {
        ui->listWidgetSeries->addItem(it.key());
    }
}

//------------------------------------------------------------------------------
// Показать список микроконтроллеров
//------------------------------------------------------------------------------
void MainForm::showMcuList(QModelIndex index)
{
    Q_UNUSED(index)

    QListWidgetItem * currentSeries = ui->listWidgetSeries->item(index.row());
    QListWidgetItem * currentFamilyItem = ui->listWidgetFamily->currentItem();
    QListWidgetItem * currentManItem = ui->listWidgetManufacturer->currentItem();

    if(currentSeries == NULL ||
       currentFamilyItem == NULL ||
       currentManItem == NULL)
    {
        return;
    }

    ui->listWidgetMcu->clear();

    QString vendor = currentManItem->text();
    QString armCore = currentFamilyItem->text();
    QString series = currentSeries->text();
    QMap<QString, Mcu>& mcuMap = pack.vendor(vendor).family(armCore).series(series).mcuMap();

    for(auto it = mcuMap.begin(); it != mcuMap.end(); ++it)
    {
        ui->listWidgetMcu->addItem(it.key());
    }
}

//------------------------------------------------------------------------------
// Показать параметры микроконтроллера
//------------------------------------------------------------------------------
void MainForm::showFeatures(QModelIndex index)
{
    Q_UNUSED(index)

    QListWidgetItem * currentManItem = ui->listWidgetManufacturer->currentItem();
    QListWidgetItem * currentFamilyItem = ui->listWidgetFamily->currentItem();
    QListWidgetItem * currentSeries = ui->listWidgetSeries->currentItem();
    QListWidgetItem * currentMcu = ui->listWidgetMcu->item(index.row());

    if(currentSeries == NULL ||
       currentFamilyItem == NULL ||
       currentManItem == NULL)
    {
        return;
    }

    ui->plainTextEditFeatures->clear();

    QString vendor = currentManItem->text();
    QString armCore = currentFamilyItem->text();
    QString series = currentSeries->text();
    QString mcu = currentMcu->text();

    //
    // Загрузка данных о памяти
    //
    Family& devCore = pack.vendor(vendor).family(armCore);
    Series& devSeries = devCore.series(series);
    Mcu& device = devSeries.mcu(mcu);
    Memory * codeMemory = device.getCodeMemory();
    Memory * dataMemory = device.getDataMemory();

    if(codeMemory)
    {
        ui->lineEditFlashStart->setText(codeMemory->startAddrHex());
        ui->lineEditFlashSize->setText(codeMemory->sizeHex());
    }
    else
    {
        ui->lineEditFlashStart->clear();
        ui->lineEditFlashSize->clear();
    }

    if(dataMemory)
    {
        ui->lineEditRamStart->setText(dataMemory->startAddrHex());
        ui->lineEditRamSize->setText(dataMemory->sizeHex());
    }
    else
    {
        ui->lineEditRamStart->clear();
        ui->lineEditRamSize->clear();
    }

    //
    // Загрузка данных о фичах
    //
#if 1
    QStringList deviceFeatures = device.featuresSummary();
    QString featuresText;

    if(!deviceFeatures.isEmpty())
        featuresText += deviceFeatures.join('\n');

    ui->plainTextEditFeatures->setPlainText(featuresText);
#else
    ui->plainTextEditFeatures->setPlainText(device.coFeaturesSummary());
#endif

    //
    // Загрузка описания
    //
#if 1
    if(!device.getDescription().isEmpty())
        ui->plainTextEditDescription->setPlainText(device.getDescription());
    else if(!device.coDescription().isEmpty())
        ui->plainTextEditDescription->setPlainText(device.coDescription());
    else
        ui->plainTextEditDescription->setPlainText(pack.description());
#elif 0
    ui->plainTextEditDescription->setPlainText(device.coMemInfo());
#else
    ui->plainTextEditDescription->setPlainText(device.defSym2coMicro());
#endif

    //
    // Загрузка алгоритмов программирования
    //
    int32_t flashStartAddr = codeMemory ? codeMemory->startAddr() : -1;
    ProgAlgorithm * devAlgorithm = device.getFlashAlgorithm(flashStartAddr);

    ui->comboBoxFlashAlg->clear();

    for(int i = 0; i < device.algorithms().count(); i++)
    {
        ProgAlgorithm a = device.algorithms().at(i);

        if(devAlgorithm && devAlgorithm->name() == a.name())
        {
            ui->comboBoxFlashAlg->setCurrentIndex(i);
        }

        ui->comboBoxFlashAlg->addItem(a.name());
    }

    //
    // Вывод алгоритмов отладки
    //
    ui->comboBoxDebugAlg->clear();

    if(!device.getDebugAlgorithm().name().isEmpty())
    {
        ui->comboBoxDebugAlg->addItem(device.getDebugAlgorithm().name());
        ui->comboBoxDebugAlg->setCurrentIndex(0);
    }

    //
    // Вывод URL
    //
    ui->lineEditUrl->setText(device.getWebPageURL());
    ui->lineEditDatasheetUrl->setText(device.getDatasheetURL());

    //
    // Вывод пути к SVD-файлу
    //
    ui->lineEditSVD->setText(device.svdLocalPath());
}

//------------------------------------------------------------------------------
// Загрузка данных из базы
//------------------------------------------------------------------------------
void MainForm::on_pushButtonDataLoad_clicked()
{
    ui->pushButtonDataLoad->setEnabled(false);

#if 0
    mcuInfo.loadDataFromDb();
    componInfo.loadDataFromDb();
#endif

    refreshData();

    ui->pushButtonDataLoad->setEnabled(true);
}

//------------------------------------------------------------------------------
// Сохранить параметры процессора
//------------------------------------------------------------------------------
void MainForm::on_pushButtonSave_clicked()
{
    ui->pushButtonSave->setEnabled(false);
    packMgr.packInstall(pack);
    ui->pushButtonSave->setEnabled(true);
#if 0
    QListWidgetItem * manItem = ui->listWidgetManufacturer->currentItem();
    QListWidgetItem * famItem = ui->listWidgetFamily->currentItem();
    QListWidgetItem * serItem = ui->listWidgetSeries->currentItem();
    QListWidgetItem * mcuItem = ui->listWidgetMcu->currentItem();

    if(manItem == NULL)
    {
        showError(tr("Не выбран производитель"));
        return;
    }
    else if(famItem == NULL)
    {
        showError(tr("Не выбрано семейство"));
        return;
    }
    else if(serItem == NULL)
    {
        showError(tr("Не выбрана серия"));
        return;
    }
    else if(mcuItem == NULL)
    {
        showError(tr("Не выбран микроконтроллер"));
        return;
    }

    int manId = extractIdFromItemText(manItem->text());
    int famId = extractIdFromItemText(famItem->text());
    int serId = extractIdFromItemText(serItem->text());
    int mcuId = extractIdFromItemText(mcuItem->text());

    QString debugAlgName = extractNameFromItemText(ui->comboBoxDebugAlg->currentText());
    QString flashAlgName = extractNameFromItemText(ui->comboBoxFlashAlg->currentText());

    QString url = ui->lineEditUrl->text();
    QString datasheet = ui->lineEditDatasheetUrl->text();
    QString description = ui->plainTextEditDescription->toPlainText();
    QStringList features = ui->plainTextEditFeatures->toPlainText().split("\n");
    QString flashStartAddr = ui->lineEditFlashStart->text();
    QString ramStartAddr = ui->lineEditRamStart->text();
    QString flashSize = ui->lineEditFlashSize->text();
    QString ramSize = ui->lineEditRamSize->text();

    Manufacturer man = mcuInfo.getManufacturer(manId);
    Family fam = man.getFamily(famId);
    Serie ser = fam.getSerie(serId);
    Mcu mcu = ser.getMcu(mcuId);

    if(mcu.getId() > -1)
    {
        DebugAlgorithm debAlg = mcuInfo.getDebugAlg(debugAlgName);
        FlashAlgorithm flashAlg = mcuInfo.getFlashAlg(flashAlgName);

        //Изменен алгоритм отладки
        if(debAlg.getId() > -1)
        {
            mcu.setDebugAlgorithm(debAlg);
        }

        //Изменен алгоритм программирования
        if(flashAlg.getId() > -1)
        {
            mcu.setFlashAlgorithm(flashAlg);
        }

        //Задан URL
        if(!url.isEmpty())
        {
            QByteArray ba;
            url  = "[\"" + url + "\"]";
            ba.append(url);

            mcu.setWebPageURL(ba);
        }

        //Задан даташит
        if(!datasheet.isEmpty())
        {
            QByteArray ba;
            datasheet = "[\"" + datasheet + "\"]";
            ba.append(datasheet);

            mcu.setDatasheetURL(ba);
        }

        //Задано описание
        if(!description.isEmpty())
        {
            mcu.setDescription(description);
        }

        //Заданы характеристики
        if(features.length())
        {
            for(int i = 0; i < features.length();i++)
            {
                features[i] = "\"" + features[i] + "\"";
            }

            QString featuresStr = "[" + features.join(",") + "]";
            mcu.setKeyParameter(QByteArray().append(featuresStr));
        }

        //Параметры памяти
        if(flashStartAddr.isEmpty() ||
           flashSize.isEmpty() ||
           ramStartAddr.isEmpty() ||
           ramSize.isEmpty())
        {
            showError(tr("Параметры памяти заданы не полностью"));
        }
        else
        {
            QString memStr = QString(
                             "[\"{\\\"type\\\":\\\"FLASH\\\","
                             "\\\"id\\\":0,\\\"start\\\":\\\"%1\\\","
                             "\\\"size\\\":\\\"%2\\\"}\","
                             "\"{\\\"type\\\":\\\"RAM\\\","
                             "\\\"id\\\":0,"
                             "\\\"start\\\":\\\"%3\\\","
                             "\\\"size\\\":\\\"%4\\\"}\"]"
                            ).
                    arg(flashStartAddr).
                    arg(flashSize).
                    arg(ramStartAddr).
                    arg(ramSize);

            mcu.setMemInfo(QByteArray().append(memStr));
        }

        if(mcuInfo.updateMcuInfo(mcu))
        {
            showInfo(tr("Данные контроллера %1 обновлены").arg(mcu.getName()));
        }
    }
    else
    {
        showError(tr("Не выбран микроконтроллер"));
        return;
    }
#endif
}

//------------------------------------------------------------------------------
// Задать путь к среде разработки
//------------------------------------------------------------------------------
void MainForm::on_pushButtonSetIdePath_clicked()
{
#if 0
    QString str =
            QFileDialog::getExistingDirectory(this,
                                              tr("Директория установки CooCox IDE"),
                                              IDE_PATH);
    ui->lineEditIdePath->setText(str);
    mcuInfo.changeIdePath(str);
    ui->tabWidget->setCurrentWidget(ui->tabMcu);

    mcuInfo.loadDataFromDb();
    componInfo.loadDataFromDb();
    refreshData();
#endif

    QString str = QFileDialog::getExistingDirectory(this, tr("Директория установки CooCox IDE"), QApplication::applicationDirPath());
    Paths::instance()->setCoIdeDir(str);
    ui->lineEditIdePath->setText(str);
    refreshData();
}

//------------------------------------------------------------------------------
// Отобразить только компоненты связанные с выбранным производителем
//------------------------------------------------------------------------------
void MainForm::showFilteredComponents(int itemIndex)
{
    //Q_UNUSED(itemIndex);

    if(itemIndex <= 0)
    {
        return;
    }

    int manufacturerId = extractIdFromItemText(ui->comboBoxManufacturer->currentText());
    int familyId = extractIdFromItemText(ui->comboBoxFamily->currentText());
    int serieId = extractIdFromItemText(ui->comboBoxSerie->currentText());
    int mcuId = extractIdFromItemText(ui->comboBoxMcu->currentText());

    //Если есть фильтр хотя бы по какому-либо параметру
//    if(manufacturerId >= 0 ||
//       familyId >= 0 ||
//       serieId >= 0 ||
//       mcuId >= 0)
    {
        ui->listWidgetComponents->clear();
#if 0
        QMap<int, Component>::iterator compIterator = componInfo.components()->begin();

        while(compIterator != componInfo.components()->end())
        {
            QString itemText;
            Component currComponent = compIterator.value();
            QList<int> mcuList = currComponent.getMcuListId();
            QList<int> mcuFamilyList = currComponent.getMcuFamilyList();
            QList<int> mcuSeriesList = currComponent.getMcuSeriesList();
            QList<int> mcuManufacturerList = currComponent.getMcuManufacturerList();

            //Компонент не подходит по производителю
            if(manufacturerId >= 0 && !mcuManufacturerList.contains(manufacturerId))
            {
                ++compIterator;
                continue;
            }

            //Компонент не подходит по семейству
            if(familyId >= 0 && !mcuFamilyList.contains(familyId))
            {
                ++compIterator;
                continue;
            }

            //Компонент не подходит по серии
            if(serieId >= 0 && !mcuSeriesList.contains(serieId))
            {
                ++compIterator;
                continue;
            }

            //Компонент не подходит по чипу
            if(mcuId >= 0 && !mcuList.contains(mcuId))
            {
                ++compIterator;
                continue;
            }

            itemText = QString("%1 - %2").
                        arg(currComponent.getId()).
                        arg(currComponent.getName());

            ui->listWidgetComponents->addItem(itemText);
            ++compIterator;
        }
#endif
    }

    ui->labelComponentsCount->setNum(ui->listWidgetComponents->count());

    return;
}

//------------------------------------------------------------------------------
// Вытащить идентификатор из текущего поля списка
//------------------------------------------------------------------------------
int MainForm::extractIdFromItemText(QString text)
{
    int id = -1;
    bool status = false;

    QString idSubStr = text.mid(0, text.indexOf(" - "));
    id = idSubStr.toInt(&status);

    if(!status)
    {
        id = -1;
    }

    return id;
}

//------------------------------------------------------------------------------
// Вытащить имя из текущего поля списка
//------------------------------------------------------------------------------
QString MainForm::extractNameFromItemText(QString text)
{
    return text.mid(text.indexOf(" - ") + 3);
}

void MainForm::on_pushButtonDbOptimize_clicked()
{
    ui->pushButtonDbOptimize->setEnabled(false);

    DBGarbageCollector gbCollector;
    gbCollector.deleteUnnecessaryTables();
    gbCollector.deleteObsoleteData();


    ui->pushButtonDbOptimize->setEnabled(true);
}
