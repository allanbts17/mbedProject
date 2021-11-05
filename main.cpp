#include "mbed.h"
#include "motorControl.h"
#include "slre.h"
#include "Comm.h"
//#include "objectGroup.h"

DigitalOut led1(LED1);
DigitalOut led2(LED2);
Serial pc(p13,p14); //tx y rx -- naranja,rojo
Serial xbee(p9,p10);//tx y rx -- naranja,amarillo
motorControl controller;
Comm comm;
Timer t;

InterruptIn a(p26);
InterruptIn b(p27);

const int BUF_LIMIT = 96; //24 * 4
int bufIndex = 0;
char *buf = new char[BUF_LIMIT];

int i = 0;
int msg;
bool once = false;
bool timerFinished = false;

char step[]="000000"; //0.0000 - 360.00

void moveMsg(){

	while(pc.readable()){
	if(!once){
		t.start();
		once = true;
		timerFinished = false;
	}
		step[i] = pc.getc();
		i++;
		pc.printf("%d\n",t.read_ms());
		
	}
	/*if(!pc.readable() and i>1){
		
			pc.printf("%s\n",step);
		}
		once = true;
	}*/
	/*while(pc.readable()){
		step[i] = pc.getc();
		i++;
	}*/
	//if(i>1)
	//	if(!pc.readable() and step[i-1] == 'a') {
			/*msg = atoi(step);
			i=0;
			controller.pulse_setpoint = msg;
			controller.last_time = controller.t.read_us();
			controller.setMsg();
			controller.run();*/
	//	}
	//while(pc.readable()){
	
		//pc.putc(pc.getc());

		//step[i] = pc.getc();
		//i++;
		
		
		/*controller.pulse_setpoint = angle;
		controller.last_time = controller.t.read_us();
		controller.setMsg();
		controller.run();*/
		
//	}
	
	//pc.printf("something");
}

void xbeeComm(){
	while(xbee.readable()){
		//pc.putc(xbee.getc());
		
		if(bufIndex < BUF_LIMIT){
			if(comm.getPackageSend()){
				comm.setPackageSend();
				bufIndex = 0;
			}
			buf[bufIndex] = xbee.getc();
			comm.messageVerification(buf,bufIndex);
			bufIndex++;
		} else {
			bufIndex = 0;
		}
	}
}

int main() 
{
	pc.attach(moveMsg);
	pc.baud(9600);
	xbee.baud(9600);
	
	xbee.attach(xbeeComm);
	//char *txt = "2.4560";
	//float num = atof(txt);
	//pc.printf("%f", num);
	controller.init();
	char g[] = "500.05";
	float f = atof(g);
  pc.printf("Empezo la prueba: \n");
	pc.printf("%f\n\n",f);
	comm.init();
	
	while(!timerFinished){
		if(t.read_ms()>10){
				pc.printf("%s\n",step);
				t.stop();
				t.reset();
				timerFinished = true;
				once = false;
				for(int i=0;i<6;i++)
					step[i] = '0';
		}
	}
	
	
}
