#-------------------------------------------------
#
# Project created by QtCreator 2015-07-02T14:17:06
#
#-------------------------------------------------

QT       += core gui xml network axcontainer

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TVWallet1.2.0
TEMPLATE = app

QMAKE_LFLAGS += /MANIFESTUAC:\"level=\'requireAdministrator\' uiAccess=\'false\'\"


LIBS += -lDbgHelp
LIBS += User32.Lib
LIBS += -L$$PWD -lqrencode
LIBS += -limm32

SOURCES += main.cpp\
        firstlogin.cpp \
    normallogin.cpp \
    frame.cpp \
    mainpage.cpp \
    accountpage.cpp \
    transferpage.cpp \
    bottombar.cpp \
    setdialog.cpp \
    lockpage.cpp \
    consoledialog.cpp \
    titlebar.cpp \
    debug_log.cpp \
    contactdialog.cpp \
    waitingforsync.cpp \
    remarkdialog.cpp \
    functionbar.cpp \
    contactpage.cpp \
    singlecontactwidget.cpp \
    upgradepage.cpp \
    commondialog.cpp \
    showcontentdialog.cpp \
    namedialog.cpp \
    deleteaccountdialog.cpp \
    transferconfirmdialog.cpp \
    commontip.cpp \
    searchoptionwidget.cpp \
    rpcthread.cpp \
    selectgoppathwidget.cpp \
    newsdialog.cpp \
    addnodedialog.cpp \
    editremarkdialog.cpp \
    addcontactdialog.cpp \
    accountcellwidget.cpp \
    exportdialog.cpp \
    importdialog.cpp \
    ipcellwidget.cpp \
    incomecellwidget.cpp \
    control/myprogressbar.cpp \
    AES/aes.cpp \
    workerthread.cpp \
    workerthreadmanager.cpp \
    control/qrcodewidget.cpp \
    control/accountdetailwidget.cpp \
    control/showbottombarwidget.cpp \
    control/rightclickmenudialog.cpp \
    control/chooseaddaccountdialog.cpp \
    dialog/renamedialog.cpp \
    extra/dynamicmove.cpp \
    control/remarkcellwidget.cpp \
    control/shadowwidget.cpp \
    tic.cpp \
    uploadparadialog.cpp \
    uploadbar.cpp \
    uploadpage.cpp \
    storagefilepage.cpp \
    downloadfilepage.cpp \
    storagefilebar.cpp \
    downloadbar.cpp

HEADERS  += firstlogin.h \
    normallogin.h \
    frame.h \
    mainpage.h \
    accountpage.h \
    transferpage.h \
    bottombar.h \
    setdialog.h \
    lockpage.h \
    consoledialog.h \
    debug_log.h \
    titlebar.h \
    contactdialog.h \
    waitingforsync.h \
    remarkdialog.h \
    functionbar.h \
    contactpage.h \
    singlecontactwidget.h \
    upgradepage.h \
    commondialog.h \
    showcontentdialog.h \
    namedialog.h \
    deleteaccountdialog.h \
    transferconfirmdialog.h \
    commontip.h \
    searchoptionwidget.h \
    rpcthread.h \
    selectgoppathwidget.h \
    newsdialog.h \
    addnodedialog.h \
    editremarkdialog.h \
    addcontactdialog.h \
    accountcellwidget.h \
    exportdialog.h \
    importdialog.h \
    ipcellwidget.h \
    incomecellwidget.h \
    control/myprogressbar.h \
    AES/aes.h \
    workerthread.h \
    workerthreadmanager.h \
    control/qrencode.h \
    control/qrcodewidget.h \
    control/accountdetailwidget.h \
    control/showbottombarwidget.h \
    control/rightclickmenudialog.h \
    control/chooseaddaccountdialog.h \
    dialog/renamedialog.h \
    extra/dynamicmove.h \
    control/remarkcellwidget.h \
    control/shadowwidget.h \
    tic.h \
    tic_common_define.h \
    uploadparadialog.h \
    uploadbar.h \
    uploadpage.h \
    storagefilepage.h \
    downloadfilepage.h \
    storagefilebar.h \
    downloadbar.h

FORMS    += firstlogin.ui \
    normallogin.ui \
    mainpage.ui \
    accountpage.ui \
    transferpage.ui \
    bottombar.ui \
    setdialog.ui \
    lockpage.ui \
    consoledialog.ui \
    titlebar.ui \
    contactdialog.ui \
    waitingforsync.ui \
    remarkdialog.ui \
    functionbar.ui \
    contactpage.ui \
    singlecontactwidget.ui \
    upgradepage.ui \
    commondialog.ui \
    showcontentdialog.ui \
    namedialog.ui \
    deleteaccountdialog.ui \
    transferconfirmdialog.ui \
    commontip.ui \
    searchoptionwidget.ui \
    selectgoppathwidget.ui \
    newsdialog.ui \
    addnodedialog.ui \
    editremarkdialog.ui \
    addcontactdialog.ui \
    accountcellwidget.ui \
    exportdialog.ui \
    importdialog.ui \
    ipcellwidget.ui \
    incomecellwidget.ui \
    control/accountdetailwidget.ui \
    control/rightclickmenudialog.ui \
    control/chooseaddaccountdialog.ui \
    dialog/renamedialog.ui \
    control/remarkcellwidget.ui \
    control/shadowwidget.ui \
    #loading.ui
    uploadparadialog.ui \
    uploadbar.ui \
    uploadpage.ui \
    storagefilepage.ui \
    downloadfilepage.ui \
    storagefilebar.ui \
    downloadbar.ui

DISTFILES += \
    logo.rc

RC_FILE = logo.rc

RESOURCES += \
    tic.qrc

TRANSLATIONS +=   gop_simplified_Chinese.ts  gop_English.ts
