TARGET   = subsets
CXX      = g++
CPP      = $(CXX) -E
CPPFLAGS += -I../../include
CXXFLAGS += -fopenmp -fmessage-length=0
LDFLAGS  += -fopenmp
LDLIBS   +=

ifeq ($(OS),Windows_NT)
	LDLIBS += -lpsapi
endif

SRCS     = $(wildcard *.cpp)
OBJS     = $(SRCS:%.cpp=%.o)

.PONY: all debug clean depend

all: $(TARGET)
	./$(TARGET) 1000 3 100 1000

%.pdf: %.dot
	dot -Tpdf $< -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -O3 $(CPPFLAGS) -DNDEBUG $(TARGET_ARCH) -c -o $@ $<

$(TARGET): $(OBJS)
	$(CXX) $(LDFLAGS) $(TARGET_ARCH) $^ $(LDLIBS) -o $@

clean:
	$(RM) $(TARGET) $(OBJS)

define make-depend
	$(RM) depend.in
	for i in $(SRCS:%.cpp=%); do\
	    $(CPP) $(CPPFLAGS) -MM $$i.cpp | perl -n0 -e\
	        's!\s+(?:\\\s*)?\S*\.cpp!!g;\
	         s!^\S+.o:!'$$i'.o:!;\
	         print;\
	         s!^\S+.o:!'$$i'-debug.o:!;\
	         print' >> depend.in;\
	done
endef

depend:
	$(make-depend)

depend.in: $(SRCS) $(HDRS) Makefile
	$(make-depend)

include depend.in
