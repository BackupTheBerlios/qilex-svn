noinst_LIBRARIES = libsimulator.a

libsimulator_a_SOURCES = $(libsimulator_a_MOC) rhrteuler.cpp rhtranslink.cpp \
			rhmatrix.cpp rchain.cpp plug.cpp rchain_hand.cpp \
			rchain_rx.cpp

EXTRA_DIST = plug.cpp plug.h rchain.h rhmatrix.h rhtranslink.h rhrteuler.h rchain_hand.h \
	     rchain_rx.h
	

INCLUDES= -I@top_srcdir@/qilex/  

%.moc.cpp: %.h
	$(MOC) -o $@ $<

libsimulator_a_MOC = \
        plug.moc.cpp \
        rchain.moc.cpp rchain_hand.moc.cpp \
	rchain_rx.moc.cpp

	
CLEANFILES = $(libsimulator_a_MOC)
