OBJDIR := objectFiles
TARGET := tOTest

$(TARGET): $(OBJDIR) $(OBJDIR)/tOTest.o $(OBJDIR)/terminalOutput.o $(OBJDIR)/Process.o $(OBJDIR)/ReadyQueueList.o
	g++ -o $(TARGET) $(OBJDIR)/tOTest.o $(OBJDIR)/terminalOutput.o $(OBJDIR)/Process.o $(OBJDIR)/ReadyQueueList.o

$(OBJDIR):
ifeq ($(OS),Windows_NT)
	-@if not exist "$(OBJDIR)" mkdir "$(OBJDIR)"
else
	-@mkdir -p $(OBJDIR)
endif

$(OBJDIR)/tOTest.o: | $(OBJDIR) tOTest.cpp ../../output/terminalOutput.h ../../output/output.h ../../processes/Process.h ../../processes/ReadyQueueList.h
	g++ -c tOTest.cpp -o $(OBJDIR)/tOTest.o

$(OBJDIR)/terminalOutput.o: | $(OBJDIR) ../../output/terminalOutput.cpp ../../output/terminalOutput.h
	g++ -c ../../output/terminalOutput.cpp -o $(OBJDIR)/terminalOutput.o

$(OBJDIR)/Process.o: | $(OBJDIR) ../../processes/Process.cpp ../../processes/Process.h
	g++ -c ../../processes/Process.cpp -o $(OBJDIR)/Process.o

$(OBJDIR)/ReadyQueueList.o: | $(OBJDIR) ../../processes/ReadyQueueList.cpp ../../processes/ReadyQueueList.h
	g++ -c ../../processes/ReadyQueueList.cpp -o $(OBJDIR)/ReadyQueueList.o

clean:
ifeq ($(OS),Windows_NT)
	-@if exist $(TARGET).exe del /q $(TARGET).exe
	-@if exist $(TARGET) del /q $(TARGET)
	-@if exist $(OBJDIR)\*.o del /q $(OBJDIR)\*.o
else
	rm -f $(TARGET) $(OBJDIR)/*.o
endif


