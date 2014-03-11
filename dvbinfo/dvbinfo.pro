#-------------------------------------------------
#
# Project created by QtCreator 2013-05-07T10:27:12
#
#-------------------------------------------------

QT       -= core gui
CONFIG   += console
CONFIG   -= app_bundle
TEMPLATE = app
TARGET = dvbinfo
DEFINES += _GNU_SOURCE

QMAKE_CC= gcc -std=c99

INCLUDEPATH +=  \
    ../libdvbpsi \
    ../libdvbpsi/src \
    ../libdvbpsi/src/descriptors \
    ../libdvbpsi/src/tables \

SOURCES += \
    buffer.c \
    dvbinfo.c \
    libdvbpsi.c \
    tcp.c \
    udp.c \
    ../libdvbpsi/src/descriptors/dr_02.c \
    ../libdvbpsi/src/descriptors/dr_03.c \
    ../libdvbpsi/src/descriptors/dr_04.c \
    ../libdvbpsi/src/descriptors/dr_05.c \
    ../libdvbpsi/src/descriptors/dr_06.c \
    ../libdvbpsi/src/descriptors/dr_07.c \
    ../libdvbpsi/src/descriptors/dr_08.c \
    ../libdvbpsi/src/descriptors/dr_09.c \
    ../libdvbpsi/src/descriptors/dr_0a.c \
    ../libdvbpsi/src/descriptors/dr_0b.c \
    ../libdvbpsi/src/descriptors/dr_0c.c \
    ../libdvbpsi/src/descriptors/dr_0d.c \
    ../libdvbpsi/src/descriptors/dr_0e.c \
    ../libdvbpsi/src/descriptors/dr_0f.c \
    ../libdvbpsi/src/descriptors/dr_13.c \
    ../libdvbpsi/src/descriptors/dr_14.c \
    ../libdvbpsi/src/descriptors/dr_40.c \
    ../libdvbpsi/src/descriptors/dr_41.c \
    ../libdvbpsi/src/descriptors/dr_42.c \
    ../libdvbpsi/src/descriptors/dr_43.c \
    ../libdvbpsi/src/descriptors/dr_44.c \
    ../libdvbpsi/src/descriptors/dr_45.c \
    ../libdvbpsi/src/descriptors/dr_47.c \
    ../libdvbpsi/src/descriptors/dr_48.c \
    ../libdvbpsi/src/descriptors/dr_49.c \
    ../libdvbpsi/src/descriptors/dr_4a.c \
    ../libdvbpsi/src/descriptors/dr_4b.c \
    ../libdvbpsi/src/descriptors/dr_4d.c \
    ../libdvbpsi/src/descriptors/dr_4e.c \
    ../libdvbpsi/src/descriptors/dr_4f.c \
    ../libdvbpsi/src/descriptors/dr_50.c \
    ../libdvbpsi/src/descriptors/dr_52.c \
    ../libdvbpsi/src/descriptors/dr_54.c \
    ../libdvbpsi/src/descriptors/dr_55.c \
    ../libdvbpsi/src/descriptors/dr_56.c \
    ../libdvbpsi/src/descriptors/dr_58.c \
    ../libdvbpsi/src/descriptors/dr_59.c \
    ../libdvbpsi/src/descriptors/dr_5a.c \
    ../libdvbpsi/src/descriptors/dr_62.c \
    ../libdvbpsi/src/descriptors/dr_66.c \
    ../libdvbpsi/src/descriptors/dr_69.c \
    ../libdvbpsi/src/descriptors/dr_73.c \
    ../libdvbpsi/src/descriptors/dr_76.c \
    ../libdvbpsi/src/descriptors/dr_7c.c \
    ../libdvbpsi/src/descriptors/dr_83.c \
    ../libdvbpsi/src/descriptors/dr_8a.c \
    ../libdvbpsi/src/demux.c \
    ../libdvbpsi/src/descriptor.c \
    ../libdvbpsi/src/dvbpsi.c \
    ../libdvbpsi/src/psi.c \
    ../libdvbpsi/src/tables/atsc_eit.c \
    ../libdvbpsi/src/tables/atsc_ett.c \
    ../libdvbpsi/src/tables/atsc_mgt.c \
    ../libdvbpsi/src/tables/atsc_stt.c \
    ../libdvbpsi/src/tables/atsc_vct.c \
    ../libdvbpsi/src/tables/bat.c \
    ../libdvbpsi/src/tables/cat.c \
    ../libdvbpsi/src/tables/eit.c \
    ../libdvbpsi/src/tables/nit.c \
    ../libdvbpsi/src/tables/pat.c \
    ../libdvbpsi/src/tables/pmt.c \
    ../libdvbpsi/src/tables/rst.c \
    ../libdvbpsi/src/tables/sdt.c \
    ../libdvbpsi/src/tables/sis.c \
    ../libdvbpsi/src/tables/tot.c

