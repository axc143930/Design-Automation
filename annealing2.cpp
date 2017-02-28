//*************************************************************************************************************************************//
//*************************************************************************************************************************************//
//Headers
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <time.h>
#include <cstring>
#include <cstdlib>
#include <math.h>
#define BSD //uncomment this line if you are running this on Linux
//#define WIN //uncomment this line if you are running this on Windows
#ifdef BSD
#include<sys/time.h>
#include<sys/resource.h>
#include<unistd.h>
#endif
#ifdef WIN
#include<ctime>
#endif
using namespace std;

//*************************************************************************************************************************************//
//**********************************************************************************************************************************************//
//Structure of Cells containing the data of cell name, area and net numbers that they are present in.
struct cells    
{
    string cell_name;  
    int x;
    int y;
    int z;
    int partition;
    int nets[1200];
    cells *next3;
};

//**********************************************************************************************************************************************//
//Structure for dummies
struct dummy
{
      int xd;
      int yd;
      dummy *next;
};
//**********************************************************************************************************************************************//
//Structure of Nodes in which Cells is used as a data member so as to pass all the properties of the cell into the netlist as well. 
struct node
{
    int net_number;
    cells *p;
    node *next;
};

//*************************************************************************************************************************************//
//Global Variables
void printtofile(node *);
void printarea(string,int);
ofstream output;
int j=0;

//**********************************************************************************************************************************************//
//**********************************************************************************************************************************************//
//This function denotes the start of the linked list of each net. Hence the temp is stored in the head.
void start(node *&head,node *&tail,int x, cells *&y)
{
	node *temp=new node;
	temp->net_number=x;
	temp->p=y;
	temp->next=NULL;
	head=temp;
	tail=temp;
  
}

//**********************************************************************************************************************************************//
//"add" function uses the head of the "start" function and includes the node to it thereby changing the value of the tail everytime the function is called//
void add(node *&head,node *&tail,int x, cells *&y)
{
	node *temp=new node;
	temp->net_number=x;
	temp->p=y;
	temp->next=NULL;
	tail->next=temp;
	tail=temp;
  
} 

//**********************************************************************************************************************************************//
//Creating a linked list of cells which have the attributes = Cell name, area and the nets it is connected to.
void start3(cells *&head3,cells *&tail3,string name,cells *&index)
{
	
	cells *temp3=new cells;
	temp3->cell_name=name;
	temp3->z=0;
	temp3->next3=NULL;
	index=temp3;
	if(j==0)
	{
	  head3=temp3;
	  tail3=temp3;
	}
	else
	{
	  tail3->next3=temp3;
	  tail3=temp3;
	}
	j++;
}

//**********************************************************************************************************************************************//
//**********************************************************************************************************************************************//
//This function stores which nets are connected to the cell.
void printcells(cells *&h3,int q)
{
	      int z0=h3->z;
	      h3->nets[z0]=q;
	      h3->z=(z0)+1;
	      
}

//**********************************************************************************************************************************************//
//This function stores dummy cell coordinates.
void dummycells(dummy *&head,int x,int y)
{
	dummy *temp=new dummy;
	temp->xd=x;
	temp->yd=y;
	temp->next=NULL;
	head=temp;
}

