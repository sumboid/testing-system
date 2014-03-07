AUTOMAKE_OPTIONS = subdir-objects

dist_data_DATA = m4
bin_PROGRAMS = ts
ts_SOURCES = src/main.cpp \
			 src/system/cell/CellMgr.cpp \
			 src/system/cell/CellMgr.h \
			 src/system/exec/ExecMgr.cpp \
			 src/system/exec/ExecMgr.h \
			 src/system/message/MessageMgr.cpp \
			 src/system/message/MessageMgr.h \
			 src/system/message/comm/Comm.cpp \
			 src/system/message/comm/Comm.h \
			 src/system/System.cpp \
			 src/system/System.h \
			 src/types/AbstractCell.h

#if HAVE_DOXYGEN
#directory = $(top_srcdir)/docs/

#$(directory)/html: doxyfile.stamp

#doxygeneration:
#	$(DOXYGEN) Doxyfile
#	echo Timestamp > doxyfile.stamp

#CLEANFILES = doxyfile.stamp

#all-local: doxygeneration
#	clean-local:
#	rm -rf $(top_srcdir)/docs/man
#endif
