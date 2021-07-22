#include "mbed.h"
#include "motorDriver.h"
#include "PID/pid.h"

class motorControl{
	private:
		BusIn *encoder; //A y B
		Serial *pc; //tx y rx
		motorDriver *m1; 
		DigitalIn *ia;
		DigitalIn *ib;
		
		//Interrupts
		InterruptIn *a;
		InterruptIn *b;
		
		//Pulse values
		int pulse;
		int last_pulse;
		const int input_limit;
		double output_limit;
		int ind;
		int stage;
		bool start_pulse;
		
		//PID values
		double dt;
		double max;
		double min;
		double kp;
		double kd;
		double ki;	
		PID *pid;
		
		//Debug values
		int debug_pulse;
		

	public:
		//Constructor
		motorControl();
				
		int pulse_setpoint;
		double last_time;
		Timer t;

/************************To count pulse functions**************************************/
	void print();
	void arise(){
		ind++;
		if(ind == input_limit) {m1 -> brake();print();}
	}
	void afall(){
		ind++;
		if(ind == input_limit) {m1->brake();print();}
	}
	void brise(){
		ind++;
		if(ind == input_limit) {m1->brake();print();}
	}
	void bfall(){
		ind++;
		if(ind == input_limit) {m1->brake();print();}
	}
	
	/************************************* ValueSerial ********************************************/
	void valueSerial(uint16_t sp){
		char *bytePtr = (char *)&sp;
		pc->putc(*bytePtr);
		pc->putc(*(bytePtr+1));
	}
	/************************************** Action functions****************************/
	//Transforming the PID signal for the motor driver
	void signal(double pid){
	int control_limit = 1000;
	//An interpolation function to fix value to a 1000 to -1000 range, and then divide by 1000
	float control = -control_limit + (control_limit/output_limit)*(pid+output_limit);
	control /= 1000; //////// revisar, podría ser necesario que los números sean float para dividirse así
		//if (abs(control) < 0.1f) {m1->brake();pc->printf("Reached: %d\n",pulse);}
		if(control > 0) m1->forward(control);
		else if(control < 0) m1->reverse(-control);
	}

/************************** Run function *****************************/
	void run(){
		//Setting delta time in seconds
		dt = (t.read_us()-last_time)/1000000.0;
		
		//Updating last_time for next step
		last_time = t.read_us();
		
		//Calculate the control signal
		/*steps => setpoint
			pulse => measured value
			dt    => delta time*/
		double inc = pid->calculate(pulse_setpoint*1.0, pulse*1.0, dt);
		//pc->printf("Setpoint: %5.2f, Pulse: %5.2f, Output: %5.2f,delta: %5.2f\n",pulse_setpoint*1.0,pulse*1.0,inc,dt);
		//pc->puts("Setpoint:");
	
		//uint16_t sp = (uint16_t)pulse_setpoint;
		//valueSerial(sp);
		valueSerial((uint16_t)pulse_setpoint);
		//pc->puts(", Pulse:");
		valueSerial((uint16_t)pulse);
		//pc->puts("\n");
		signal(inc);
	}

/****************************** Encoder Read function **************/
	//When encoder moves, this function works
	void encoderRead(){
		//pc.putc('a');
		
		switch(encoder->read()){
		case 0: //1
			//If this is second pulse it has to move twice.
			if(start_pulse) {
				start_pulse = false;
				
				//Add or rest two times
				if(stage == 4) pulse++; //clockwise
				else if(stage == 2) pulse--; //counter-clockwise
			}
			if(stage == -1) start_pulse = true; //If it's first pulse, is imposible to khown direction,
			//so in next pulse it have to count twice.
			//To sum or rest a pusle, depending on last stage
			else{
					if(stage == 4) pulse++; //clockwise
					else if(stage == 2) pulse--; //counter-clockwise
			}
			stage = 1; //Updating stage
			break;
			
		case 1: //2
			if(start_pulse) {
				start_pulse = false;
				
				//Add or rest two times
				if(stage == 1) pulse++; //clockwise
				else if(stage == 3) pulse--; //counter-clockwise
			}
			if(stage == -1) start_pulse = true;
			else{
					if(stage == 1) pulse++; //clockwise
					else if(stage == 3) pulse--; //counter-clockwise
			}
			stage = 2;
			break;
			
		case 3: //3
			if(start_pulse) {
				start_pulse = false;
				
				//Add or rest two times
				if(stage == 1) pulse++; //clockwise
				else if(stage == 4) pulse--; //counter-clockwise
			}
			if(stage == -1) start_pulse = true;
			else{
					if(stage == 1) pulse++; //clockwise
					else if(stage == 4) pulse--; //counter-clockwise
			}
			stage = 3;
			break;
			
		case 2://4
			if(start_pulse) {
				start_pulse = false;
				
				//Add or rest two times
				if(stage == 3) pulse++; //clockwise
				else if(stage == 1) pulse--; //counter-clockwise
			}
			if(stage == -1) start_pulse = true;
			else{
					if(stage == 3) pulse++; //clockwise
					else if(stage == 1) pulse--; //counter-clockwise
			}
			stage = 4;
			break;
		}
		
		//pc->printf("1");
		//pc -> printf("%d",encoder -> read());
		run();
	}

/***************************** Init function ***********************/
	void init(){
		t.start();
		last_time = t.read_us();
		
//	PID pid = PID(dt, max, min, kp, kd, ki);
//	ia->mode(PullDown);
//	ib->mode(PullDown);
		pulse = 0;
		
		a -> rise(this,&motorControl::encoderRead);
		a -> fall(this,&motorControl::encoderRead);
		b -> rise(this,&motorControl::encoderRead);
		b -> fall(this,&motorControl::encoderRead);
		
		//a -> mode(PullDown);
		//b -> mode(PullDown);
	}

