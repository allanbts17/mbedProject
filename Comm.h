#include "mbed.h"
#include "objectGroup.h"

class Comm{
	private:
		bool receiveData;
		const int groupLimit;
		int groupIndex;
		bool once;
		int totalPoint[120];
		int pointNum[120];
		float x[120];
		float y[120];
		Serial *pc;
		bool packageSend;
		bool startStore;
		ObjectGroup *group;
		//ObjectGroup *groupList;
	
	public:
		Comm();
		
		void init(){
			receiveData = true;
			groupIndex = 0;
			once = true;
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
		
		void messageVerification(char *buff,int limit){
			char ax[5];
			char ay[5];
			char atotal[1];
			char aactual[1];
			
			int size = limit;//strlen(buff);
			if(receiveData)
			for(int idx=0;idx<size;idx++){
				if(buff[idx]=='&' && idx+21 <= limit){
					if(buff[idx+21]=='@'){

						//Obtain total object an object number
						atotal[0] = buff[idx+1];
						aactual[0] = buff[idx+4];
						
						//Obtain X and Y
						for(int idy=0;idy<5;idy++){
							ax[idy] = buff[idx+idy+7];
							ay[idy] = buff[idx+idy+14];							
						}
						
						if(aactual[0] == '0'){
							startStore = true;
							if(groupIndex < groupLimit)
								group[groupIndex].setObjectCount(atoi(atotal));
						}
						
						if(startStore){
							group[groupIndex].addObject(atoi(aactual),atof(ax),atof(ay));
							if(groupIndex < groupLimit)
							if(group[groupIndex].reviewCount()){
								startStore = false;
								//groupList[objectIndex] = group;
								groupIndex++;
							}
						}
						
						
						/*totalPoint[objectIndex] = atoi(atotal);
						pointNum[objectIndex] = atoi(aactual);
						x[objectIndex] = atof(ax);
						y[objectIndex] = atof(ay);
						objectIndex++;*/
						
						if(groupIndex >= groupLimit){
							if(once){
								once = false;
								receiveData = false;
								for(int idz=0;idz<groupLimit;idz++){
									//pc->printf("%d,%d,%5.3f,%5.3f\n",totalPoint[idz],pointNum[idz],x[idz],y[idz]);
									pc->printf("Grupo %d, Objects number: %d\n",idz,group[idz].getObjectCount()+1);
									for(int i=0;i<=group[idz].getObjectCount();i++){
										pc->printf("ID: %d, X: %5.3f, Y: %5.3f\n",i,group[idz].getX(i),group[idz].getY(i));
									}
									pc->printf("\n");
								}
								receiveData = true;
								once = true;
								groupIndex = 0;
							}
						}
						
						packageSend = true;
						//pc->printf("%d,%d,%5.3f,%5.3f\n",totalPoint[0],pointNum[0],x[0],y[0]);
						//pc->printf("T: %d, A: %d, X: %5.3f, Y: %5.3f\n",totalPoint,pointNum,x,y);
						//pc->printf("%d,%d,%5.2f,%5.2f,%d\n",totalPoint,pointNum,x,y,limit);
						//pc->printf("%c,%d,%d\n",buff[idx],idx,limit);
					}
				}
			}
		}
		
		void printTest(){
			pc->baud(9600);
			pc->printf("print test comm 1\n");
		}
};

Comm::Comm():
groupLimit(20),totalPoint(),
pointNum(),x(),y(),
pc(new Serial(p13,p14)),
packageSend(false),
startStore(false),
group(new ObjectGroup[20])
//groupList(new ObjectGroup[10])
{}
//objectLimit = 120 and 110
//groupList(new ObjectGroup)
