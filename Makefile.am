AUTOMAKE_OPTIONS = subdir-objects

#dist_data_DATA = m4
lib_LTLIBRARIES = libts.la
libts_la_SOURCES =  src/system/cell/CellMgr.cpp \
										src/system/cell/CellMgr.h \
										src/system/exec/ExecMgr.cpp \
										src/system/exec/ExecMgr.h \
										src/system/message/MessageMgr.cpp \
										src/system/message/MessageMgr.h \
										src/system/message/comm/Comm.cpp \
										src/system/message/comm/Comm.h \
										src/system/System.cpp \
										src/system/System.h \
										src/system/util/Listener.cpp \
										src/system/util/Listener.h \
										src/system/util/Semaphore.cpp \
										src/system/util/Semaphore.h \
										src/distr/Distributor.cpp \
										src/distr/Distributor.h \
										src/types/Cell.cpp \
										src/types/Cell.h \
										src/types/CellTools.h \
										src/types/CellTools.cpp \
										src/types/ID.cpp \
										src/types/ID.h \
										src/types/ReduceData.h \
										src/types/ReduceDataTools.h \
										src/types/util/CellSerializer.h \
										src/types/util/CellSerializer.cpp \
										src/types/util/CellDeserializer.h \
										src/types/util/CellDeserializer.cpp

tssystemdir = $(includedir)/ts/system
tssystem_HEADERS =  src/system/System.h

tssystemexecdir = $(includedir)/ts/system/exec
tssystemexec_HEADERS = src/system/exec/ExecMgr.h

tssystemcelldir = $(includedir)/ts/system/cell
tssystemcell_HEADERS = src/system/cell/CellMgr.h

tssystemmsgdir = $(includedir)/ts/system/message
tssystemmsg_HEADERS = src/system/message/MessageMgr.h

tssystemmsgcommdir = $(includedir)/ts/system/message/comm
tssystemmsgcomm_HEADERS = src/system/message/comm/Comm.h

tstypesdir = $(includedir)/ts/types
tstypes_HEADERS = src/types/Cell.h \
									src/types/CellTools.h \
									src/types/ReduceData.h \
									src/types/ReduceDataTools.h \
									src/types/ID.h

tssystemutildir = $(includedir)/ts/system/util
tssystemutil_HEADERS = src/system/util/Listener.h \
											 src/system/util/Semaphore.h

tstypesutildir = $(includedir)/ts/types/util
tstypesutildir_HEADER = src/types/util/CellSerializer.h \
												src/types/util/CellDeserializer.h
