#include "mbed.h"
#include "motorControl.h"

DigitalOut led1(LED1);
DigitalOut led2(LED2);
Serial pc1(p13,p14); //tx y rx -- naranja,rojo
motorControl controller;
//motorDriver driver(p22,p23,p21);

InterruptIn a(p26);
InterruptIn b(p27);
	
int i =0;
int msg;


char step[20];

void moveMsg(){
	//char msg = pc1.getc();
	//pc1.putc(msg);
	//pc1.printf("Func.");
	while(pc1.readable()){
	//	pc1.printf("readable.");
		step[i] = pc1.getc();
		i++;
	}
//	pc1.printf("%s",step);
	if(i>1)
	if(!pc1.readable() and step[i-1] == 'a') {
		msg = atoi(step);
		//pc1.printf("Pulsos: %d\n",msg);
		i=0;
		controller.pulse_setpoint = msg;
		controller.last_time = controller.t.read_us();
		controller.run();
	}
}

/*
void Atrigger(){
	pc1.printf("A");
}

void Btrigger(){
	pc1.printf("B");
}*/

int main() 
{
	pc1.attach(moveMsg);
	pc1.baud(9600);
	controller.init();
	
	/*a.rise(Atrigger);
	a.fall(Atrigger);
	b.rise(Btrigger);
	b.fall(Btrigger);
	
	a.mode(PullDown);
	b.mode(PullDown);
	
	
	driver.forward(0.5f);*/
	
	
	
/*	uint16_t b = 0xD1E4;
	int g = 65420;
	char *bytePtr = (char *)&g;
	pc1.putc(*bytePtr);      // reverse the order of these lines if you have endian issues
	pc1.putc(*(bytePtr+1));
	//pc1.puts("\n");
	pc1.puts("Hola mundo con puts \n");
	pc1.printf("hola con printf");       */
  //pc1.printf("Empezo la prueba: \n");
	//pulseTest();
	while(1){

	
	}

}
