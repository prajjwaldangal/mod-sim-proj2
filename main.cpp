#include <iostream>
#include <cmath>
#include "lcgrand.h"
#include <time.h>
// #include "Header.h"
#include <vector>

using namespace std;

// definitions, statistics and
/////////////////////////////////////////////////////////////////////////
const int Queu_Limit=1000;
const int BUSY=1;
const int IDLE=0;

int
choice,
Num_Completed_Customers,    //Number of Completed Customers
Number_of_Events,           //Number of Events 1.Arriving 2.Completion
Number_in_Queue,            //Number of Customers In Queue
// Server1_Status,              //Server Status ( Idle , Busy )
// Server2_Status,
Number_of_Servers,
C_Index;

double
End_Time,
Type_Next_Event,
Mean_interArrival_Time,
Mean_service_Time,
Clock,
Time_Arrival[Queu_Limit + 1],
Service_Time[Queu_Limit + 1],
Next_Arrival_Time,
Next_Completion_Time1,
Next_Service_Time,
Total_Flow_Time,
Progres_Arrival_Time,
Progres_Completion_Time,
Waiting_Time;

vector <double> Completion_Time;
vector <int> server_status; // 0 -> idle, 1 -> busy



////////////////////////////////////////////////////////////////////////////
// Prototype of the System Function

void  initialize();
void  Timing();
void  Arrival();
void  Completition();
float expon(float mean);
void  Search_Min(double[],double[]);


float urand(){
    // get unsigned random values
    srand(time(NULL));
    return (float)rand() / (float)((unsigned)RAND_MAX + 1);
}

float genexp(float lambda){
    // generate random value based on exponential distribution
    float u,x;
    u = urand();
    x= log(u)* (-lambda);
    return(x);
}

//////////////////////////////////////////////////////////////////////////////
//Intialization Function

void initialize()
{
    Number_of_Servers = 2;
    
    Number_of_Events = 2;      // Arrival , Completion
    
    Mean_interArrival_Time= 3.0;
    
    Mean_service_Time= 5.0;
    
    End_Time=480.0;
    
    Clock = 0.0;
    
    Number_in_Queue = 0;
    
    Num_Completed_Customers = 0;
    
    Total_Flow_Time = 0.0;
    
    Waiting_Time = 0.0;
    
    Next_Arrival_Time = Clock + expon(Mean_interArrival_Time); //Arriving
    
    Next_Service_Time = expon(Mean_service_Time);
    
    Next_Completion_Time1 = 1.0e+10;    // Completing  Guarantening that the first event is arriving
    
    Progres_Arrival_Time=0.0;
    
    Progres_Completion_Time = 0.0;
    
    for(int i = 0; i<Number_of_Servers;i++)
    {
        Completion_Time.push_back(1.0e+10);
        // printf("%f\n",Completion_Time[i]);
    }
    
    C_Index = 0;
    
    // masfo-vanication -> re-relocation
    for(int i = 0; i<Number_of_Servers; i++)
    {
        server_status.push_back(0);
        // printf("%f\n", server_status[i]);
    }
}

void Timing() {
    Type_Next_Event = 0;
    Next_Completion_Time1 = 1.0e10;
    for (int i=0; i < Completion_Time.size(); i++)
    {
        if (Completion_Time[i] < Next_Completion_Time1)
        {
            Next_Completion_Time1 = Completion_Time[i];
            C_Index = i;
        }
    }
    if (Next_Arrival_Time < Next_Completion_Time1)
    {
        Type_Next_Event = 1;
        Clock = Next_Arrival_Time;
    }
    else
    {
        Type_Next_Event = 2;
        Clock = Next_Completion_Time1;
    }
}

void Arrival() {
    // either serve or put in queue
    
    bool any_server_idle = false;
    // int index;
    
    for (int i=0; i< server_status.size(); i++)
    {
        if (server_status[i] == 0)
        {
            any_server_idle = true;
            server_status[i] = 1;
            C_Index = i;
        }
    }
    if (any_server_idle)
    {
        ++ Num_Completed_Customers;
        Completion_Time[C_Index] = Clock + Next_Service_Time;
        
        Progres_Arrival_Time = Next_Arrival_Time;
        
        Progres_Completion_Time = Completion_Time[C_Index];
        
    } else
    {
        ++ Number_in_Queue;
        if (Number_in_Queue > Queu_Limit)
        {
            cout<<"\nOverflow of the array time_arrival at";
            
            cout<<"time: "<<Clock;
            
            exit(2);
        }
        Time_Arrival[Number_in_Queue] = Clock;
        Service_Time[Number_in_Queue] = Next_Service_Time;
        // see here: Time_Arrival
    }
    
    Next_Arrival_Time = Clock + expon(Mean_interArrival_Time);
    
    Next_Service_Time = expon(Mean_service_Time);
}

void Completion()
{
    ++ Num_Completed_Customers;
    double Delay;
    
    Total_Flow_Time += Progres_Completion_Time - Progres_Arrival_Time;
    
    if (Number_in_Queue == 0)
    {
        Completion_Time[C_Index] =1.0e+10;  // high value
        server_status[C_Index] = 0;
    } else
    {
        Delay = Clock - Time_Arrival[1];
        Waiting_Time += Delay;
        
        Next_Completion_Time1 = Clock + Service_Time[1];
        Progres_Arrival_Time = Time_Arrival[1];
        
        Progres_Completion_Time = (Completion_Time[C_Index] < Next_Completion_Time1) ? Completion_Time[C_Index]  :Next_Completion_Time1;
        
        --Number_in_Queue;
        
        for (int i=1;i<=Number_in_Queue;i++)
        {
            Time_Arrival[i] = Time_Arrival[i + 1];
            
            Service_Time[i] = Service_Time[i + 1];
        }
        
    }
}

///////////////////////////////////////////////////////////////////////////
// Generate The Rondom Number

float expon(float mean)
{
    srand(time(NULL));
    int k = rand() % 5;
    return (-mean * log(lcgrand(k)));
}

////////////////////////////////////////////////////////////////////////////////////
//Main Function

int main()
{
    initialize();           // Intialization of the System
    
    cout<<"                 * Two-server queueing system with fixed run *\n";
    
    cout<<"\nMean Inter arrival Time: "<<Mean_interArrival_Time;
    
    cout<<"\nMean Service Time: "<<Mean_service_Time<<endl;
    
    cout<<"The End of Simulation Time: "<<End_Time<<endl<<endl;
    
    
    while(true)
    {
        
        Timing();  // Timing Routine To Determine The Next Event
        
        if (Clock > End_Time)
            break;
        //printf("\nIA time: %f", genexp(5));
        
        switch (int(Type_Next_Event))
        {
                
            case 1:
                Arrival();
                break;
                
            case 2:
                Completion();
                break;
        }
    }
    
    // Print Summary Statistics.
    
    // cout <<"\nAverage delay: "<< Delay
    
    cout<<"\nTotal Flow Time: "<<Total_Flow_Time;
    
    cout<<"\nTotal Waiting Time in Queue: "<<Waiting_Time;
    
    cout<<"\nAverage Waiting Time (Delay) in Queue: "<< Waiting_Time / Num_Completed_Customers;
    
    cout<<"\nAverage Flow Time: "<<Total_Flow_Time / Num_Completed_Customers;
    
    cout<<"\nNumber of Completed Customers: "<<Num_Completed_Customers;
    
    cout<<"\nAverage Number of Customers In System / Unit Time: "<<Num_Completed_Customers / Clock<<endl<<endl;
    
    return 0;
}


