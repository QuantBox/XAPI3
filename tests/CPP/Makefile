DEPS=../../languages/CPP/XAPI_CPP/XApiImpl.o \
	 ../../languages/CPP/XAPI_CPP/XApiCpp.o \
	 ../../languages/CPP/XAPI_CPP/XApiC.o

DEPS+=../../include/toolkit.o \
	  ../../include/ApiProcess.o
ALL = CPP_TEST

include ../../Makefile.global

LFLAGS=-Wl,-rpath,\$$ORIGIN \
-ldl \
-L../../apis/CTP -lQuantBox_CTP_Quote -lQuantBox_CTP_Trade \
-L../../common/Queue  -lQueue

CPP_TEST: CPP_TEST.o $(DEPS)
	$(CXX)  $(IFLAGS) $(CFLAGS) $(LFLAGS) -o $@ $^

empty_test: empty_test.cpp $(DEPS)
	$(CXX)  $(IFLAGS) $(CFLAGS) $(LFLAGS) -o $@ $^

