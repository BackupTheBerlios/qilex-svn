bin_PROGRAMS = qilex

qilex_LDADD   = ./simulator/libsimulator.a ./soqthybrid/libsoqthybrid.a \
		./widgets/libwidgets.a ./interpret/libinterpret.a \
		./contrib/libcontrib.a

SUBDIRS = images robots simulator soqthybrid interpret \
	  widgets contrib objects grasp_objects

EXTRA_DIST = main.cpp qilex.cpp qilex.h qilexdoc.cpp qilexdoc.h qilexview.cpp \
			 qilexview.h lo32-app-qilex.png lo16-app-qilex.png functions.cpp \
			 functions.h qilex_dtd.xml cellrx90.qlx

# set the include path for X, qt and KDE
INCLUDES= $(all_includes) -I@top_srcdir@/qilex

%.moc.cpp: %.h
	$(MOC) -o $@ $<

qilex_MOC = \
	qilex.moc.cpp \
	qilexview.moc.cpp \
	qilexdoc.moc.cpp 

qilex_SOURCES = $(qilex_MOC) qilexview.cpp qilexdoc.cpp qilex.cpp functions.cpp main.cpp 

CLEANFILES = $(qilex_MOC)
	
