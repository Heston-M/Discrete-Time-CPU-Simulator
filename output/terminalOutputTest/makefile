OBJDIR := objectFiles
TARGET := tOTest

$(TARGET): $(OBJDIR) $(OBJDIR)/tOTest.o $(OBJDIR)/terminalOutput.o
	g++ -o $(TARGET) $(OBJDIR)/tOTest.o $(OBJDIR)/terminalOutput.o

$(OBJDIR):
ifeq ($(OS),Windows_NT)
	-@if not exist "$(OBJDIR)" mkdir "$(OBJDIR)"
else
	-@mkdir -p $(OBJDIR)
endif

$(OBJDIR)/tOTest.o: | $(OBJDIR) tOTest.cpp ../terminalOutput.h
	g++ -c tOTest.cpp -o $(OBJDIR)/tOTest.o

$(OBJDIR)/terminalOutput.o: | $(OBJDIR) ../terminalOutput.cpp ../terminalOutput.h ../../config.h
	g++ -c ../terminalOutput.cpp -o $(OBJDIR)/terminalOutput.o

clean:
ifeq ($(OS),Windows_NT)
	-@if exist $(TARGET).exe del /q $(TARGET).exe
	-@if exist $(TARGET) del /q $(TARGET)
	-@if exist $(OBJDIR)\*.o del /q $(OBJDIR)\*.o
else
	rm -f $(TARGET) $(OBJDIR)/*.o
endif


