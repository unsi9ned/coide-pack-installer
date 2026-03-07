#include <QScrollBar>
#include <QTimer>
#include "mainform.h"
#include "ui_mainform.h"
#include "services/paths.h"
#include "utils/ziparchive.h"
#include "models/pack/packmanager.h"
#include "models/database/dbgarbagecollector.h"
#include "services/settings.h"

MainForm::MainForm(QWidget *parent) :
    QMainWindow(parent),
    m_viewModel(new MainViewModel(this)),
    ui(new Ui::MainForm)
{
    ui->setupUi(this);

    //Иконка
    this->setWindowIcon(QIcon(":coide_project.ico"));

    //Версия программы
    ui->labelAppVersion->setText(compilationVersion());
    ui->lineEditIdePath->setText(Paths::instance()->coIdeDir());

    ui->plainTextEdit->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(ui->plainTextEdit,
            &QPlainTextEdit::customContextMenuRequested,
            this,
            &MainForm::showLogContextMenu);

    // Вывод сообщений
    connect(&packMgr,
            SIGNAL(errorOccured(QString)),
            SLOT(printLogMessages(QString)));

    connect(&packMgr,
            SIGNAL(eventOccured(QString)),
            SLOT(printLogMessages(QString)));

    // Изменение пути к каталогу CoIDE
    connect(ui->actionPreferences, SIGNAL(triggered(bool)), SLOT(changeCoIDEPath()));

    // Выбор пакета DFP и загрузка
    connect(ui->actionOpen_DFP, SIGNAL(triggered(bool)), SLOT(loadDFP()));

    //--------------------------------------------------------------------------
    // Сигналы загрузки
    connect(m_viewModel, &MainViewModel::loadStarted, [this]()
    {
        ui->statusBar->showMessage("Загрузка...");
        ui->plainTextEditFeatures->clear();
        ui->plainTextEditDescription->clear();
    });

    connect(m_viewModel, &MainViewModel::loadFinished, [this]() {
        ui->lineEditRelease->setText(m_viewModel->releaseVersion());
        ui->statusBar->showMessage("Готово", 3000);
    });

    connect(m_viewModel, &MainViewModel::loadFailed, [this](const QString& error) {
        QMessageBox::critical(this, "Ошибка", error);
    });

    connect(m_viewModel, &MainViewModel::statusMessage,
            ui->statusBar, &QStatusBar::showMessage);


    // Обновление списков
    connect(m_viewModel, &MainViewModel::vendorsChanged, [this]() {
        ui->listWidgetManufacturer->clear();
        ui->listWidgetManufacturer->addItems(m_viewModel->vendors());
    });

    connect(m_viewModel, &MainViewModel::familiesChanged, [this]() {
        ui->listWidgetFamily->clear();
        ui->listWidgetFamily->addItems(m_viewModel->families());

        if(!m_viewModel->currentVendor().isEmpty())
        {
            auto items = ui->listWidgetManufacturer->findItems(m_viewModel->currentVendor(), Qt::MatchExactly);

            if(!items.isEmpty())
            {
                ui->listWidgetManufacturer->setCurrentItem(items.first());
            }
        }
    });

    connect(m_viewModel, &MainViewModel::seriesChanged, [this]() {
        ui->listWidgetSeries->clear();
        ui->listWidgetSeries->addItems(m_viewModel->series());

        if(!m_viewModel->currentFamily().isEmpty())
        {
            auto items = ui->listWidgetFamily->findItems(m_viewModel->currentFamily(), Qt::MatchExactly);

            if(!items.isEmpty())
            {
                ui->listWidgetFamily->setCurrentItem(items.first());
            }
        }
    });

    connect(m_viewModel, &MainViewModel::mcusChanged, [this]()
    {
        ui->listWidgetMcu->clear();
        ui->listWidgetMcu->addItems(m_viewModel->mcus());

        if(!m_viewModel->currentSeries().isEmpty())
        {
            auto items = ui->listWidgetSeries->findItems(m_viewModel->currentSeries(), Qt::MatchExactly);

            if(!items.isEmpty())
            {
                ui->listWidgetSeries->setCurrentItem(items.first());
            }
        }
    });


    // Обновление детальной информации
    connect(m_viewModel, &MainViewModel::mcuChanged, [this]()
    {
        ui->lineEditFlashStart->setText(m_viewModel->flashStart());
        ui->lineEditFlashSize->setText(m_viewModel->flashSize());
        ui->lineEditRamStart->setText(m_viewModel->ramStart());
        ui->lineEditRamSize->setText(m_viewModel->ramSize());
        ui->plainTextEditFeatures->setPlainText(m_viewModel->features());
        ui->plainTextEditDescription->setPlainText(m_viewModel->description());
        ui->lineEditUrl->setText(m_viewModel->webPageUrl());
        ui->lineEditDatasheetUrl->setText(m_viewModel->datasheetUrl());
        ui->lineEditSVD->setText(m_viewModel->svdLocalPath());

        // Заполнение списка алгоритмов отладки
        ui->comboBoxDebugAlg->clear();

        if(!m_viewModel->debugAlgorithm().isEmpty())
        {
            ui->comboBoxDebugAlg->addItem(m_viewModel->debugAlgorithm());
            ui->comboBoxDebugAlg->setCurrentIndex(0);
        }

        // Заполнение списка алгоритмов программирования
        ui->comboBoxFlashAlg->clear();

        for(int i = 0; i < m_viewModel->flashAlgorithms().count(); i++)
        {
            QString algo = m_viewModel->flashAlgorithms().at(i);

            ui->comboBoxFlashAlg->addItem(algo);

            if(algo == m_viewModel->currentFlashAlgorithm())
            {
                ui->comboBoxFlashAlg->setCurrentIndex(i);
            }
        }

        // Восстанавливаем выбор, если элемент еще существует
        if (!m_viewModel->currentMcu().isEmpty())
        {
            auto items = ui->listWidgetMcu->findItems(m_viewModel->currentMcu(), Qt::MatchExactly);
            if (!items.isEmpty())
            {
                ui->listWidgetMcu->setCurrentItem(items.first());
            }
        }
    });


    // Пользовательский ввод
    //--------------------------------------------------------------------------

    // Выбор производителя
    connect(ui->listWidgetManufacturer, &QListWidget::currentRowChanged, [this]()
    {
        QListWidgetItem * currentItem = ui->listWidgetManufacturer->currentItem();

        if(currentItem)
        {
            m_viewModel->selectVendor(currentItem->text());
        }
    });

    connect(ui->listWidgetFamily, &QListWidget::currentRowChanged, [this]()
    {
        QListWidgetItem * currentItem = ui->listWidgetFamily->currentItem();

        if(currentItem)
        {
            m_viewModel->selectFamily(currentItem->text());
        }
    });

    connect(ui->listWidgetSeries, &QListWidget::currentRowChanged, [this]()
    {
        QListWidgetItem * currentItem = ui->listWidgetSeries->currentItem();

        if(currentItem)
        {
            m_viewModel->selectSeries(currentItem->text());
        }
    });

    connect(ui->listWidgetMcu, &QListWidget::currentRowChanged, [this]()
    {
        QListWidgetItem * currentItem = ui->listWidgetMcu->currentItem();

        if(currentItem)
        {
            m_viewModel->selectMcu(currentItem->text());
        }
    });

    // Отладка
    //--------------------------------------------------------------------------
#if 0
    connect(ui->listWidgetManufacturer, &QListWidget::currentRowChanged, [this]() {
       qInfo()  << "manufacturerChanged";
    });

    connect(ui->listWidgetFamily, &QListWidget::currentRowChanged, [this]() {
       qInfo()  << "familyChanged";
    });

    connect(ui->listWidgetSeries, &QListWidget::currentRowChanged, [this]() {
       qInfo()  << "seriesChanged";
    });

    connect(ui->listWidgetMcu, &QListWidget::currentRowChanged, [this]() {
       qInfo()  << "mcuChanged";
    });
#endif
    //--------------------------------------------------------------------------

#if 1
    QMetaObject::invokeMethod(this, "delayedInit", Qt::QueuedConnection);
#else
    delayedInit();
#endif
}

