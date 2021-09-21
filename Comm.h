#include "mbed.h"
#include "slre.h"

class Comm{
	private:
		char *buf;
		char *pattern;
		struct slre	slre;
		struct cap	captures[4 + 1];
		
		const int objectLimit;
		int objectIndex;
		bool once;
		int totalPoint[120];
		int pointNum[120];
		float x[120];
		float y[120];
		Serial *pc;
		bool packageSend;
		
		
		/*struct Object  
		{  
			char ax[5];
			char ay[5];
			char atotal[1];
			char aactual[1];
		}; 
		
		Object obj[20];*/
		
		
		void compile(){
			slre_compile(&slre, pattern);
		}
		
		void matched(){
			/*totalPoint = atoi(captures[0].ptr);
			pointNum = atoi(captures[1].ptr);
			x = atof(captures[2].ptr);
			y = atof(captures[3].ptr);
			pc->printf("Total: %d, Actual: %d, x: %5.2f, y: %5.2f",totalPoint,pointNum,x,y);*/
		}
	
	public:
		Comm();
		
		void init(){
			objectIndex = 0;
			once = true;
			
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
						//obj[0] = {ax,ay,atotal,aactual};
						
						for(int idy=0;idy<5;idy++){
							ax[idy] = buff[idx+idy+7];
							ay[idy] = buff[idx+idy+14];							
						}
						
						totalPoint[objectIndex] = atoi(atotal);
						pointNum[objectIndex] = atoi(aactual);
						x[objectIndex] = atof(ax);
						y[objectIndex] = atof(ay);
						objectIndex++;
						
						if(objectIndex > objectLimit){
							if(once){
								once = false;
								for(int idz=0;idz<objectLimit;idz++)
									pc->printf("%d,%d,%5.3f,%5.3f\n",totalPoint[idz],pointNum[idz],x[idz],y[idz]);
							}
						}
						
						packageSend = true;
						//pc->printf("%d,%d,%5.3f,%5.3f\n",totalPoint[0],pointNum[0],x[0],y[0]);
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
buf(),pattern(),slre(),captures(),objectLimit(110),totalPoint(),
pointNum(),x(),y(),
pc(new Serial(p13,p14)),
packageSend(false)
{}
//objectLimit = 120
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
