## your application name here

INDRIPATH=/Users/kojayboy/Workspace/indri-5.9/

include $(INDRIPATH)/MakeDefns
SHARED=
INCPATH=-I$(INDRIPATH)include $(patsubst %, -I$(INDRIPATH)contrib/%/include, $(DEPENDENCIES))
LIBPATH=-L$(INDRIPATH)obj  $(patsubst %, -L$(INDRIPATH)contrib/%/obj, $(DEPENDENCIES))
LIBS=-lindri $(patsubst %, -l%, $(DEPENDENCIES))
APP= BigramGenerator

all:
	$(CXX) $(CXXFLAGS) $(APP).cpp -o $(APP) $(LIBPATH) $(LIBS) $(CPPLDFLAGS)

install:
	$(INSTALL_PROGRAM) $(APP) $(bindir)

clean:
	rm -f $(APP)

