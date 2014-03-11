#-------------------------------------------------
#
# Project created by QtCreator 2013-05-07T10:27:12
#
#-------------------------------------------------

QT       += core gui network xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SATIP_Light_Client
TEMPLATE = app
DEFINES += _GNU_SOURCE
INCLUDEPATH += \
#    ../libdvbpsi/src \
#    ../libdvbpsi/src/descriptors \
#    ../libdvbpsi/src/tables \
    ../UDP-Listener \
    ortp/include \
    ortp/src
ICON = tv.icns

SOURCES += \
#    ../libdvbpsi/src/descriptors/dr_02.c \
#    ../libdvbpsi/src/descriptors/dr_03.c \
#    ../libdvbpsi/src/descriptors/dr_04.c \
#    ../libdvbpsi/src/descriptors/dr_05.c \
#    ../libdvbpsi/src/descriptors/dr_06.c \
#    ../libdvbpsi/src/descriptors/dr_07.c \
#    ../libdvbpsi/src/descriptors/dr_08.c \
#    ../libdvbpsi/src/descriptors/dr_09.c \
#    ../libdvbpsi/src/descriptors/dr_0a.c \
#    ../libdvbpsi/src/descriptors/dr_0b.c \
#    ../libdvbpsi/src/descriptors/dr_0c.c \
#    ../libdvbpsi/src/descriptors/dr_0d.c \
#    ../libdvbpsi/src/descriptors/dr_0e.c \
#    ../libdvbpsi/src/descriptors/dr_0f.c \
#    ../libdvbpsi/src/descriptors/dr_13.c \
#    ../libdvbpsi/src/descriptors/dr_14.c \
#    ../libdvbpsi/src/descriptors/dr_40.c \
#    ../libdvbpsi/src/descriptors/dr_41.c \
#    ../libdvbpsi/src/descriptors/dr_42.c \
#    ../libdvbpsi/src/descriptors/dr_43.c \
#    ../libdvbpsi/src/descriptors/dr_44.c \
#    ../libdvbpsi/src/descriptors/dr_45.c \
#    ../libdvbpsi/src/descriptors/dr_47.c \
#    ../libdvbpsi/src/descriptors/dr_48.c \
#    ../libdvbpsi/src/descriptors/dr_49.c \
#    ../libdvbpsi/src/descriptors/dr_4a.c \
#    ../libdvbpsi/src/descriptors/dr_4b.c \
#    ../libdvbpsi/src/descriptors/dr_4d.c \
#    ../libdvbpsi/src/descriptors/dr_4e.c \
#    ../libdvbpsi/src/descriptors/dr_4f.c \
#    ../libdvbpsi/src/descriptors/dr_50.c \
#    ../libdvbpsi/src/descriptors/dr_52.c \
#    ../libdvbpsi/src/descriptors/dr_54.c \
#    ../libdvbpsi/src/descriptors/dr_55.c \
#    ../libdvbpsi/src/descriptors/dr_56.c \
#    ../libdvbpsi/src/descriptors/dr_58.c \
#    ../libdvbpsi/src/descriptors/dr_59.c \
#    ../libdvbpsi/src/descriptors/dr_5a.c \
#    ../libdvbpsi/src/descriptors/dr_62.c \
#    ../libdvbpsi/src/descriptors/dr_66.c \
#    ../libdvbpsi/src/descriptors/dr_69.c \
#    ../libdvbpsi/src/descriptors/dr_73.c \
#    ../libdvbpsi/src/descriptors/dr_76.c \
#    ../libdvbpsi/src/descriptors/dr_7c.c \
#    ../libdvbpsi/src/descriptors/dr_83.c \
#    ../libdvbpsi/src/descriptors/dr_8a.c \
#    ../libdvbpsi/src/demux.c \
#    ../libdvbpsi/src/descriptor.c \
#    ../libdvbpsi/src/dvbpsi.c \
#    ../libdvbpsi/src/psi.c \
#    ../libdvbpsi/src/tables/atsc_eit.c \
#    ../libdvbpsi/src/tables/atsc_ett.c \
#    ../libdvbpsi/src/tables/atsc_mgt.c \
#    ../libdvbpsi/src/tables/atsc_stt.c \
#    ../libdvbpsi/src/tables/atsc_vct.c \
#    ../libdvbpsi/src/tables/bat.c \
#    ../libdvbpsi/src/tables/cat.c \
#    ../libdvbpsi/src/tables/eit.c \
#    ../libdvbpsi/src/tables/nit.c \
#    ../libdvbpsi/src/tables/pat.c \
#    ../libdvbpsi/src/tables/pmt.c \
#    ../libdvbpsi/src/tables/rst.c \
#    ../libdvbpsi/src/tables/sdt.c \
#    ../libdvbpsi/src/tables/sis.c \
#    ../libdvbpsi/src/tables/tot.c \
#    ../UDP-Listener/abstractdvbpsihandler.cpp \
#    ../UDP-Listener/pathandler.cpp \
#    ../UDP-Listener/pmthandler.cpp \
#    ../UDP-Listener/sdthandler.cpp \
#    ../UDP-Listener/demuxhandler.cpp \
#    ../UDP-Listener/nithandler.cpp \
#    ../UDP-Listener/eithandler.cpp \
#    ../UDP-Listener/abstractdvbpsihandler_p.cpp \
#    ../UDP-Listener/contentdiscoverymodel.cpp \
    main.cpp\
    mainwindow.cpp \
    gatewaydevice.cpp \
    gatewaywidget.cpp \
    dvbtconfig.cpp \
    dvbsconfig.cpp \
    rtspsession.cpp \
    sessionmodel.cpp \
    addsessiondialog.cpp \
    sessionview.cpp \
    scandialog.cpp \
    scanprocedure.cpp \
    rtcpreport.cpp \
    program.cpp \
    programmodel.cpp \
    settings.cpp \
    scanwidget.cpp \
    sessionwidget.cpp \
    udpforwarddialog.cpp \
    ssdpclient.cpp \
    epgcollector.cpp \
    objectpool.cpp \
    epgview.cpp \
    eventitem.cpp \
    epgwidget.cpp \
    programitem.cpp \
    epgparser.cpp \
    event_p.cpp \
    event.cpp \
    table_p.cpp \
    severselectionwidget.cpp \
    serverinformationwidget.cpp \
    channellistview.cpp \
    channellistwidget.cpp \
    programrecorder.cpp \
    programrecorderrtsp.cpp \
    programrecordermodel.cpp \
    programrecorderview.cpp \
    programrecorderwidget.cpp \
    programtester.cpp \
    programtesterview.cpp \
    programtesterwidget.cpp \
    programtestermodel.cpp \
    demuxer.cpp \
    abstractpidhandler.cpp \
    patpidhandler.cpp \
    pmtpidhandler.cpp \
    sdtpidhandler.cpp \
    tableparser_p.cpp \
    section_p.cpp \
    ../UDP-Listener/dvbpsihelper.cpp \
    ../UDP-Listener/udptableview.cpp \
    ../UDP-Listener/udplistmodel.cpp \
    ../UDP-Listener/udplistener.cpp \
    ../UDP-Listener/rtpsocket.cpp \
    ../UDP-Listener/abstractdescriptor.cpp \
    ../UDP-Listener/elementarystream.cpp \
    ../UDP-Listener/rtcpsocket.cpp \
    zapchanneldialog.cpp \
    scanproceduredvbt.cpp \
    scanproceduredvbs.cpp \
    scansetupdvbtwidget.cpp \
    scansetupdvbswidget.cpp

