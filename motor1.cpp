#include <stdio.h>
#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <prussdrv.h>
#include <pruss_intc_mapping.h>

using namespace std;

#define GPIO_DIR "/sys/class/gpio/"
#define PRU_NUM   0

int main (void)
{
   if(getuid()!=0){
      cout<<"You must run this program as root. Exiting."<<endl;
      exit(EXIT_FAILURE);//use should execute the profram under root environment
   }
   // Initialize structure used by prussdrv_pruintc_intc
   tpruss_intc_initdata pruss_intc_initdata = PRUSS_INTC_INITDATA;

   // Allocate and initialize memory
   prussdrv_init ();
   prussdrv_open (PRU_EVTOUT_0);

   // Map PRU intrrupts
   prussdrv_pruintc_init(&pruss_intc_initdata);
   cout<<"Start, if you want to stop, press the button."<<endl;
   //open GPIO path and export GPIO60 which is  p9_12
    FILE*stream=NULL;
    stream=fopen(GPIO_DIR"gpio60/direction","r+");
    if (stream==NULL)
    {
        stream=fopen(GPIO_DIR"export","w");
        fwrite("60",sizeof(int),2,stream);
        fclose(stream);
        stream=fopen(GPIO_DIR"gpio60/direction","r+");
    }
    fwrite("out",sizeof(char),3,stream);
    fclose(stream);
 //create a loop to enable the program runs again and again
   for (; ;)
   {

      cout<<"Please enter a number represent duty cycle, between 0 and 100:"<<endl;   //Duty cycle, also refer to the period of the output signal
      unsigned int p;  //(0-100)
      cin>>p;
      if (p<=0||p>=100){
          cout<<"Wrong number, please enter a new number:"<<endl;
          cin>>p;      //Avoid wrong number input
      }
      prussdrv_pru_write_memory(PRUSS0_PRU0_DATARAM, 0, &p, 4);   //transform this data to PRU program
      cout<<"Please enter a number represent the speed, when speed is 200, stepper motor rotates 1 circle in 1 second:"<<endl;
      unsigned int s;  //delay factor
      cin>>s;
      if (s<=0){
          cout<<"Wrong number, please enter a new number:"<<endl;
          cin>>s;
      }

      unsigned int v;
      v=(1.0/s)*1000000;              //transform the delay factor to speed, so user can input speed directly
      prussdrv_pru_write_memory(PRUSS0_PRU0_DATARAM, 1, &v, 4);
      //ask user choose direction
      int m;
      cout<<"Choose the direction, 1 for clockwise,2 for counter-clockwise."<<endl;
      cin>>m;
      //two chooses, value 1 output high and 2 output low. The direction is clockwise and anticlockwise
      switch (m){
      case 1:
      stream=fopen(GPIO_DIR"gpio60/value","r+");
      fwrite("1",sizeof(char),1,stream);
      fclose(stream);
      break;
      case 2:
      stream=fopen(GPIO_DIR"gpio60/value","r+");
      fwrite("0",sizeof(char),1,stream);
      fclose(stream);
      break;
 prussdrv_exec_program (PRU_NUM, "./motor1.bin");
   }
   // Wait for event completion from PRU
   int n = prussdrv_pru_wait_event (PRU_EVTOUT_0);
   cout<<"PRU program completed, event number"<<n<<endl;

   // Disable PRU and close memory mappings
   prussdrv_pru_disable(PRU_NUM);
   prussdrv_exit ();
   return EXIT_SUCCESS;
}

