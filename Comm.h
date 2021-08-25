#include "mbed.h"
#include "slre.h"

class Comm{
	private:
		char *buf;
		char *pattern;
		struct slre	slre;
		struct cap	captures[4 + 1];
		
		int totalPoint;
		int pointNum;
		float x;
		float y;
		Serial *pc;
		
		
		
		
		void compile(){
			slre_compile(&slre, pattern);
		}
		
		void matched(){
			totalPoint = atoi(captures[0].ptr);
			pointNum = atoi(captures[1].ptr);
			x = atof(captures[2].ptr);
			y = atof(captures[3].ptr);
			pc->printf("Total: %d, Actual: %d, x: %5.2f, y: %5.2f",totalPoint,pointNum,x,y);
		}
	
	public:
		Comm();
		
		void init(){
			buf = "GET /foo.bar?query=moo HTTP/1.5\r\n";
			pattern = "&(\d)\r\n(\d)\r\n([\d-][\d.][\d.][\d.][\d])\r\n([\d-][\d.][\d.][\d.][\d])\r\n@";
			compile();
			pc->baud(9600);
		}
		
		void setBuffer(char *buffer){
			buf = buffer;
		}
	
		void dataReached(){
			if(slre_match(&slre, buf, strlen(buf), captures)){
				matched();
			}
		}
		
		void dataReached(char *buff){
			if(slre_match(&slre, buff, strlen(buf), captures)){
				matched();
			}
		}
};

Comm::Comm():
buf(),pattern(),slre(),captures(),totalPoint(),
pointNum(),x(),y(),
pc(new Serial(p13,p14))
{}
/*
char *buf;
	char *pattern;
	struct slre	slre;
	struct cap	captures[4 + 1];
	
	int totalPoint;
	int pointNum;
	float x;
	float y;
	Serial *pc;*/
