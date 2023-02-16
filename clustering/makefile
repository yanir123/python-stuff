PACK="C:\\Program Files\\7-Zip\\7z.exe"
TARGET=package.7z
SRC=package/
FLAGS=a -t7z

$(TARGET):
	$(PACK) $(FLAGS) $(TARGET) $(SRC)

clean:
	$(RM) $(TARGET)