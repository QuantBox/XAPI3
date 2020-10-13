ALL=libQuantBox_CTP_Quote.so libQuantBox_CTP_Trade.so
include ../../Makefile.global

DEPS=../../include/toolkit.o \
	  ../../include/ApiProcess.o

LFLAGS=-Wl,-rpath,\$$ORIGIN -ldl -L../../include/CTP/linux64 -L../../common/Queue -lQueue

libQuantBox_CTP_Quote.so: Quote/main.cpp Quote/MdUserApi.cpp TypeConvert.cpp $(DEPS)
	$(CXX) --shared $(IFLAGS) $(LFLAGS) $(CFLAGS) -lthostmduserapi -I../CTP_Quote -o $@ $^
libQuantBox_CTP_Trade.so: Trade/main.cpp Trade/TraderApi.cpp TypeConvert.cpp $(DEPS)
	$(CXX) --shared $(IFLAGS) $(LFLAGS) $(CFLAGS) -lthosttraderapi -I../CTP_Trade -o $@ $^

