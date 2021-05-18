TEMPLATE = app
WORKPATH = .
QT += quick qml
SOURCES +=  ui/main.cpp \
            ui/qcasualvm.cpp\
            ui/datasource.cpp \
            ui/sipslistmodel.cpp
SOURCES +=core/scookies.cpp
SOURCES +=core/shttpproxies.cpp
SOURCES +=core/scurlclient.cpp
SOURCES +=core/shttputil.cpp
SOURCES +=core/scloud_vm.cpp
SOURCES +=core/scurlholder.cpp
SOURCES +=core/smultipart_parser.cpp
SOURCES +=core/scloud_xml_parser.cpp
SOURCES +=core/shttperror.cpp
INCLUDEPATH += ${WORKPATH}/core
INCLUDEPATH += $$[BOOST_PATH]
VPATH += $$[WORKPATH]/core
#system(echo $$[VPATH] )
#EXAMPLE_FILES = \
#    window.qml \
#    resources

target.path = ./bin
INSTALLS += target


RESOURCES += hp_player-qml.qrc \
    hp_player-qml.qrc
#ICON = resources/icon64.png
#macx: ICON = resources/icon.icns
#win32: RC_FILE = resources/window.rc
LIBS += -lcurl
LIBS += -lssl
LIBS += -lcrypto
DISTFILES += \
    Button.qml \
    home.qml

HEADERS += \
    ui/qcasualvm.h\
    ui/qcomponents.hpp\
    ui/layouts.h\
    ui/datasource.h \
    ui/genericmodel.h \
    ui/rangealgorithm.h \
    ui/s_type_traits.h \
    ui/sipslistmodel.h \
    ui/spropertyaccessor.h
DESTPATH = $$[WORKPATH]/bin/
target.path = $$DESTPATH

qml.files = *.qml
qml.path = $$DESTPATH