noinst_LIBRARIES = libsoqthybrid.a

INCLUDES= $(all_includes) -I@top_srcdir@/qilex/

EXTRA_DIST = soqtcomposerotation.cpp soqtcomposerotation.h \
	   soqtcomposetranslation.h soqtcomposetranslation.cpp

%.moc.cpp: %.h
	$(MOC) -o $@ $<

libsoqthybrid_a__MOC = \
        soqtcomposerotation.moc.cpp \
        soqtcomposetranslation.moc.cpp

libsoqthybrid_a_SOURCES = $(libsoqthybrid_a__MOC) soqtcomposerotation.cpp \
			  soqtcomposetranslation.cpp

CLEANFILES = soqtcomposerotation.moc.cpp soqtcomposetranslation.moc.cpp