HEADERS  += \
#    ../libdvbpsi/config.h \
#    ../libdvbpsi/src/descriptors/dr.h \
#    ../libdvbpsi/src/descriptors/dr_02.h \
#    ../libdvbpsi/src/descriptors/dr_03.h \
#    ../libdvbpsi/src/descriptors/dr_04.h \
#    ../libdvbpsi/src/descriptors/dr_05.h \
#    ../libdvbpsi/src/descriptors/dr_06.h \
#    ../libdvbpsi/src/descriptors/dr_07.h \
#    ../libdvbpsi/src/descriptors/dr_08.h \
#    ../libdvbpsi/src/descriptors/dr_09.h \
#    ../libdvbpsi/src/descriptors/dr_0a.h \
#    ../libdvbpsi/src/descriptors/dr_0b.h \
#    ../libdvbpsi/src/descriptors/dr_0c.h \
#    ../libdvbpsi/src/descriptors/dr_0d.h \
#    ../libdvbpsi/src/descriptors/dr_0e.h \
#    ../libdvbpsi/src/descriptors/dr_0f.h \
#    ../libdvbpsi/src/descriptors/dr_13.h \
#    ../libdvbpsi/src/descriptors/dr_14.h \
#    ../libdvbpsi/src/descriptors/dr_40.h \
#    ../libdvbpsi/src/descriptors/dr_41.h \
#    ../libdvbpsi/src/descriptors/dr_42.h \
#    ../libdvbpsi/src/descriptors/dr_43.h \
#    ../libdvbpsi/src/descriptors/dr_44.h \
#    ../libdvbpsi/src/descriptors/dr_45.h \
#    ../libdvbpsi/src/descriptors/dr_47.h \
#    ../libdvbpsi/src/descriptors/dr_48.h \
#    ../libdvbpsi/src/descriptors/dr_49.h \
#    ../libdvbpsi/src/descriptors/dr_4a.h \
#    ../libdvbpsi/src/descriptors/dr_4b.h \
#    ../libdvbpsi/src/descriptors/dr_4d.h \
#    ../libdvbpsi/src/descriptors/dr_4e.h \
#    ../libdvbpsi/src/descriptors/dr_4f.h \
#    ../libdvbpsi/src/descriptors/dr_50.h \
#    ../libdvbpsi/src/descriptors/dr_52.h \
#    ../libdvbpsi/src/descriptors/dr_54.h \
#    ../libdvbpsi/src/descriptors/dr_55.h \
#    ../libdvbpsi/src/descriptors/dr_56.h \
#    ../libdvbpsi/src/descriptors/dr_58.h \
#    ../libdvbpsi/src/descriptors/dr_59.h \
#    ../libdvbpsi/src/descriptors/dr_5a.h \
#    ../libdvbpsi/src/descriptors/dr_62.h \
#    ../libdvbpsi/src/descriptors/dr_66.h \
#    ../libdvbpsi/src/descriptors/dr_69.h \
#    ../libdvbpsi/src/descriptors/dr_73.h \
#    ../libdvbpsi/src/descriptors/dr_76.h \
#    ../libdvbpsi/src/descriptors/dr_7c.h \
#    ../libdvbpsi/src/descriptors/dr_83.h \
#    ../libdvbpsi/src/descriptors/dr_8a.h \
#    ../libdvbpsi/src/demux.h \
#    ../libdvbpsi/src/descriptor.h \
#    ../libdvbpsi/src/dvbpsi.h \
#    ../libdvbpsi/src/dvbpsi_private.h \
#    ../libdvbpsi/src/psi.h \
#    ../libdvbpsi/src/tables/atsc_eit.h \
#    ../libdvbpsi/src/tables/atsc_ett.h \
#    ../libdvbpsi/src/tables/atsc_mgt.h \
#    ../libdvbpsi/src/tables/atsc_stt.h \
#    ../libdvbpsi/src/tables/atsc_vct.h \
#    ../libdvbpsi/src/tables/bat.h \
#    ../libdvbpsi/src/tables/bat_private.h \
#    ../libdvbpsi/src/tables/cat.h \
#    ../libdvbpsi/src/tables/cat_private.h \
#    ../libdvbpsi/src/tables/eit.h \
#    ../libdvbpsi/src/tables/eit_private.h \
#    ../libdvbpsi/src/tables/nit.h \
#    ../libdvbpsi/src/tables/nit_private.h \
#    ../libdvbpsi/src/tables/pat.h \
#    ../libdvbpsi/src/tables/pat_private.h \
#    ../libdvbpsi/src/tables/pmt.h \
#    ../libdvbpsi/src/tables/pmt_private.h \
#    ../libdvbpsi/src/tables/rst.h \
#    ../libdvbpsi/src/tables/rst_private.h \
#    ../libdvbpsi/src/tables/sdt.h \
#    ../libdvbpsi/src/tables/sdt_private.h \
#    ../libdvbpsi/src/tables/sis.h \
#    ../libdvbpsi/src/tables/sis_private.h \
#    ../libdvbpsi/src/tables/tot.h \
#    ../libdvbpsi/src/tables/tot_private.h \
#    ../UDP-Listener/nithandler.h \
#    ../UDP-Listener/eithandler.h \
#    ../UDP-Listener/abstractdvbpsihandler.h \
#    ../UDP-Listener/pathandler.h \
#    ../UDP-Listener/pmthandler.h \
#    ../UDP-Listener/sdthandler.h \
#    ../UDP-Listener/demuxhandler.h \
#    ../UDP-Listener/contentdiscoverymodel.h \
#    event_p.h \
    mainwindow.h \
    gatewaydevice.h \
    gatewaywidget.h \
    dvbtconfig.h \
    dvbsconfig.h \
    rtspsession.h \
    sessionmodel.h \
    addsessiondialog.h \
    sessionview.h \
    scandialog.h \
    scanprocedure.h \
    rtcpreport.h \
    program.h \
    programmodel.h \
    settings.h \
    scanwidget.h \
    sessionwidget.h \
    udpforwarddialog.h \
    ssdpclient.h \
    epgcollector.h \
    objectpool.h \
    epgview.h \
    eventitem.h \
    epgwidget.h \
    programitem.h \
    epgconstants.h \
    epgparser.h \
    table_p.h \
    severselectionwidget.h \
    serverinformationwidget.h \
    channellistview.h \
    channellistwidget.h \
    programrecorder.h \
    programrecordermodel.h \
    programrecorderview.h \
    programrecorderwidget.h \
    programtester.h \
    programtesterview.h \
    programtesterwidget.h \
    programtestermodel.h \
    programrecorderrtsp.h \
    demuxer.h \
    abstractpidhandler.h \
    patpidhandler.h \
    tableparser_p.h \
    pmtpidhandler.h \
    sdtpidhandler.h \
    section_p.h \
    event.h \
    ../UDP-Listener/abstractdescriptor.h \
    ../UDP-Listener/dvbpsihelper.h \
    ../UDP-Listener/udptableview.h \
    ../UDP-Listener/udplistmodel.h \
    ../UDP-Listener/udplistener.h \
    ../UDP-Listener/rtpsocket.h \
    ../UDP-Listener/elementarystream.h \
    ../UDP-Listener/rtcpsocket.h \
    ../UDP-Listener/pmtelementarystream.h \
    ../UDP-Listener/sdtservice.h \
    zapchanneldialog.h \
    scanproceduredvbt.h \
    scanproceduredvbs.h \
    scansetupdvbtwidget.h \
    scansetupdvbswidget.h

FORMS    += mainwindow.ui \
    gatewaywidget.ui \
    dvbtconfig.ui \
    dvbsconfig.ui \
    addsessiondialog.ui \
    ../UDP-Listener/udplistener.ui \
    scandialog.ui \
    scansetuppage.ui \
    scanprogress.ui \
    scanwizard.ui \
    scanwidget.ui \
    sessionwidget.ui \
    udpforwarddialog.ui \
    epgwidget.ui \
    severselectionwidget.ui \
    serverinformationwidget.ui \
    channellistwidget.ui \
    programrecorderwidget.ui \
    programtesterwidget.ui \
    zapchanneldialog.ui \
    scansetupdvbtwidget.ui \
    scansetupdvbswidget.ui

RESOURCES += \
    ../resources.qrc

OTHER_FILES +=
