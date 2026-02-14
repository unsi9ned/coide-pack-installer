#-------------------------------------------------
#
# Project created by QtCreator 2026-02-06T15:03:55
#
#-------------------------------------------------

QT       += core gui xml sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CoIDE_PackInstaller
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
    pdscparser.cpp \
    packdescription.cpp \
    memory.cpp \
    mainform.cpp \
    devicefeature.cpp \
    featurecontainer.cpp \
    algorithmcontainer.cpp \
    progalgorithm.cpp \
    debugalgorithm.cpp \
    paths.cpp \
    packmanager.cpp \
    ziparchive.cpp \
    manufacturer.cpp \
    logger.cpp \
    database.cpp \
    requestmanager.cpp \
    component.cpp \
    componentsinfo.cpp \
    category.cpp \
    family.cpp \
    series.cpp \
    mcu.cpp

HEADERS  += \
    pdscparser.h \
    mcu.h \
    debugalgorithm.h \
    family.h \
    manufacturer.h \
    packdescription.h \
    series.h \
    memory.h \
    mainform.h \
    qclickablelabel.h \
    devicefeature.h \
    featurecontainer.h \
    algorithmcontainer.h \
    progalgorithm.h \
    paths.h \
    packmanager.h \
    ziparchive.h \
    logger.h \
    database.h \
    requestmanager.h \
    component.h \
    componentsinfo.h \
    category.h \
    constants.h

FORMS    += \
    mainform.ui

RESOURCES += \
    resources.qrc

# Копирует utils в каталог сборки (debug/release)

# Определяем исходный путь
UTILS_SRC = $$PWD/utils

# Определяем целевую папку в зависимости от конфигурации
CONFIG(debug, debug|release) {
    # Debug сборка
    BUILD_TYPE = debug
} else {
    # Release сборка
    BUILD_TYPE = release
}

# Путь назначения с учетом типа сборки
UTILS_DST = $$OUT_PWD/$$BUILD_TYPE/utils

# Отладочная информация
# message("Copying utils from: $$UTILS_SRC")
# message("Copying utils to: $$UTILS_DST")
# message("Build type: $$BUILD_TYPE")

# Создаем команду копирования
win32 {
    # Для Windows - используем QMAKE_COPY_DIR
    copy_cmd = $$QMAKE_COPY_DIR \"$$shell_path($$UTILS_SRC)\" \"$$shell_path($$UTILS_DST)\"
    QMAKE_POST_LINK += $$copy_cmd
}

# Для Linux/macOS
unix {
    copy_cmd = cp -rf \"$$UTILS_SRC\" \"$$UTILS_DST\"
    QMAKE_POST_LINK += $$copy_cmd
}