//------------------------------------------------------------------------------
// Деструктор
//------------------------------------------------------------------------------
MainForm::~MainForm()
{
    delete m_viewModel;
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
        date_part += compilation_date.mid(4, 2).replace(' ', '0');
    }

    time_part = time_part.mid(0, 2) + time_part.mid(3, 2);

    return date_part + "." + time_part;
}

//------------------------------------------------------------------------------
// Инициализация программы после прогрузки основного окна
//------------------------------------------------------------------------------
void MainForm::delayedInit()
{
    //Обновление данных на форме
    m_viewModel->loadDeviceFamilyPack();
}

//------------------------------------------------------------------------------
// Отобразить ошибку
//------------------------------------------------------------------------------
void MainForm::showErrorMessage(QString e)
{
    QMessageBox::critical(this, tr("Ошибка"), e, QMessageBox::Ok);
}

//------------------------------------------------------------------------------
// Отобразить информацию
//------------------------------------------------------------------------------
void MainForm::showInfoMessage(QString i)
{
    QMessageBox::information(this, tr("Информация"), i, QMessageBox::Ok);
}

//------------------------------------------------------------------------------
// Загрузка данных из базы
//------------------------------------------------------------------------------
void MainForm::on_pushButtonDataLoad_clicked()
{
    ui->pushButtonDataLoad->setEnabled(false);
    m_viewModel->loadDeviceFamilyPack();
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
}

