# server.pro - Файл проекта сервера
# TCP сервер для решения математических уравнений

QT += core network sql      # Подключаем модули: ядро, сеть, базы данных
CONFIG += console            # Консольное приложение
CONFIG -= app_bundle         # Не создавать bundle на macOS
CONFIG += c++11              # Используем стандарт C++11

TARGET = server              # Имя исполняемого файла
TEMPLATE = app               # Шаблон приложения

# Определения для предпроцессора
DEFINES += QT_DEPRECATED_WARNINGS  # Предупреждения об устаревших API

# Исходные файлы
SOURCES += \
    main.cpp \
    controller/servercontroller.cpp \
    view/serverview.cpp \
    model/databasemanager.cpp \
    model/functionSolver.cpp

# Заголовочные файлы
HEADERS += \
    controller/servercontroller.h \
    view/serverview.h \
    model/databasemanager.h \
    model/functionSolver.h

# Цель для генерации Doxygen документации
doctarget.target = docs
doctarget.commands = doxygen Doxyfile
doctarget.depends = $$HEADERS $$SOURCES
QMAKE_EXTRA_TARGETS += doctarget

# Информация о версии (опционально)
VERSION = 1.0.0
DEFINES += APP_VERSION=\\\"$$VERSION\\\"

# Настройки для разных платформ
unix:!macx {
    # Linux специфичные настройки
    QMAKE_LFLAGS += -no-pie
}

# Вывод сообщения о конфигурации
message("Building server version $$VERSION")
message("Qt version: $$[QT_VERSION]")
