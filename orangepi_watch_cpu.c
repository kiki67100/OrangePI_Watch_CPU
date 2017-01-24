#include <stdlib.h>
#include <stdio.h>

#define SYS_CPU_TEMP  "/sys/devices/virtual/thermal/thermal_zone0/temp"

//For HandleCritital 
#define WGET_PATH "/usr/bin/wget"
#define CHECK_TIME 20
#define LIMIT_INTERVAL_HANDLE_SECOND 60*10 		//Handle can only execute every LIMIT_INTERVAL_HANDLE_SECOND no everytime 

static unsigned int CRITICAL_TEMP=65;			//if cpu_temp more than CRITICAL_TEMP call  Handle_Temp_Critial() 
static unsigned int CRITICAL_LOAD=15;			//if load average more than CRITICAL_LOAD call  Handle_Load_Critital() 
char absolute_path[BUFSIZ];
float uptime;
float last_call=0.00;

void GetUpTime(){
	/*Get UpTime in seconds source http://stackoverflow.com/questions/17728173/linux-get-system-time-since-power-on */
	FILE* proc_uptime_file = fopen("/proc/uptime", "r");
	fscanf(proc_uptime_file, "%f", &uptime);
	printf("[+] UpTime : %f\n",uptime);
	fclose(proc_uptime_file);
}

void Handle_Temp_Critial(int cpu_temp){

	if(uptime-last_call < LIMIT_INTERVAL_HANDLE_SECOND){
		return;
	}
	last_call=uptime;

	unsigned char msg[150];

	sprintf(msg,"%s/critical_temp.sh %d",absolute_path,cpu_temp);

	printf("[+] Handle_Temp_Critial system string : %d\n",cpu_temp);
	printf("[+] system string call : %s\n",msg);
	system(msg);

	
}

void Handle_Load_Critital(load){
	if(uptime-last_call < LIMIT_INTERVAL_HANDLE_SECOND){
		return;
	}
	last_call=uptime;

	unsigned char msg[150];
	sprintf(msg,"%s/critical_load.sh %f",absolute_path,load);
	printf("[+] Handle_Load_Critital system string : %f\n",load);
	printf("[+] system string call : %s\n",msg);
	system(msg);
}

int main(){



	/*Get the absolute path of the current executable */
  	readlink("/proc/self/exe", absolute_path, BUFSIZ);
  	printf("[+] %s ABSOLUTE PATH : \n",dirname(absolute_path));


	unsigned int load;
	double loadavg[3]; 	//Double array get loadavg x times
	unsigned int cpu_temp;		//Store cpu_temp in orangePi temperature is int why ?

	while(1){
		GetUpTime();
		load=getloadavg(loadavg,3); 
		
		int unsigned i=0;
		for(i=0;i<3;i++){
			printf("loadavg(%d) double : %f\n",i,loadavg[i]);
		}

		FILE *temp0;
		temp0 = fopen(SYS_CPU_TEMP,"r");
		if( temp0 == NULL ){
			fprintf(stderr,"[+] Unable to open thermal_zone0/temp");
			return 1;
		}
		fscanf(temp0,"%d",&cpu_temp);
		printf("[+] CPU temperature : %d\n",cpu_temp);
		
		
		if(cpu_temp>CRITICAL_TEMP){
			Handle_Temp_Critial(cpu_temp);
		}else{
			printf("[+] cpu_temp %d < %d no warning\n",cpu_temp,CRITICAL_TEMP);
		}
			
		if(loadavg[0]>CRITICAL_LOAD){
			Handle_Load_Critital(loadavg[0]);

		}else{
			printf("[+] loadavg %f < %d no warning\n",loadavg[0],CRITICAL_LOAD);
		}
			
			sleep(CHECK_TIME);
	}	

	return 0;
}

