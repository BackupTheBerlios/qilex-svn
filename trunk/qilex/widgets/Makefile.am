noinst_LIBRARIES = libwidgets.a


libwidgets_a_SOURCES = $(libwidgets_a_MOC) consolewidget.cpp new_geometric_object.cpp qpanel_joint.cpp \  	panel_control.cpp new_kinematic_chain.cpp panel_control_hand.cpp new_grasping_object.cpp \         	    panel_control_rx.cpp netthread.cpp


#EXTRA_DIST = new_kinematic_chain.cpp new_kinematic_chain.h new_kinematic_chain.ui panel_control.cpp panel_control.h panel_control.ui containers.h qpanel_joint.h qpanel_joint.cpp qpanel_joint.ui new_geometric_object.h new_geometric_object.cpp consolewidget.cpp consolewidget.h

EXTRA_DIST = consolewidget.h  new_geometric_object.h  new_kinematic_chain.h  panel_control_hand.h \
 		containers.h  new_grasping_object.h  panel_control.h  qpanel_joint.h panel_control_rx netthread.h

INCLUDES = -I$(top_srcdir)/qilex

%.moc.cpp: %.h
	$(MOC) -o $@ $<

libwidgets_a_MOC = \
        consolewidget.moc.cpp \
        new_geometric_object.moc.cpp \
        new_kinematic_chain.moc.cpp \
		  panel_control.moc.cpp \
	 	  qpanel_joint.moc.cpp panel_control_hand.moc.cpp \
	     panel_control_rx.moc.cpp new_grasping_object.moc.cpp 

CLEANFILES = $(libwidgets_a_MOC) 

