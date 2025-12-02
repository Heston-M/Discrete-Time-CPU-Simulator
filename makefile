
OBJDIR := objectFiles

simulator: $(OBJDIR) objectFiles/simulator.o objectFiles/RandomGenerator.o objectFiles/TimeGenerator.o objectFiles/Process.o objectFiles/ReadyQueueList.o objectFiles/CPUList.o
	g++ -o simulator objectFiles/simulator.o objectFiles/RandomGenerator.o objectFiles/TimeGenerator.o objectFiles/Process.o objectFiles/ReadyQueueList.o objectFiles/CPUList.o

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

objectFiles/Process.o: | $(OBJDIR) processes/Process.cpp processes/Process.h
	g++ -c processes/Process.cpp -o objectFiles/Process.o

objectFiles/ReadyQueueList.o: | $(OBJDIR) processes/ReadyQueueList.cpp processes/ReadyQueueList.h
	g++ -c processes/ReadyQueueList.cpp -o objectFiles/ReadyQueueList.o

objectFiles/CPUList.o: | $(OBJDIR) processes/CPUList.cpp processes/CPUList.h
	g++ -c processes/CPUList.cpp -o objectFiles/CPUList.o

clean:
	rm -f simulator objectFiles/*.o

