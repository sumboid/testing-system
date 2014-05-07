AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = -I m4
#dist_data_DATA = m4
lib_LTLIBRARIES = libts.la
libts_la_SOURCES =  src/system/fragment/FragmentMgr.cpp \
										src/system/fragment/FragmentMgr.h \
										src/system/exec/ExecMgr.cpp \
										src/system/exec/ExecMgr.h \
										src/system/message/MessageMgr.cpp \
										src/system/message/MessageMgr.h \
										src/system/message/NodeID.h \
										src/system/message/comm/Comm.cpp \
										src/system/message/comm/Comm.h \
										src/system/System.cpp \
										src/system/System.h \
										src/system/util/Listener.cpp \
										src/system/util/Listener.h \
										src/system/util/Semaphore.cpp \
										src/system/util/Semaphore.h \
										src/system/action/Action.h \
										src/system/action/Action.cpp \
										src/system/action/ActionBuilder.h \
										src/system/action/ActionBuilder.cpp \
										src/system/action/actions/Update.h \
										src/system/action/actions/Update.cpp \
										src/system/action/actions/Reduce.h \
										src/system/action/actions/Reduce.cpp \
										src/system/action/actions/StartMove.h \
										src/system/action/actions/StartMove.cpp \
										src/system/action/actions/ConfirmMove.h \
										src/system/action/actions/ConfirmMove.cpp \
										src/system/action/actions/Move.h \
										src/system/action/actions/Move.cpp \
										src/distr/Distributor.cpp \
										src/distr/Distributor.h \
										src/types/Fragment.cpp \
										src/types/Fragment.h \
										src/types/FragmentTools.h \
										src/types/FragmentTools.cpp \
										src/types/ID.cpp \
										src/types/ID.h \
										src/types/ReduceData.h \
										src/types/ReduceDataTools.h \
										src/types/util/FragmentSerializer.h \
										src/types/util/FragmentSerializer.cpp \
										src/types/util/FragmentDeserializer.h \
										src/types/util/FragmentDeserializer.cpp \
										src/util/RWLock.h \
										src/util/RWLock.cpp \
										src/util/Uberlogger.h \
										src/util/Arc.h

tssystemdir = $(includedir)/ts/system
tssystem_HEADERS =  src/system/System.h

tssystemexecdir = $(includedir)/ts/system/exec
tssystemexec_HEADERS = src/system/exec/ExecMgr.h

tssystemfragmentdir = $(includedir)/ts/system/fragment
tssystemfragment_HEADERS = src/system/fragment/FragmentMgr.h

tssystemmsgdir = $(includedir)/ts/system/message
tssystemmsg_HEADERS = src/system/message/MessageMgr.h \
											src/system/message/NodeID.h

tssystemmsgcommdir = $(includedir)/ts/system/message/comm
tssystemmsgcomm_HEADERS = src/system/message/comm/Comm.h

tssystemactiondir = $(includedir)/ts/system/action
tssystemaction_HEADERS =  src/system/action/Action.h \
													src/system/action/ActionBuilder.h

tssystemactionactionsdir = $(includedir)/ts/system/action/actions
tssystemactionactions_HEADERS =  src/system/action/actions/Reduce.h \
																 src/system/action/actions/Update.h \
																 src/system/action/actions/StartMove.h \
																 src/system/action/actions/Move.h \
																 src/system/action/actions/ConfirmMove.h

tstypesdir = $(includedir)/ts/types
tstypes_HEADERS = src/types/Fragment.h \
									src/types/FragmentTools.h \
									src/types/ReduceData.h \
									src/types/ReduceDataTools.h \
									src/types/ID.h

tssystemutildir = $(includedir)/ts/system/util
tssystemutil_HEADERS = src/system/util/Listener.h \
											 src/system/util/Semaphore.h

tstypesutildir = $(includedir)/ts/types/util
tstypesutil_HEADERS = src/types/util/FragmentSerializer.h \
											src/types/util/FragmentDeserializer.h

tsutildir = $(includedir)/ts/util
tsutil_HEADERS = src/util/RWLock.h \
								 src/util/Uberlogger.h \
								 src/util/Arc.h