//**********************************************************************************************************************************************//
//**********************************************************************************************************************************************//
//Main code 
int main(int argc,char *arg[])
{
		//Variables
		//n=No. of nets; c=No. of cells; net_data=string of a line for reading .net; are_data=string of a line for reading .are; 
		int k=0,n,c,d,t=0,i,init,flag,cntrl,inner_count;
		string net_data;			
		string are_data;
		cells *cell_location;
		int choose_swap1, choose_swap2;
		
		//To choose a random variable at different instances of time. By using srand the system would not generate the same random numbers on executing the code multiple times.
		time_t clk;
		srand(time(&clk));
				
		//starting and tail pointers of the linked lists.
		struct node *tail=NULL;
		struct cells *head3=NULL;
		struct cells *tail3=NULL;
		struct cells *h3=NULL;
		struct cells *h3_2=NULL;
		struct node *traverse_nets;
		
		//input files
		ifstream net_file;			
		ifstream are_file;
		
		ofstream placement;
		//ofstream graph;
		//graph.open("graph.txt", ios::trunc);
		
		//opening the first argument(file) i.e. .net benchmark file
		net_file.open(arg[1]);
		//opening the second argument(file) i.e. .are benchmark file
		are_file.open(arg[2]);	
		//Creating output file name according to the benchmark's name. For example->team03_output_ibm01.txt
		string st="Chodavarapu_Lakkaraju_output_"+string(arg[1]).substr(0,string(arg[1]).find_first_of('.'))+".txt";
		output.open(st.c_str(), ios::trunc);		
		string st1=string(arg[1]).substr(0,string(arg[1]).find_first_of('.'))+".p";
		placement.open(st1.c_str(), ios::trunc);
			//**********************************************************************************************************************************************//
			//To keep track of the net number we are in.
			int q=0;	
			while(k<4)	
			{
				//Reading .net file line by line and storing the required net numbers.
				getline(net_file,net_data);
			      
				k++;		//k denotes the line number
				
				if(k==3)
				{
				  //converting string to integer
				  istringstream(net_data)>>n;	
				  //cout<<"No. of nets:"<<n<<'\n';
				}
				if(k==4)
				{
				  //converting string to integer
				  istringstream(net_data)>>c;	
				  //cout<<"No. of cells:"<<c<<'\n';
				}
			}
		    
			//Creating an array of nodes with size=Number of nets and an array of cells with size=Number of cells
			struct node *list[n];
			struct cells *index[c];
			//
			int cut_net[n+1],prevcut_net[n+1];
			//**********************************************************************************************************************************************//
		    
			//Reads each line from the .are file unless it is null.
			while(getline(are_file,are_data)!=NULL)	
			{
				    
				index[t]=NULL;
				string name;		
				name=are_data.substr(0,are_data.find_first_of(' '));	//Separating the cell name from the area.
				start3(head3,tail3,name,index[t]);	  	//Creating a linked list of Cells.
				t++;
			}
			h3=head3;
			//**********************************************************************************************************************************************//
			
			k=0;
			//Reading .net file line by line.
			while(getline(net_file,net_data)!=NULL)	
			{
				  
				  
				k++;		
				if(k>1)
				{
						head3=h3;
						if(net_data[net_data.find_first_of(' ')+1]=='s')//Denotes the starting of the new net.
						{
								if(q>0)
								{
									//printtofile(list[q]);	//Makes a linked list of the head of each net and this function also calls the
								}									//printofile function which writes the netlist into the file
								q++;							//Updates the net number.
								list[q]=NULL;					//Creates a new linked list for every new net.
								string str=net_data.substr(0,net_data.find_first_of(' ')); //Decodes the cell name from the line.
								while(head3->cell_name!=str)		//Checks for the cell with the same name in the linked list of Cells.
								{
								      head3=head3->next3;				//traverses the linked list if not found.
								}
								printcells(head3,q);				//if found updates the net number thereby denoting that the cell is connected to this net. 
								start(list[q],tail,q,head3); 		//Creates the linked list of the net and passes the net number and the cell as an 
																      //object. Thereby all the attributes of the cell can be accessed even in the netlist.
						}
						else 
						{
							      //Adds the nodes to the same net.
								string str=net_data.substr(0,net_data.find_first_of(' '));
								while(head3->cell_name!=str)	//Checks for the cell with the same name in the linked list of Cells.
								{
								      head3=head3->next3;			//traverses the linked list if not found.
								}
								printcells(head3,q);			//if found updates the net number thereby denoting that the cell is connected to this net.
								add(list[q],tail,q,head3);		//Adds the node to the net.
					      
						}
						head3=h3;  //Retrieves the starting address of the Cells linked list. 
				}
					
				    
			}
			//**********************************************************************************************************************************************//
			// Writing the name of the benchmark to the file
			output<<string(arg[1]).substr(0,string(arg[1]).find_first_of('.'))<<endl;
			
			//Beginning of the execution time for Partitioning and Simulated Annealing algorithm,
			#ifdef BSD
			rusage time;
			#endif
			#ifdef WIN
			clock_t ex_time;
			start_time = clock();
			#endif
			#ifdef BSD
			//**********************************************************************************************************************************************//
			
			//Setting the Aspect Ratio
			long int width=c*4; 
			long int height=48;
			int sizing=1;
			while(height/width<1)
			{	
			      sizing++;
			      height=height+32;
			      width=(c*4)/sizing;
			}
			width=height;			
			
			
			//Initial Placement
			int X1=0;
			int Y1=16;
			for(i=0;i<c;i++)
			{
			      if(X1<width)
			      {
				    index[i]->x=X1;
				    index[i]->y=Y1;
				    X1+=4;
			      }
			      else
			      {
				    Y1=Y1+32;
				    X1=0;
				    index[i]->x=X1;
				    index[i]->y=Y1;
				    X1+=4;
			      }
			      
			}
			//Dummy Cells: So as to swap dead spaces as well
			int total_cells= (width/4)*(height-16)/32;
			int num_dummies=total_cells-c;
			
			struct dummy *dummy_list[num_dummies];
			for(i=0;i<num_dummies;i++)
			{
			      dummy_list[i]=NULL;
			      if(X1<width)
			      {
				    dummycells(dummy_list[i],X1,Y1);
				    X1+=4;
			      }
			      else
			      {
				    Y1=Y1+32;
				    X1=0;
				    dummycells(dummy_list[i],X1,Y1);
				    X1+=4;
			      }
			      
			}
			
			//Initial Wirelength
			long long int wirelength=0;
			int Xmax,Ymax,Xmin,Ymin;
			long long int prev_netwl[n],netwl[n];
			for(i=1;i<=n;i++)
			{
				Xmax=0;Ymax=0;Xmin=X1;Ymin=Y1;
				traverse_nets=list[i];
				while(traverse_nets!=NULL)
				{
					cell_location=traverse_nets->p;
					if(cell_location->x<Xmin)
					{
					      Xmin=cell_location->x;
					}
					if(cell_location->x>Xmax)
					{
					      Xmax=cell_location->x;
					}
					if(cell_location->y<Ymin)
					{
					      Ymin=cell_location->y;
					}
					if(cell_location->y>Ymax)
					{
					      Ymax=cell_location->y;
					}
					traverse_nets=traverse_nets->next;
				}
				netwl[i]=(Xmax-Xmin)+(Ymax-Ymin);
				prev_netwl[i]=netwl[i];
				wirelength+=netwl[i];
				
			}
			
			
			//**********************************************************************************************************************************************//
			//Finding the mean for the wirelength to obtain the standard deviation and calculate the initial temperature.
			init=-1; int x2,y2;
			long long int total_wl=0;
			long long int temp_wl[100];
			while(init<99)
			{
				init++;
				//Selects a random cell 
				choose_swap1=(unsigned)rand()%(c+num_dummies-1);
				//choose_swap1=21;
				if(choose_swap1>=c)
				{
					//If its a dead space another cell is chosen which is not a dead space.
					choose_swap2=(unsigned)rand()%(c-1);
					//choose_swap2=4;
					x2=dummy_list[choose_swap1-c]->xd;
					y2=dummy_list[choose_swap1-c]->yd;
					dummy_list[choose_swap1-c]->xd=index[choose_swap2]->x;
					dummy_list[choose_swap1-c]->yd=index[choose_swap2]->y;
					index[choose_swap2]->x=x2;
					index[choose_swap2]->y=y2;
					//Updating the Wirelength
					//updates the wirelength by visiting the nets that have been affected by the swap i.e. visits the nets of the cell that are swapped.
					cntrl=index[choose_swap2]->z;
					for(d=0;d<cntrl;d++)
					{
						Xmax=0;Ymax=0;Xmin=X1;Ymin=Y1;
						traverse_nets=list[index[choose_swap2]->nets[d]];
						while(traverse_nets!=NULL)
						{
							cell_location=traverse_nets->p;
							if(cell_location->x<Xmin)
							{
							      Xmin=cell_location->x;
							}
							if(cell_location->x>Xmax)
							{
							      Xmax=cell_location->x;
							}
							if(cell_location->y<Ymin)
							{
							      Ymin=cell_location->y;
							}
							if(cell_location->y>Ymax)
							{
							      Ymax=cell_location->y;
							}
							traverse_nets=traverse_nets->next;
						}
						netwl[index[choose_swap2]->nets[d]]=(Xmax-Xmin)+(Ymax-Ymin);
						wirelength+=netwl[index[choose_swap2]->nets[d]]-prev_netwl[index[choose_swap2]->nets[d]];
						prev_netwl[index[choose_swap2]->nets[d]]=netwl[index[choose_swap2]->nets[d]];
					}
				}
				else if(choose_swap1<c)
				{
					//If not a dead space the other cell is chosen from the over all set
					choose_swap2=(unsigned)rand()%(c+num_dummies-1);
					if(choose_swap2>=c)
					{
						x2=dummy_list[choose_swap2-c]->xd;
						y2=dummy_list[choose_swap2-c]->yd;
						dummy_list[choose_swap2-c]->xd=index[choose_swap1]->x;
						dummy_list[choose_swap2-c]->yd=index[choose_swap1]->y;
						index[choose_swap1]->x=x2;
						index[choose_swap1]->y=y2;
						//Updating the Wirelength
						//updates the wirelength by visiting the nets that have been affected by the swap i.e. visits the nets of the cell that are swapped.
						cntrl=index[choose_swap1]->z;
						for(d=0;d<cntrl;d++)
						{
							Xmax=0;Ymax=0;Xmin=X1;Ymin=Y1;
							traverse_nets=list[index[choose_swap1]->nets[d]];
							while(traverse_nets!=NULL)
							{
								cell_location=traverse_nets->p;
								if(cell_location->x<Xmin)
								{
								      Xmin=cell_location->x;
								}
								if(cell_location->x>Xmax)
								{
								      Xmax=cell_location->x;
								}
								if(cell_location->y<Ymin)
								{
								      Ymin=cell_location->y;
								}
								if(cell_location->y>Ymax)
								{
								      Ymax=cell_location->y;
								}
								traverse_nets=traverse_nets->next;
							}
							netwl[index[choose_swap1]->nets[d]]=(Xmax-Xmin)+(Ymax-Ymin);
							wirelength+=netwl[index[choose_swap1]->nets[d]]-prev_netwl[index[choose_swap1]->nets[d]];
							prev_netwl[index[choose_swap1]->nets[d]]=netwl[index[choose_swap1]->nets[d]];
						}
					}
					else if(choose_swap2<c)
					{
						x2=index[choose_swap2]->x;
						y2=index[choose_swap2]->y;
						index[choose_swap2]->x=index[choose_swap1]->x;
						index[choose_swap2]->y=index[choose_swap1]->y;
						index[choose_swap1]->x=x2;
						index[choose_swap1]->y=y2;
						//Updating the Wirelength
						//updates the wirelength by visiting the nets that have been affected by the swap i.e. visits the nets of the cell that are swapped.
						cntrl=index[choose_swap1]->z;
						for(d=0;d<cntrl;d++)
						{
							Xmax=0;Ymax=0;Xmin=X1;Ymin=Y1;
							traverse_nets=list[index[choose_swap1]->nets[d]];
							while(traverse_nets!=NULL)
							{
								cell_location=traverse_nets->p;
								if(cell_location->x<Xmin)
								{
								      Xmin=cell_location->x;
								}
								if(cell_location->x>Xmax)
								{
								      Xmax=cell_location->x;
								}
								if(cell_location->y<Ymin)
								{
								      Ymin=cell_location->y;
								}
								if(cell_location->y>Ymax)
								{
								      Ymax=cell_location->y;
								}
								traverse_nets=traverse_nets->next;
							}
							netwl[index[choose_swap1]->nets[d]]=(Xmax-Xmin)+(Ymax-Ymin);
							wirelength+=netwl[index[choose_swap1]->nets[d]]-prev_netwl[index[choose_swap1]->nets[d]];
							prev_netwl[index[choose_swap1]->nets[d]]=netwl[index[choose_swap1]->nets[d]];
						}
						cntrl=index[choose_swap2]->z;
						for(d=0;d<cntrl;d++)
						{
							Xmax=0;Ymax=0;Xmin=X1;Ymin=Y1;
							traverse_nets=list[index[choose_swap2]->nets[d]];
							while(traverse_nets!=NULL)
							{
								cell_location=traverse_nets->p;
								if(cell_location->x<Xmin)
								{
								      Xmin=cell_location->x;
								}
								if(cell_location->x>Xmax)
								{
								      Xmax=cell_location->x;
								}
								if(cell_location->y<Ymin)
								{
								      Ymin=cell_location->y;
								}
								if(cell_location->y>Ymax)
								{
								      Ymax=cell_location->y;
								}
								traverse_nets=traverse_nets->next;
							}
							netwl[index[choose_swap2]->nets[d]]=(Xmax-Xmin)+(Ymax-Ymin);
							wirelength+=netwl[index[choose_swap2]->nets[d]]-prev_netwl[index[choose_swap2]->nets[d]];
							prev_netwl[index[choose_swap2]->nets[d]]=netwl[index[choose_swap2]->nets[d]];
						}
					}
				}
				temp_wl[init]=wirelength;
				total_wl+=temp_wl[init];
				
			}
			//End of sampling 
			float summation=0;
			for(init=0;init<100;init++)
			{
			      summation+=(float)((temp_wl[init]-(total_wl/100))*(temp_wl[init]-(total_wl/100)));
			}
			//Standard Deviation
			float std_dev=sqrt(summation/100);
			//Initial Temperature calculated using the Standard deviation.
			float initial_temp=(-std_dev/log(0.65));
			//cout<<"Summation:"<<summation<<"\tStd.Dev:"<<std_dev<<"\tInitial Temp:"<<initial_temp<<endl;
			
			//graph<<"Temperature\t"<<"Wirelength"<<endl;
			//Simulated Annealing
			int dependancy=c/2;int x3,y3;
			long long int delta;
			float random_prob,prob;
			int iter=0;
			float start_temp=initial_temp;
			long long int initial_wl=temp_wl[99];
			long long int start_wl=initial_wl;
			while(initial_temp>=pow(10,-40))
			{
				inner_count=dependancy;
				//Temperature Equilibrium is represented by inner_count
				while(inner_count!=0)
				{
					iter++;
					if(initial_temp>=pow(10,-20))
					{
						random_prob=0.01*((unsigned)rand()%100);
						
					}
					else
					{
						random_prob=0.5;
					}
						
					
					//Initializing
					wirelength=initial_wl;
					//Selects a random cell 
					choose_swap1=(unsigned)rand()%(c+num_dummies-1);
					//choose_swap1=21;
					if(choose_swap1>=c)
					{
						//If its a dead space another cell is chosen which is not a dead space.
						choose_swap2=(unsigned)rand()%(c-1);
						//choose_swap2=4;
						flag=1;
						x2=dummy_list[choose_swap1-c]->xd;
						y2=dummy_list[choose_swap1-c]->yd;
						x3=index[choose_swap2]->x;
						y3=index[choose_swap2]->y;
						dummy_list[choose_swap1-c]->xd=x3;
						dummy_list[choose_swap1-c]->yd=y3;
						index[choose_swap2]->x=x2;
						index[choose_swap2]->y=y2;
						//Updating the Wirelength
						//updates the wirelength by visiting the nets that have been affected by the swap i.e. visits the nets of the cell that are swapped.
						cntrl=index[choose_swap2]->z;
						for(d=0;d<cntrl;d++)
						{
							Xmax=0;Ymax=0;Xmin=X1;Ymin=Y1;
							traverse_nets=list[index[choose_swap2]->nets[d]];
							while(traverse_nets!=NULL)
							{
								cell_location=traverse_nets->p;
								if(cell_location->x<Xmin)
								{
								      Xmin=cell_location->x;
								}
								if(cell_location->x>Xmax)
								{
								      Xmax=cell_location->x;
								}
								if(cell_location->y<Ymin)
								{
								      Ymin=cell_location->y;
								}
								if(cell_location->y>Ymax)
								{
								      Ymax=cell_location->y;
								}
								traverse_nets=traverse_nets->next;
							}
							netwl[index[choose_swap2]->nets[d]]=(Xmax-Xmin)+(Ymax-Ymin);
							wirelength+=netwl[index[choose_swap2]->nets[d]]-prev_netwl[index[choose_swap2]->nets[d]];
						}
					}
					else if(choose_swap1<c)
					{
						//If not a dead space the other cell is chosen from the over all set
						choose_swap2=(unsigned)rand()%(c+num_dummies-1);
						if(choose_swap2>=c)
						{
							flag=2;
							x2=dummy_list[choose_swap2-c]->xd;
							y2=dummy_list[choose_swap2-c]->yd;
							x3=index[choose_swap1]->x;
							y3=index[choose_swap1]->y;
							dummy_list[choose_swap2-c]->xd=x3;
							dummy_list[choose_swap2-c]->yd=y3;
							index[choose_swap1]->x=x2;
							index[choose_swap1]->y=y2;
							//Updating the Wirelength
							//updates the wirelength by visiting the nets that have been affected by the swap i.e. visits the nets of the cell that are swapped.
							cntrl=index[choose_swap1]->z;
							for(d=0;d<cntrl;d++)
							{
								Xmax=0;Ymax=0;Xmin=X1;Ymin=Y1;
								traverse_nets=list[index[choose_swap1]->nets[d]];
								while(traverse_nets!=NULL)
								{
									cell_location=traverse_nets->p;
									if(cell_location->x<Xmin)
									{
									      Xmin=cell_location->x;
									}
									if(cell_location->x>Xmax)
									{
									      Xmax=cell_location->x;
									}
									if(cell_location->y<Ymin)
									{
									      Ymin=cell_location->y;
									}
									if(cell_location->y>Ymax)
									{
									      Ymax=cell_location->y;
									}
									traverse_nets=traverse_nets->next;
								}
								netwl[index[choose_swap1]->nets[d]]=(Xmax-Xmin)+(Ymax-Ymin);
								wirelength+=netwl[index[choose_swap1]->nets[d]]-prev_netwl[index[choose_swap1]->nets[d]];
								
							}
						}
						else if(choose_swap2<c)
						{
							flag=3;
							x2=index[choose_swap2]->x;
							y2=index[choose_swap2]->y;
							x3=index[choose_swap1]->x;
							y3=index[choose_swap1]->y;
							index[choose_swap2]->x=x3;
							index[choose_swap2]->y=y3;
							index[choose_swap1]->x=x2;
							index[choose_swap1]->y=y2;
							//Updating the Wirelength
							//updates the wirelength by visiting the nets that have been affected by the swap i.e. visits the nets of the cell that are swapped.
							cntrl=index[choose_swap1]->z;
							for(d=0;d<cntrl;d++)
							{
								Xmax=0;Ymax=0;Xmin=X1;Ymin=Y1;
								traverse_nets=list[index[choose_swap1]->nets[d]];
								while(traverse_nets!=NULL)
								{
									cell_location=traverse_nets->p;
									if(cell_location->x<Xmin)
									{
									      Xmin=cell_location->x;
									}
									if(cell_location->x>Xmax)
									{
									      Xmax=cell_location->x;
									}
									if(cell_location->y<Ymin)
									{
									      Ymin=cell_location->y;
									}
									if(cell_location->y>Ymax)
									{
									      Ymax=cell_location->y;
									}
									traverse_nets=traverse_nets->next;
								}
								netwl[index[choose_swap1]->nets[d]]=(Xmax-Xmin)+(Ymax-Ymin);
								wirelength+=netwl[index[choose_swap1]->nets[d]]-prev_netwl[index[choose_swap1]->nets[d]];
								
							}
							cntrl=index[choose_swap2]->z;
							for(d=0;d<cntrl;d++)
							{
								Xmax=0;Ymax=0;Xmin=X1;Ymin=Y1;
								traverse_nets=list[index[choose_swap2]->nets[d]];
								while(traverse_nets!=NULL)
								{
									cell_location=traverse_nets->p;
									if(cell_location->x<Xmin)
									{
									      Xmin=cell_location->x;
									}
									if(cell_location->x>Xmax)
									{
									      Xmax=cell_location->x;
									}
									if(cell_location->y<Ymin)
									{
									      Ymin=cell_location->y;
									}
									if(cell_location->y>Ymax)
									{
									      Ymax=cell_location->y;
									}
									traverse_nets=traverse_nets->next;
								}
								netwl[index[choose_swap2]->nets[d]]=(Xmax-Xmin)+(Ymax-Ymin);
								wirelength+=netwl[index[choose_swap2]->nets[d]]-prev_netwl[index[choose_swap2]->nets[d]];
								
							}
						}
					}
					//Calculating delta to determine the probability of acceptance												
					delta=initial_wl-wirelength;
					//As the temperature goes low the probability of accepting a bad solution also decreases.
					if(delta<0)
					      prob=exp(delta/initial_temp);
					else prob=1;
					//If the below condition is satisfied the wirelength is updated and the swap is validated
					if(random_prob<=prob)
					{
						//updating wirelength
						initial_wl=wirelength;
						//Updating the wirelength for the nets that were modified
						if(flag==1)
						{
							cntrl=index[choose_swap2]->z;
							for(d=0;d<cntrl;d++)
							{
								prev_netwl[index[choose_swap2]->nets[d]]=netwl[index[choose_swap2]->nets[d]];
							}
						}
						else if(flag==2)
						{
							cntrl=index[choose_swap1]->z;
							for(d=0;d<cntrl;d++)
							{
								prev_netwl[index[choose_swap1]->nets[d]]=netwl[index[choose_swap1]->nets[d]];
							}
						}
						else if(flag==3)
						{
							cntrl=index[choose_swap1]->z;
							for(d=0;d<cntrl;d++)
							{
								prev_netwl[index[choose_swap1]->nets[d]]=netwl[index[choose_swap1]->nets[d]];
							}
							cntrl=index[choose_swap2]->z;
							for(d=0;d<cntrl;d++)
							{
								prev_netwl[index[choose_swap2]->nets[d]]=netwl[index[choose_swap2]->nets[d]];
							}
						}
					}
					else
					{
						wirelength=initial_wl;
						if(flag==1)
						{
							cntrl=index[choose_swap2]->z;
							for(d=0;d<cntrl;d++)
							{
								netwl[index[choose_swap2]->nets[d]]=prev_netwl[index[choose_swap2]->nets[d]];
							}
							dummy_list[choose_swap1-c]->xd=x2;
							dummy_list[choose_swap1-c]->yd=y2;
							index[choose_swap2]->x=x3;
							index[choose_swap2]->y=y3;
						}
						else if(flag==2)
						{
							cntrl=index[choose_swap1]->z;
							for(d=0;d<cntrl;d++)
							{
								netwl[index[choose_swap1]->nets[d]]=prev_netwl[index[choose_swap1]->nets[d]];
							}
							dummy_list[choose_swap2-c]->xd=x2;
							dummy_list[choose_swap2-c]->yd=y2;
							index[choose_swap1]->x=x3;
							index[choose_swap1]->y=y3;
						}
						else if(flag==3)
						{
							cntrl=index[choose_swap1]->z;
							for(d=0;d<cntrl;d++)
							{
								netwl[index[choose_swap1]->nets[d]]=prev_netwl[index[choose_swap1]->nets[d]];
							}
							cntrl=index[choose_swap2]->z;
							for(d=0;d<cntrl;d++)
							{
								netwl[index[choose_swap2]->nets[d]]=prev_netwl[index[choose_swap2]->nets[d]];
							}
							index[choose_swap2]->x=x2;
							index[choose_swap2]->y=y2;
							index[choose_swap1]->x=x3;
							index[choose_swap1]->y=y3;
							
						}
						
					}
					//cout<<wirelength<<endl;	
					inner_count--;
					//graph<<initial_temp<<"\t"<<wirelength<<endl;
				}
				//Exponential Decrease of temperature.
				//graph<<initial_temp<<"\t"<<wirelength<<endl;
				initial_temp=0.9*initial_temp;
				
				
			}
			//End of Simulated Annealing Algorithm 	
			//**********************************************************************************************************************************************//
			#endif
			#ifdef BSD
			getrusage(RUSAGE_SELF,&time);
			printf("Execution time is:[%f] seconds.\n",(double)(1.0*time.ru_utime.tv_sec+0.000001*time.ru_utime.tv_usec));
			output<<"Execution time: "<<(double)(1.0*time.ru_utime.tv_sec+0.000001*time.ru_utime.tv_usec)<<"seconds"<<endl;
			#endif
			#ifdef WIN
			ex_time= clock()-ex_time;
			printf ("Execution time is: [%f]seconds.\n",ex_time,((float)ex_time)/CLOCKS_PER_SEC);
			#endif
			//Execution time is calculated above
			output<<"Initial Wire length: "<<start_wl<<endl;
			output<<"Final Wire length: "<<wirelength<<endl;
			float perc_change=(float)100*(start_wl-wirelength)/start_wl;
			output<<"Percentage change: "<<perc_change<<endl;
			float AR=(float) width/height;
			output<<"Aspect Ratio: "<<AR<<endl;
			output<<"WidthXHeight of the chip: "<<width<<" X "<<height<<endl;
			//Writing the results to the output
			placement<<"First set of coordinates are the dimensions of the chip"<<endl;
			placement<<width<<"\t"<<height<<endl;//First line represents the height and width of the chip
			
			for(i=0;i<c;i++)
			{
			         placement<<index[i]->x<<"\t"<<index[i]->y<<endl;		      
			}
			
			
	net_file.close();
	are_file.close();
	output.close();
	placement.close();
	//Closing the files
	return 0;
}
//End of Main
//**********************************************************************************************************************************************//  
//**********************************************************************************************************************************************//
//**********************************************************************************************************************************************//
//**********************************************************************************************************************************************//