	void pulseTest(){
		a -> rise(this,&motorControl::arise);
		a -> fall(this,&motorControl::afall);
		b -> rise(this,&motorControl::brise);
		b -> fall(this,&motorControl::bfall);
		m1-> forward(1.0f);
	}
};
		
motorControl::motorControl():

encoder(new BusIn(p26,p27)),
pc(new Serial(p13,p14)),
m1(new motorDriver(p22,p23,p21)),

ia(new DigitalIn(p26)),
ib(new DigitalIn(p27)),

a(new InterruptIn(p26)),
b(new InterruptIn(p27)),

pulse(0),last_pulse(0),
input_limit(7920),
output_limit((input_limit/2)*0.20),
ind(0),
dt(0.1),
max(output_limit),min(-output_limit),
kp(1),kd(0.2),ki(0.5),

pid(new PID(dt, max, min, kp, kd, ki)),
last_time(0),
stage(-1),start_pulse(false),

debug_pulse(0)
{}

void motorControl::print(){
	pc -> printf("%d",encoder -> read());
}
/*
//Pulse values
int motorControl::pulse = 0;
int motorControl::last_pulse = 0;
const int motorControl::input_limit = 7920;
double motorControl::output_limit = (input_limit/2)*0.20;
int motorControl::ind = 0;

//PID values
double motorControl::dt = 0.1;
double motorControl::max = output_limit;
double motorControl::min = -output_limit;
double motorControl::kp = 1;
double motorControl::ki = 0.5;
double motorControl::kd = 0.2;
PID motorControl::pid = PID(dt, max, min, kp, kd, ki);

int motorControl::stage=-1;
bool motorControl::start_pulse = false;
*/
/*BusIn *encoder; = new BusIn(p26,p27); //A y B
		Serial *pc; = new Serial(p13,p14); //tx y rx
		motorDriver *m1; = new motorDriver(p22,p23,p21);
		DigitalIn *ia; = new DigitalIn(p26);
		DigitalIn *ib; = new DigitalIn(p27);
		//Interrupts
		InterruptIn *a; = new InterruptIn(p26);
		InterruptIn *b; = new InterruptIn(p27);*/
		