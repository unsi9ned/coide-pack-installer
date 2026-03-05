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


SOURCES += src/main.cpp\
    src/pdscparser.cpp \
    src/packdescription.cpp \
    src/memory.cpp \
    src/mainform.cpp \
    src/devicefeature.cpp \
    src/featurecontainer.cpp \
    src/algorithmcontainer.cpp \
    src/progalgorithm.cpp \
    src/debugalgorithm.cpp \
    src/paths.cpp \
    src/packmanager.cpp \
    src/ziparchive.cpp \
    src/manufacturer.cpp \
    src/logger.cpp \
    src/database.cpp \
    src/requestmanager.cpp \
    src/component.cpp \
    src/componentsinfo.cpp \
    src/category.cpp \
    src/family.cpp \
    src/series.cpp \
    src/mcu.cpp \
    src/dbgarbagecollector.cpp \
    src/example.cpp \
    src/pdsc/pdscelement.cpp \
    src/pdsc/pdscfile.cpp \
    src/pdsc/pdsccomponentattr.cpp \
    src/pdsc/pdsccomponent.cpp \
    src/pdsc/pdsccondition.cpp \
    src/pdsc/pdscrequirement.cpp \
    src/makelink.cpp \
    src/settings.cpp

HEADERS  += \
    src/pdscparser.h \
    src/mcu.h \
    src/debugalgorithm.h \
    src/family.h \
    src/manufacturer.h \
    src/packdescription.h \
    src/series.h \
    src/memory.h \
    src/mainform.h \
    src/qclickablelabel.h \
    src/devicefeature.h \
    src/featurecontainer.h \
    src/algorithmcontainer.h \
    src/progalgorithm.h \
    src/paths.h \
    src/packmanager.h \
    src/ziparchive.h \
    src/logger.h \
    src/database.h \
    src/requestmanager.h \
    src/component.h \
    src/componentsinfo.h \
    src/category.h \
    src/constants.h \
    src/dbgarbagecollector.h \
    src/example.h \
    src/pdsc/pdscelement.h \
    src/pdsc/pdscfile.h \
    src/pdsc/pdscfilecategory.h \
    src/pdsc/pdsccomponentattr.h \
    src/pdsc/pdsccomponent.h \
    src/pdsc/pdsccondition.h \
    src/pdsc/pdscrequirement.h \
    src/makelink.h \
    src/settings.h

FORMS    += \
    src/mainform.ui

RESOURCES += \
    resources/resources.qrc

INCLUDEPATH += \
    src \

# Копирует utils в каталог сборки (debug/release)

# Определяем исходный путь
UTILS_SRC = $$PWD/utils
CMSIS_SRC = $$PWD/CMSIS

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
CMSIS_DST = $$OUT_PWD/$$BUILD_TYPE/CMSIS

# Отладочная информация
# message("Copying utils from: $$UTILS_SRC")
# message("Copying utils to: $$UTILS_DST")
# message("Build type: $$BUILD_TYPE")

# Создаем команду копирования
win32 {
    # Для Windows - используем QMAKE_COPY_DIR
    copy_cmd = $$QMAKE_COPY_DIR \"$$shell_path($$UTILS_SRC)\" \"$$shell_path($$UTILS_DST)\"
    QMAKE_POST_LINK += $$copy_cmd
    copy_cmd = && $$QMAKE_COPY_DIR \"$$shell_path($$CMSIS_SRC)\" \"$$shell_path($$CMSIS_DST)\"
    QMAKE_POST_LINK += $$copy_cmd
}

# Для Linux/macOS
unix {
    copy_cmd = cp -rf \"$$UTILS_SRC\" \"$$UTILS_DST\"
    QMAKE_POST_LINK += $$copy_cmd
}
