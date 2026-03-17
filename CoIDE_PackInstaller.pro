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
    src/models/pack/pdscparser.cpp \
    src/models/pack/packdescription.cpp \
    src/models/pack/packmanager.cpp \
    src/models/mcu/memory.cpp \
    src/views/mainform.cpp \
    src/models/mcu/devicefeature.cpp \
    src/models/mcu/featurecontainer.cpp \
    src/models/algorithms/algorithmcontainer.cpp \
    src/models/algorithms/progalgorithm.cpp \
    src/models/algorithms/debugalgorithm.cpp \
    src/services/paths.cpp \
    src/utils/ziparchive.cpp \
    src/models/mcu/manufacturer.cpp \
    src/services/logger.cpp \
    src/models/database/database.cpp \
    src/models/database/requestmanager.cpp \
    src/models/components/component.cpp \
    src/models/database/componentsinfo.cpp \
    src/models/components/category.cpp \
    src/models/mcu/family.cpp \
    src/models/mcu/series.cpp \
    src/models/mcu/mcu.cpp \
    src/models/database/dbgarbagecollector.cpp \
    src/models/components/example.cpp \
    src/models/pdsc/pdscelement.cpp \
    src/models/pdsc/pdscfile.cpp \
    src/models/pdsc/pdsccomponentattr.cpp \
    src/models/pdsc/pdsccomponent.cpp \
    src/models/pdsc/pdsccondition.cpp \
    src/models/pdsc/pdscrequirement.cpp \
    src/utils/makelink.cpp \
    src/services/settings.cpp \
    src/viewmodels/mcubrowserviewmodel.cpp \
    src/viewmodels/mcudetailsviewmodel.cpp \
    src/viewmodels/mainviewmodel.cpp \
    src/viewmodels/deviceviewmodel.cpp

HEADERS  += \
    src/models/pack/pdscparser.h \
    src/models/pack/packdescription.h \
    src/models/pack/packmanager.h \
    src/models/mcu/mcu.h \
    src/models/algorithms/debugalgorithm.h \
    src/models/mcu/family.h \
    src/models/mcu/manufacturer.h \
    src/models/mcu/series.h \
    src/models/mcu/memory.h \
    src/views/mainform.h \
    src/views/widgets/qclickablelabel.h \
    src/models/mcu/devicefeature.h \
    src/models/mcu/featurecontainer.h \
    src/models/algorithms/algorithmcontainer.h \
    src/models/algorithms/progalgorithm.h \
    src/services/paths.h \
    src/utils/ziparchive.h \
    src/services/logger.h \
    src/models/database/database.h \
    src/models/database/requestmanager.h \
    src/models/components/component.h \
    src/models/database/componentsinfo.h \
    src/models/components/category.h \
    src/common/constants.h \
    src/models/database/dbgarbagecollector.h \
    src/models/components/example.h \
    src/models/pdsc/pdscelement.h \
    src/models/pdsc/pdscfile.h \
    src/models/pdsc/pdscfilecategory.h \
    src/models/pdsc/pdsccomponentattr.h \
    src/models/pdsc/pdsccomponent.h \
    src/models/pdsc/pdsccondition.h \
    src/models/pdsc/pdscrequirement.h \
    src/utils/makelink.h \
    src/services/settings.h \
    src/viewmodels/mcubrowserviewmodel.h \
    src/viewmodels/mcudetailsviewmodel.h \
    src/viewmodels/mainviewmodel.h \
    src/utils/versionhelper.h \
    src/viewmodels/deviceviewmodel.h

FORMS    += \
    src/views/mainform.ui

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
