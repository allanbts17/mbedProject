#include "mbed.h"
#include "motorDriver.h"
#include "PID/pid.h"
#include "list.h"

class motorControl{
	private:
		BusIn *encoder; //A y B
		Serial *pc; //tx y rx
		motorDriver *m1; 
		DigitalIn *ia;
		DigitalIn *ib;
		List *lst;
		
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
		bool clockwise;
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
		//control /= 1000; //////// revisar, podría ser necesario que los números sean float para dividirse así
		control = control/1000.0f;
		//if (abs(control) < 0.1f) {m1->brake();pc->printf("Reached: %d\n",pulse);}
		if(control > 0){
			m1->forward(control);
			clockwise = true;
		}
		else if(control < 0) {
			m1->reverse(-control);
			clockwise = false;
		}
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
		bool sendString = false;
		
		if(!sendString){
			valueSerial((uint16_t)pulse_setpoint);
			valueSerial((uint16_t)pulse);
			
			//valueSerial((uint16_t)inc);
		}
		else{
			pc->printf("%5.2f, %5.2f, %5.2f\n",pulse_setpoint*1.0,pulse*1.0,inc);
		}

		signal(inc);
	}

/****************************** Encoder Read function **************/
	//When encoder moves, this function works
	void encoderRead(){
		
		int add = clockwise? 1:-1;
		pulse += add;
		/*int encoderPos = encoder->read();
		if(!start_pulse){
				lst->setIndexByState(encoderPos);
				start_pulse = true;
			}else{
				int add = clockwise? 1:-1;
				pulse += add;
				
				lst->getCurrentState();
				stage = clockwise? lst->next():lst->previous();
			}*/
		run();
	}
	
	

/***************************** Init function ***********************/
	void init(){
		t.start();
		last_time = t.read_us();
		
		lst->initValues();
		
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
lst(new List()),

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
stage(-1),
clockwise(true),
start_pulse(false),
debug_pulse(0)
{}

void motorControl::print(){
	pc -> printf("%d",encoder -> read());
}
		
