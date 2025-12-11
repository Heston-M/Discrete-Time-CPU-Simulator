
OBJDIR := objectFiles

simulator: $(OBJDIR) objectFiles/simulator.o objectFiles/RandomGenerator.o objectFiles/TimeGenerator.o objectFiles/Process.o objectFiles/ReadyQueueList.o objectFiles/CPUList.o objectFiles/StatisticsUnit.o objectFiles/InputHandler.o objectFiles/EndChecker.o objectFiles/terminalOutput.o
	g++ -o simulator objectFiles/simulator.o objectFiles/RandomGenerator.o objectFiles/TimeGenerator.o objectFiles/Process.o objectFiles/ReadyQueueList.o objectFiles/CPUList.o objectFiles/StatisticsUnit.o objectFiles/InputHandler.o objectFiles/EndChecker.o objectFiles/terminalOutput.o

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

objectFiles/StatisticsUnit.o: | $(OBJDIR) statistics/StatisticsUnit.cpp statistics/StatisticsUnit.h
	g++ -c statistics/StatisticsUnit.cpp -o objectFiles/StatisticsUnit.o

objectFiles/InputHandler.o: | $(OBJDIR) input/InputHandler.cpp input/InputHandler.h
	g++ -c input/InputHandler.cpp -o objectFiles/InputHandler.o

objectFiles/EndChecker.o: | $(OBJDIR) endChecker/endChecker.cpp endChecker/endChecker.h
	g++ -c endChecker/endChecker.cpp -o objectFiles/EndChecker.o

objectFiles/terminalOutput.o: | $(OBJDIR) output/terminalOutput.cpp output/terminalOutput.h
	g++ -c output/terminalOutput.cpp -o objectFiles/terminalOutput.o

clean:
ifeq ($(OS),Windows_NT)
	-@if exist simulator.exe del /q simulator.exe
	-@if exist simulator del /q simulator
	-@if exist objectFiles\*.o del /q objectFiles\*.o
else
	rm -f simulator objectFiles/*.o
endif

