
CXXFLAGS = -O2 -Wall

SRCS = casm.cpp imag.cpp io.cpp kasm.cpp pasm.cpp ps.cpp rdafm.cpp rps.cpp
OBJS = $(SRCS:.cpp=.o)
MAIN = Kasm

all: $(MAIN)

$(MAIN): $(OBJS) 
	$(CXX) $(CXXFLAGS) -o $(MAIN) $(OBJS)

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	$(RM) *.o *~ $(MAIN)
