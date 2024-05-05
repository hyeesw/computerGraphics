CC = g++
SOURCES = main.cpp # 소스 파일 리스트
OBJECTS = $(SOURCES:.cpp=.o) # object 파일 리스트
TARGET = main # 실행 파일 이름

all: $(TARGET) # make 명령 실행했을 때, $(TARGET) 코드 실행

$(TARGET): $(OBJECTS) # Target을 만들기 위해 필요한 object 파일들을 컴파일
	$(CC) -o $(TARGET) $(OBJECTS)

%.o: %.cpp
	$(CC) -c $<

clean: # .o 파일과 실행파일을 모두 제거
	rm -f $(OBJECTS) $(TARGET)
