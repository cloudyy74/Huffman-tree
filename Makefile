CXX = g++
CXXFLAGS = -O2 -Wall -Werror -std=c++11 -Iinclude
LDFLAGS = 

TEST_EXE = huffman_test
EXE = huffman
SRCDIR = src
OBJDIR = obj
TESTDIR = test

OBJECTS = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(wildcard $(SRCDIR)/*.cpp))

all: $(EXE)

$(EXE): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(EXE) $(LDFLAGS)
	
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c -MMD -o $@ $<

include $(wildcard $(OBJDIR)/*.d)

$(OBJDIR):
	mkdir -p $(OBJDIR)

clean:
	rm -rf $(OBJDIR) $(EXE) $(TEST_EXE)

test: $(OBJDIR) $(TEST_EXE)

$(TEST_EXE): $(OBJDIR)/huffman.o $(OBJDIR)/test.o
	$(CXX) $(OBJDIR)/huffman.o $(OBJDIR)/test.o -o $(TEST_EXE) $(LDFLAGS)

$(OBJDIR)/huffman.o: $(SRCDIR)/huffman.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c -MMD -o $(OBJDIR)/huffman.o $(SRCDIR)/huffman.cpp

$(OBJDIR)/test.o: $(TESTDIR)/test.cpp | $(OBJDIR)
		$(CXX) $(CXXFLAGS) -c -MMD -o $(OBJDIR)/test.o $(TESTDIR)/test.cpp


.PHONY: clean all test

