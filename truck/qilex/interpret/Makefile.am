noinst_LIBRARIES = libinterpret.a

libinterpret_a_SOURCES = list_ppoint.cpp list_point.cpp point.cpp \
			valplus.cpp valplus_lex.cpp $(libinterpret_a_MOC) \
			ppoint.cpp qvalparser.cpp 

#EXTRA_DIST = ppoint.cpp ppoint.h valplus.yy valplus.h valplus_lex.lxx qvalparser.cpp valplus_lex.cpp valplus.cpp qvalparser.h point.cpp point.h list_point.cpp list_point.h list_ppoint.cpp list_ppoint.h
EXTRA_DIST = list_point.h  list_ppoint.h  point.h  ppoint.h  qvalparser.h  valplus.h valplus.yy valplus_lex.lxx

%.moc.cpp: %.h
	$(MOC) -o $@ $<

libinterpret_a_MOC = \
        qvalparser.moc.cpp 

INCLUDES= -I@top_srcdir@/qilex/


AM_LFLAGS=-8 -+ --noansi-definitions
AM_YFLAGS=-d

valplus.h valplus.cpp: valplus.yy
	$(BISONXX) -t -d  -v \
	-o valplus.cpp \
	-h valplus.h  \
$<

valplus_lex.cpp: valplus_lex.lxx valplus.h
	$(LEX) -d -8 -+ -ovalplus_lex.cpp \
$<

CLEANFILES = $(libinterpret_a_MOC) valplus_lex.cpp valplus.h valplus.cpp
