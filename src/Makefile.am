
CXX=g++
CC=gcc
#CFLAGS=-O3 -g -Wall -I/usr/local/include -L/usr/local/lib
# Profiling
#CFLAGS=-O3 -pg -Wall -I/usr/local/include -L/usr/local/lib
# Debugging

CPPFLAGS = -Wall -Wextra 
CXXFLAGS = -O0 -g

OOQPINCLUDEDIR=/usr/local/include/ooqp

#ILOGINSTALLDIR=/data/patentdata/ILOG/CPLEX_Studio124
ILOGINSTALLDIR=/opt/ibm/ILOG/CPLEX_Studio124
CPLEXINCLUDE=$(ILOGINSTALLDIR)/cplex/include
CONCERTINCLUDE=$(ILOGINSTALLDIR)/concert/include

# Linux server
#CPLEXLIB=$(ILOGINSTALLDIR)/cplex/lib/x86-64_sles10_4.1/static_pic
#CONCERTLIB=$(ILOGINSTALLDIR)/concert/lib/x86-64_sles10_4.1/static_pic
# Local macbook
CPLEXLIB=$(ILOGINSTALLDIR)/cplex/lib/x86-64_sles10_4.1/static_pic
CONCERTLIB=$(ILOGINSTALLDIR)/concert/lib/x86-64_sles10_4.1/static_pic

INCLUDES = -I/usr/local/include -I$(CPLEXINCLUDE) -I$(CONCERTINCLUDE) -I../include

noinst_LIBRARIES = libdisambiguation.a
libdisambiguation_a_SOURCES = disambiguate.cpp cluster.cpp comparators.cpp attribute.cpp \
                              engine.cpp blocking_operation.cpp newcluster.cpp \
                              postprocess.cpp ratios.cpp ratio_smoothing.cpp \
                              training.cpp utilities.cpp threading.cpp strcmp95.c record.cpp \
                              string_manipulator.cpp record_reconfigurator.cpp

#libdisambiguation_a_CXXFLAGS = -O0 -pg a
libdisambiguation_a_CPPFLAGS = -Wall -Wextra -fno-inline $(INCLUDES) -DIL_STD -L/usr/local/lib -DNDEBUG -w #-Wno-ignored-qualifiers 
#libdisambiguation_a_LDFLAGS = -pg  -O0

#disambiguate_CPPFLAGS = -O0 -g -Wall -fno-inline $(INCLUDES) -DIL_STD -L/usr/local/lib -DNDEBUG -w
disambiguate_LDFLAGS = -L$(CPLEXLIB) -lilocplex -lcplex -L$(CONCERTLIB) -lconcert
#disambiguate_LDADD = -L$(CPLEXLIB) -lilocplex -lcplex -L$(CONCERTLIB) -lconcert
#disambiguate_LDADD   = ilocplex

bin_PROGRAMS = disambiguate  zardoz #txt2sqlite3
disambiguate_SOURCES = main.cpp 
#disambiguate_CXXFLAGS = -O0 -pg 
disambiguate_CPPFLAGS = -Wall -Wextra -fno-inline $(INCLUDES) -DIL_STD -L/usr/local/lib 
#disambiguate_CPPFLAGS = -O0 -pg  -Wall -Wextra -fno-inline $(INCLUDES) -DIL_STD -L/usr/local/lib #-DNDEBUG -w -finstrument-functions-exclude-file-list=iostream.h,string.h,vector.h
#disambiguate_LDFLAGS = -L$(CPLEXLIB) -lilocplex -lcplex -L$(CONCERTLIB) -lconcert
#disambiguate_LDFLAGS = -pg  -O0
# For Macos
#disambiguate_LDADD = libdisambiguation.a # -L$(CPLEXLIB) -lilocplex -lcplex -L$(CONCERTLIB) -lconcert
disambiguate_LDADD = libdisambiguation.a  -L$(CPLEXLIB) -lilocplex -lcplex -L$(CONCERTLIB) -lconcert
#disambiguate_LDADD   = ilocplex


zardoz_SOURCES = txt2sqlite3.cpp
