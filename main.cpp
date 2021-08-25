#include "mbed.h"
#include "motorControl.h"
#include "slre.h"
#include "Comm.h"

DigitalOut led1(LED1);
DigitalOut led2(LED2);
Serial pc(p13,p14); //tx y rx -- naranja,rojo
Serial xbee(p9,p10);
motorControl controller;
Comm comm;

InterruptIn a(p26);
InterruptIn b(p27);

const int BUF_LIMIT = 96; //24 * 4
int bufIndex = 0;
char *buf = new char[BUF_LIMIT];

int i = 0;
int msg;

char step[20];

void moveMsg(){
	while(pc.readable()){
		step[i] = pc.getc();
		i++;
	}
	if(i>1)
		if(!pc.readable() and step[i-1] == 'a') {
			msg = atoi(step);
			i=0;
			controller.pulse_setpoint = msg;
			controller.last_time = controller.t.read_us();
			controller.setMsg();
			controller.run();
		}
}

void xbeeComm(){
	while(xbee.readable()){
		//pc.putc(xbee.getc());
		
		if(bufIndex < BUF_LIMIT){
			buf[bufIndex] = xbee.getc();
			comm.dataReached(buf);
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
//	char *txt = "2.4560";
	//float num = atof(txt);
	//pc.printf("%f", num);
	/*controller.init();*/
	
  pc.printf("Empezo la prueba: \n");
	comm.init();
	
	char *leng = new char[5];
	char dato = '2';
	//pc.printf("tesst");
	pc.putc(dato);
	leng[0] = 'A';
	leng[1] = 'B';
	leng[2] = '\0';
	
	pc.putc(leng[0]);
	pc.putc(leng[1]);
	
	pc.printf("\n%d",strlen(buf));
	
	//pulseTest();
	//Quitar?
	/*while(1){
	}*/
	/*
	char *buf = "GET /foo.bar?query=moo HTTP/1.5\r\n";
	struct slre	slre;
	struct cap	captures[4 + 1];
	char *pattern = "^(GET|POST) (\\S+) HTTP/(\\S+?)\r\n";
	if (!slre_compile(&slre, pattern)) {
		pc.printf("Error compiling RE: %s\n", slre.err_str);
	} else if (!slre_match(&slre, buf, strlen(buf), captures)) {
		printf("Not a valid HTTP request\n" );
	} else {
		pc.printf("Request line length: %d\n", captures[0].len);
		pc.printf("Method: %.*s\n", captures[1].len, captures[1].ptr);
		pc.printf("URI: %.*s\n", captures[2].len, captures[2].ptr);
	}*/
}