HEADERS  += \
    buffer.h \
    dvbinfo.h \
    libdvbpsi.h \
    tcp.h \
    udp.h \
    ../libdvbpsi/config.h \
    ../libdvbpsi/src/descriptors/dr.h \
    ../libdvbpsi/src/descriptors/dr_02.h \
    ../libdvbpsi/src/descriptors/dr_03.h \
    ../libdvbpsi/src/descriptors/dr_04.h \
    ../libdvbpsi/src/descriptors/dr_05.h \
    ../libdvbpsi/src/descriptors/dr_06.h \
    ../libdvbpsi/src/descriptors/dr_07.h \
    ../libdvbpsi/src/descriptors/dr_08.h \
    ../libdvbpsi/src/descriptors/dr_09.h \
    ../libdvbpsi/src/descriptors/dr_0a.h \
    ../libdvbpsi/src/descriptors/dr_0b.h \
    ../libdvbpsi/src/descriptors/dr_0c.h \
    ../libdvbpsi/src/descriptors/dr_0d.h \
    ../libdvbpsi/src/descriptors/dr_0e.h \
    ../libdvbpsi/src/descriptors/dr_0f.h \
    ../libdvbpsi/src/descriptors/dr_13.h \
    ../libdvbpsi/src/descriptors/dr_14.h \
    ../libdvbpsi/src/descriptors/dr_40.h \
    ../libdvbpsi/src/descriptors/dr_41.h \
    ../libdvbpsi/src/descriptors/dr_42.h \
    ../libdvbpsi/src/descriptors/dr_43.h \
    ../libdvbpsi/src/descriptors/dr_44.h \
    ../libdvbpsi/src/descriptors/dr_45.h \
    ../libdvbpsi/src/descriptors/dr_47.h \
    ../libdvbpsi/src/descriptors/dr_48.h \
    ../libdvbpsi/src/descriptors/dr_49.h \
    ../libdvbpsi/src/descriptors/dr_4a.h \
    ../libdvbpsi/src/descriptors/dr_4b.h \
    ../libdvbpsi/src/descriptors/dr_4d.h \
    ../libdvbpsi/src/descriptors/dr_4e.h \
    ../libdvbpsi/src/descriptors/dr_4f.h \
    ../libdvbpsi/src/descriptors/dr_50.h \
    ../libdvbpsi/src/descriptors/dr_52.h \
    ../libdvbpsi/src/descriptors/dr_54.h \
    ../libdvbpsi/src/descriptors/dr_55.h \
    ../libdvbpsi/src/descriptors/dr_56.h \
    ../libdvbpsi/src/descriptors/dr_58.h \
    ../libdvbpsi/src/descriptors/dr_59.h \
    ../libdvbpsi/src/descriptors/dr_5a.h \
    ../libdvbpsi/src/descriptors/dr_62.h \
    ../libdvbpsi/src/descriptors/dr_66.h \
    ../libdvbpsi/src/descriptors/dr_69.h \
    ../libdvbpsi/src/descriptors/dr_73.h \
    ../libdvbpsi/src/descriptors/dr_76.h \
    ../libdvbpsi/src/descriptors/dr_7c.h \
    ../libdvbpsi/src/descriptors/dr_83.h \
    ../libdvbpsi/src/descriptors/dr_8a.h \
    ../libdvbpsi/src/demux.h \
    ../libdvbpsi/src/descriptor.h \
    ../libdvbpsi/src/dvbpsi.h \
    ../libdvbpsi/src/dvbpsi_private.h \
    ../libdvbpsi/src/psi.h \
    ../libdvbpsi/src/tables/atsc_eit.h \
    ../libdvbpsi/src/tables/atsc_ett.h \
    ../libdvbpsi/src/tables/atsc_mgt.h \
    ../libdvbpsi/src/tables/atsc_stt.h \
    ../libdvbpsi/src/tables/atsc_vct.h \
    ../libdvbpsi/src/tables/bat.h \
    ../libdvbpsi/src/tables/bat_private.h \
    ../libdvbpsi/src/tables/cat.h \
    ../libdvbpsi/src/tables/cat_private.h \
    ../libdvbpsi/src/tables/eit.h \
    ../libdvbpsi/src/tables/eit_private.h \
    ../libdvbpsi/src/tables/nit.h \
    ../libdvbpsi/src/tables/nit_private.h \
    ../libdvbpsi/src/tables/pat.h \
    ../libdvbpsi/src/tables/pat_private.h \
    ../libdvbpsi/src/tables/pmt.h \
    ../libdvbpsi/src/tables/pmt_private.h \
    ../libdvbpsi/src/tables/rst.h \
    ../libdvbpsi/src/tables/rst_private.h \
    ../libdvbpsi/src/tables/sdt.h \
    ../libdvbpsi/src/tables/sdt_private.h \
    ../libdvbpsi/src/tables/sis.h \
    ../libdvbpsi/src/tables/sis_private.h \
    ../libdvbpsi/src/tables/tot.h \
    ../libdvbpsi/src/tables/tot_private.h