//------------------------------------------------------------------------------
// Задать путь к среде разработки
//------------------------------------------------------------------------------
void MainForm::on_pushButtonSetIdePath_clicked()
{
    QString str = QFileDialog::getExistingDirectory(this,
                                                    tr("Директория установки CooCox IDE"),
                                                    QApplication::applicationDirPath());
    Paths::instance()->setCoIdeDir(str);
    ui->lineEditIdePath->setText(str);
}

//------------------------------------------------------------------------------
// Запуск оптимизации банных в БД
//------------------------------------------------------------------------------
void MainForm::on_pushButtonDbOptimize_clicked()
{
    ui->pushButtonDbOptimize->setEnabled(false);

    DBGarbageCollector gbCollector;

    connect(&gbCollector, &DBGarbageCollector::errorOccured, this, &MainForm::printLogMessages);
    connect(&gbCollector, &DBGarbageCollector::eventOccured, this, &MainForm::printLogMessages);

    gbCollector.deleteUnnecessaryTables();
    gbCollector.deleteObsoleteData();

    disconnect(&gbCollector, &DBGarbageCollector::errorOccured, this, &MainForm::printLogMessages);
    disconnect(&gbCollector, &DBGarbageCollector::eventOccured, this, &MainForm::printLogMessages);

    ui->pushButtonDbOptimize->setEnabled(true);
}

//------------------------------------------------------------------------------
// Расширенное контекстное меню для окна логирования
//------------------------------------------------------------------------------
void MainForm::showLogContextMenu(const QPoint &pos)
{
    QMenu *menu = ui->plainTextEdit->createStandardContextMenu();

    menu->addSeparator();

    QAction *clearAction = menu->addAction("Clear");
    clearAction->setIcon(style()->standardIcon(QStyle::SP_DialogResetButton));

    connect(clearAction, &QAction::triggered,
            ui->plainTextEdit, &QPlainTextEdit::clear);

    menu->exec(ui->plainTextEdit->mapToGlobal(pos));
    delete menu;
}

//------------------------------------------------------------------------------
// Изменение пути к каталогу CoIDE
//------------------------------------------------------------------------------
void MainForm::changeCoIDEPath()
{
    QString str = QFileDialog::getExistingDirectory(this,
                                                    tr("Директория установки CooCox IDE"),
                                                    Paths::instance()->coIdeDir());
    if(!str.isEmpty())
    {
        Paths::instance()->setCoIdeDir(str);
        ui->lineEditIdePath->setText(str);
    }
}

//------------------------------------------------------------------------------
// Выбор и загрузка DFP
//------------------------------------------------------------------------------
void MainForm::loadDFP()
{
    QFileInfo packFileInfo(Settings::instance()->lastLoadedPack());
    QFileDialog dialog(this, tr("Device Family Pack"), packFileInfo.path(), "*.pack");

    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setOption(QFileDialog::DontUseNativeDialog);

    if (dialog.exec())
    {
        QString path = dialog.selectedFiles().first();
        Settings::instance()->saveLastLoadedPack(path);
        m_viewModel->loadDeviceFamilyPack();
    }
}

//------------------------------------------------------------------------------
// Печать сообщений об ошибках и событиях
//------------------------------------------------------------------------------
void MainForm::printLogMessages(QString msg)
{
    ui->plainTextEdit->appendPlainText(msg);
    ui->plainTextEdit->verticalScrollBar()->setValue(ui->plainTextEdit->verticalScrollBar()->maximum());
}
