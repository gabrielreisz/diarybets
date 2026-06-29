QT += core gui sql charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

TARGET = Diarybetes
TEMPLATE = app

# Diretórios
INCLUDEPATH += include
INCLUDEPATH += gui

# Headers do núcleo (compartilhado com a versão de terminal) + GUI
HEADERS += \
    include/Person.hpp \
    include/Patient.hpp \
    include/Time.hpp \
    include/Medication.hpp \
    include/MedicationRecord.hpp \
    include/HealthRecord.hpp \
    include/ExamRecord.hpp \
    include/ConsultationRecord.hpp \
    include/GlucoseRecord.hpp \
    include/MealPlan.hpp \
    include/DatabaseMethods.hpp \
    include/Security.hpp \
    include/Console.hpp \
    include/GlucoseAnalytics.hpp \
    gui/MainWindow.hpp

# Fontes do núcleo (mesmos arquivos usados pelo executável de terminal) + GUI.
# Security e GlucoseAnalytics são obrigatórios: Patient/DatabaseMethods
# dependem de Security; a GUI reutiliza GlucoseAnalytics para gráficos/painéis.
SOURCES += \
    src/Person.cpp \
    src/Patient.cpp \
    src/Time.cpp \
    src/Medication.cpp \
    src/MedicationRecord.cpp \
    src/HealthRecord.cpp \
    src/ExamRecord.cpp \
    src/ConsultationRecord.cpp \
    src/GlucoseRecord.cpp \
    src/MealPlan.cpp \
    src/DatabaseMethods.cpp \
    src/Security.cpp \
    src/Console.cpp \
    src/GlucoseAnalytics.cpp \
    src/sqlite3.c \
    gui/main_gui.cpp \
    gui/MainWindow.cpp

# Forms
FORMS += \
    ui/forms/MainWindow.ui

# Flags
QMAKE_CXXFLAGS += -Wall -Wextra

# Saída isolada em build-gui/ para nao colidir com o build de terminal
# (que usa obj/ e o Makefile escrito a mao). Gere o makefile da GUI com um
# nome proprio:  qmake6 -o Makefile.gui Diarybetes.pro
DESTDIR = $$PWD/build-gui/bin
OBJECTS_DIR = $$PWD/build-gui/obj
MOC_DIR = $$PWD/build-gui/moc
UI_DIR = $$PWD/build-gui/ui_generated
RCC_DIR = $$PWD/build-gui/rcc

