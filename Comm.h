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
		bool packageSend;
		
		
		
		
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
			pattern = "&(	\d)\r\n(\d)\r\n([\d-][\d.][\d.][\d.][\d])\r\n([\d-][\d.][\d.][\d.][\d])\r\n@";
			compile();
			pc->baud(9600);
		}
		
		bool getPackageSend(){
			return packageSend;
		}
		
		void setPackageSend(){
			packageSend = !packageSend;
		}
		
		void setPackageSend(bool set){
			packageSend = set;
		}
		
		void setBuffer(char *buffer){
			buf = buffer;
		}
	
		void dataReached(){
			pc->printf("reached");
			if(slre_match(&slre, buf, strlen(buf), captures)){
				pc->printf("matched");
				matched();
			}
		}
		
		void dataReached(char *buff){
			pc->printf("reached\n");
			if(slre_match(&slre, buff, strlen(buff), captures)){
				pc->printf("matched\n");
				matched();
			}
		}
		
		void messageVerification(char *buff,int limit){
			char ax[5];
			char ay[5];
			char atotal[1];
			char aactual[1];
			
			int size = limit;//strlen(buff);
			for(int idx=0;idx<size;idx++){
				if(buff[idx]=='&' && idx+21 <= limit){
					if(buff[idx+21]=='@'){						
						atotal[0] = buff[idx+1];
						aactual[0] = buff[idx+4];
						
						for(int idy=0;idy<5;idy++){
							ax[idy] = buff[idx+idy+7];
							ay[idy] = buff[idx+idy+14];							
						}
						
						totalPoint = atoi(atotal);
						pointNum = atoi(aactual);
						x = atof(ax);
						y = atof(ay);
						
						//
						packageSend = true;
						pc->printf("%d,%d,%5.3f,%5.3f\n",totalPoint,pointNum,x,y);
						//pc->printf("T: %d, A: %d, X: %5.3f, Y: %5.3f\n",totalPoint,pointNum,x,y);
						//pc->printf("%d,%d,%5.2f,%5.2f,%d\n",totalPoint,pointNum,x,y,limit);
						
						//pc->printf("%c,%d,%d\n",buff[idx],idx,limit);
					}
					//pc->printf("yes\n");
				}
				/*else{
				pc->putc('a');
				pc->putc('\n');
				}*/
			}
		}
		
		void printTest(){
			pc->baud(9600);
			pc->printf("print test comm 1\n");
		}
};

Comm::Comm():
buf(),pattern(),slre(),captures(),totalPoint(),
pointNum(),x(),y(),
pc(new Serial(p13,p14)),
packageSend(false)
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
