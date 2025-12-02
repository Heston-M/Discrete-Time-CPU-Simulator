
OBJDIR := objectFiles

simulator: $(OBJDIR) objectFiles/simulator.o objectFiles/RandomGenerator.o objectFiles/TimeGenerator.o
	g++ -o simulator objectFiles/simulator.o objectFiles/RandomGenerator.o objectFiles/TimeGenerator.o

$(OBJDIR):
ifeq ($(OS),Windows_NT)
	-@if not exist "$(OBJDIR)" mkdir "$(OBJDIR)"
else
	-@mkdir -p $(OBJDIR)
endif

objectFiles/simulator.o: | $(OBJDIR) simulator.cpp generators/TimeGenerator.h generators/RandomGenerator.h
	g++ -c simulator.cpp -o objectFiles/simulator.o

objectFiles/RandomGenerator.o: | $(OBJDIR) generators/RandomGenerator.cpp generators/RandomGenerator.h
	g++ -c generators/RandomGenerator.cpp -o objectFiles/RandomGenerator.o

objectFiles/TimeGenerator.o: | $(OBJDIR) generators/TimeGenerator.cpp generators/TimeGenerator.h
	g++ -c generators/TimeGenerator.cpp -o objectFiles/TimeGenerator.o

clean:
	rm -f simulator objectFiles/*.o

