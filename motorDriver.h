#include "mbed.h"

class motorDriver{
	private:
		DigitalOut* in1;
		DigitalOut* in2;
		PwmOut* ena;
		bool direction;

	public:
		//Constructor
		motorDriver(PinName _in1,PinName _in2,PinName _ena);
		void setPeriod_us(float period){
			ena -> period_us(period);
		}
		
		void brake(){
			in1 -> write(0);
			in2 -> write(0);
		}
		
		void floating(){
			in1 -> write(1);
			in2 -> write(1);
			ena -> write(1.0f);
		}
		
		void forward(float speed){
			direction = true;
			in1 -> write(1);
			in2 -> write(0);
			ena -> write(speed);
		}
		
		void reverse(float speed){
			direction = false;
			in1 -> write(0);
			in2 -> write(1);
			ena -> write(speed);
		}
		
		void fullForward(){
			direction = true;
			in1 -> write(1);
			in2 -> write(0);
			ena -> write(1.0f);
		}
		
		void fullReverse(){
		direction = false;
			in1 -> write(0);
			in2 -> write(1);
			ena -> write(1.0f);
		}
};

motorDriver::motorDriver(PinName _in1,PinName _in2,PinName _ena):
in1(new DigitalOut(_in1)), in2(new DigitalOut(_in2)), ena(new PwmOut(_ena)){}

	
