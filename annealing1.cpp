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
    int area;
    int z;
    int old_part;
    int partition;
    int nets[1200];
    cells *next3;
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
void start3(cells *&head3,cells *&tail3,string name,int a, cells *&index)
{
	
	cells *temp3=new cells;
	temp3->cell_name=name;
	temp3->area=a;
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
//**********************************************************************************************************************************************//
//Main code 
int main(int argc,char *arg[])
{
		//Variables
		//n=No. of nets; c=No. of cells; net_data=string of a line for reading .net; are_data=string of a line for reading .are; 
		int k=0,n,c,d,t=0,i,init,flag,cntrl,inner_count;
		string net_data;			
		string are_data;
		int total_cutset=0;
		int area1=0;
		int area2=0;
		int set1=0;
		int set2=0;
		int start_cut, final_cut;
		float perc_change,ratio_cut;
		float prob;
	        float summation=0;
		int cutset=0,initial_cutset=0;
		int temp_cutset[20];
		float std_dev,initial_temp;
		int cuts,total_area,delta;
		cells *cell_partition;
		int choose_swap1, choose_swap2, new_cutset;
		
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
		
		//opening the first argument(file) i.e. .net benchmark file
		net_file.open(arg[1]);
		//opening the second argument(file) i.e. .are benchmark file
		are_file.open(arg[2]);	
		//Creating output file name according to the benchmark's name. For example->team03_output_ibm01.txt
		string st="team03_output_"+string(arg[1]).substr(0,string(arg[1]).find_first_of('.'))+".txt";
		output.open(st.c_str(), ios::trunc);		
			
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
				  cout<<"No. of nets:"<<n<<'\n';
				}
				if(k==4)
				{
				  //converting string to integer
				  istringstream(net_data)>>c;	
				  cout<<"No. of cells:"<<c<<'\n';
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
				string ar,name;		
				int a;
				name=are_data.substr(0,are_data.find_first_of(' '));	//Separating the cell name from the area.
				ar=are_data.substr(are_data.find_first_of(' '),are_data.find_first_of('\n'));
				istringstream(ar)>>a;			//Changing area into an integer.
				start3(head3,tail3,name,a,index[t]);	  	//Creating a linked list of Cells.
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
			//Start time
			//Initial Partition according to their areas. Partitioned such that the areas are almost equal. 
			for(i=0;i<c;i++)
			{
				if(area1<=area2)
				{
					//updating the partition
					index[i]->partition=1;
					//updating area1 
					area1+=index[i]->area;
					
				}
				else if(area1>area2)
				{
					//updating the partition
					index[i]->partition=2;
					//updating area2
					area2+=index[i]->area;
					
				}
			}
			//Total area of all the cells.
			total_area=area1+area2;
						
			for(i=1;i<=n;i++)
			{
				set1=0;
				set2=0;
				traverse_nets=list[i];
				while(traverse_nets!=NULL)
				{
					cell_partition=traverse_nets->p;  
					if(cell_partition->partition==1)
					{
						set1++;
					}
					else if(cell_partition->partition==2)
					{
						set2++;
					}
					traverse_nets=traverse_nets->next;
				}
				if(set1==0 || set2==0)
					cuts=0;
				else cuts=1;
				cut_net[i]=cuts;
				prevcut_net[i]=cuts;
				cutset+=cuts;
				
			}
			initial_cutset=cutset;
			
			start_cut=initial_cutset;
			
			init=-1;
			while(init<19)
			{
				
				
				choose_swap1=(unsigned)rand()%(c-1);
				if(index[choose_swap1]->partition==1)
				{
					choose_swap2=(unsigned)rand()%(c-1);
					while((index[choose_swap2]->partition)!=2 )
					{	
					    choose_swap2=(unsigned)rand()%(c-1);
					}
					
					area2=area2-(index[choose_swap2]->area)+(index[choose_swap1]->area);
					area1=area1-(index[choose_swap1]->area)+(index[choose_swap2]->area);
					
					flag=0;
				      
				}
				else if(index[choose_swap1]->partition==2)
				{
					choose_swap2=(unsigned)rand()%(c-1);
					while((index[choose_swap2]->partition)!=1)
					{	
					      choose_swap2=(unsigned)rand()%(c-1);
					}
					area2=area2-(index[choose_swap1]->area)+(index[choose_swap2]->area);
					area1=area1-(index[choose_swap2]->area)+(index[choose_swap1]->area);
					
					flag=1;
				}
				     
				
				if((100*(area1-area2)/total_area)>=-10 && (100*(area1-area2)/total_area)<=10)
				{
					init++;
					temp_cutset[init]=0;
					
					if(flag==0)
					{		
						index[choose_swap1]->partition=2;						    
						index[choose_swap2]->partition=1; 						    
					}
					else if(flag==1)
					{
					      
						index[choose_swap1]->partition=1;						      
						index[choose_swap2]->partition=2;						    
					}
					
					cntrl=index[choose_swap1]->z;					
					for(d=0;d<cntrl;d++)
					{
						set1=0;
						set2=0;
						traverse_nets=list[index[choose_swap1]->nets[d]];
						while(traverse_nets!=NULL)
						{
							cell_partition=traverse_nets->p;  
							if(cell_partition->partition==1)
							{
								set1++;
							}
							else if(cell_partition->partition==2)
							{
								set2++;
							}
							traverse_nets=traverse_nets->next;
						}
						if(set1==0 || set2==0)
							cuts=0;
						else cuts=1;
						
						if(cuts==cut_net[index[choose_swap1]->nets[d]])
						{
							cutset=cutset;
							temp_cutset[init]=cutset;
						}
						else if(cuts==0 && cut_net[index[choose_swap1]->nets[d]]==1)
						{
							cutset=cutset-1;
							temp_cutset[init]=cutset;
						}
						else if(cuts==1 && cut_net[index[choose_swap1]->nets[d]]==0)
						{
							cutset=cutset+1;
							temp_cutset[init]=cutset;
						}
						cut_net[index[choose_swap1]->nets[d]]=cuts;
						prevcut_net[index[choose_swap1]->nets[d]]=cuts;
					}
					cntrl=index[choose_swap2]->z;					
					for(d=0;d<cntrl;d++)
					{
						set1=0;
						set2=0;
						traverse_nets=list[index[choose_swap2]->nets[d]];
						while(traverse_nets!=NULL)
						{
							cell_partition=traverse_nets->p;  
							if(cell_partition->partition==1)
							{
								set1++;
							}
							else if(cell_partition->partition==2)
							{
								set2++;
							}
							traverse_nets=traverse_nets->next;
						}
						if(set1==0 || set2==0)
						    cuts=0;
						else cuts=1;
						
						if(cuts==cut_net[index[choose_swap2]->nets[d]])
						{
							cutset=cutset;
							temp_cutset[init]=cutset;							
						}
						else if(cuts==0 && cut_net[index[choose_swap2]->nets[d]]==1)
						{
							cutset=cutset-1;
							temp_cutset[init]=cutset;							
						}
						else if(cuts==1 && cut_net[index[choose_swap2]->nets[d]]==0)
						{
							cutset=cutset+1;
							temp_cutset[init]=cutset;						
						}
						cut_net[index[choose_swap2]->nets[d]]=cuts;
						prevcut_net[index[choose_swap2]->nets[d]]=cuts;
					}
					total_cutset+=temp_cutset[init];
				}
				else
				{
					if(flag==0)
					{
						  area2=area2-(index[choose_swap1]->area)+(index[choose_swap2]->area);
						  area1=area1-(index[choose_swap2]->area)+(index[choose_swap1]->area);
						 
					}
					else if(flag==1)
					{
						area2=area2-(index[choose_swap2]->area)+(index[choose_swap1]->area);
						area1=area1-(index[choose_swap1]->area)+(index[choose_swap2]->area);
						
					}
				
				}
				
			}
			
			for(init=0;init<20;init++)
			{
			      summation+=((temp_cutset[init]-(total_cutset/20))*(temp_cutset[init]-(total_cutset/20)));
			}
			
			std_dev=sqrt(summation/20);
			
			initial_temp=(-std_dev/log(0.5));
			
			inner_count=c;
			initial_cutset=temp_cutset[19];
			
			while(initial_temp>=0.0001)
			{
				
				inner_count=c/2;
				while(inner_count!=0)
				{
					
					cutset=initial_cutset;
					choose_swap1=(unsigned)rand()%(c-1);
					if(index[choose_swap1]->partition==1)
					{
						choose_swap2=(unsigned)rand()%(c-1);
						while((index[choose_swap2]->partition)!=2)
						{	
							choose_swap2=(unsigned)rand()%(c-1);
						}
						  
						area2=area2-(index[choose_swap2]->area)+(index[choose_swap1]->area);
						area1=area1-(index[choose_swap1]->area)+(index[choose_swap2]->area);
						    
						flag=0;
						    
					}
					else if(index[choose_swap1]->partition==2)
					{
						choose_swap2=(unsigned)rand()%(c-1);
						while((index[choose_swap2]->partition)!=1)
						{	
							choose_swap2=(unsigned)rand()%(c-1);
						}
						area2=area2-(index[choose_swap1]->area)+(index[choose_swap2]->area);
						area1=area1-(index[choose_swap2]->area)+(index[choose_swap1]->area);
						    
						flag=1;
					}
					    
					
					if((100*(area1-area2)/total_area)>=-10 && (100*(area1-area2)/total_area)<=10)
					{
						      
						inner_count--;
						if(flag==0)
						{		
						      //cout<<"1st if"<<endl;
						      index[choose_swap1]->partition=2;
						      index[choose_swap2]->partition=1; 
						}
						else if(flag==1)
						{
						      //cout<<"2nd if"<<endl;
						      index[choose_swap1]->partition=1;
						      index[choose_swap2]->partition=2;
						}
						
						cntrl=index[choose_swap1]->z;
						for(d=0;d<cntrl;d++)
						{
							set1=0;
							set2=0;
							traverse_nets=list[index[choose_swap1]->nets[d]];
							while(traverse_nets!=NULL)
							{
							      cell_partition=traverse_nets->p;  
							      if(cell_partition->partition==1)
							      {
								      set1++;
							      }
							      else if(cell_partition->partition==2)
							      {
								      set2++;
							      }
							      traverse_nets=traverse_nets->next;
							}
							if(set1==0 || set2==0)
							    cuts=0;
							else cuts=1;
							if(cuts==cut_net[index[choose_swap1]->nets[d]])
							{
							      cutset=cutset;
							      new_cutset=cutset;
							      
							}
							else if(cuts==0 && cut_net[index[choose_swap1]->nets[d]]==1)
							{
							      cutset=cutset-1;
							      new_cutset=cutset;
							}
							else if(cuts==1 && cut_net[index[choose_swap1]->nets[d]]==0)
							{
							      cutset=cutset+1;
							      new_cutset=cutset;
							      
							}
							cut_net[index[choose_swap1]->nets[d]]=cuts;
						}
						cntrl=index[choose_swap2]->z;
						for(d=0;d<cntrl;d++)
						{
							set1=0;
							set2=0;
							traverse_nets=list[index[choose_swap2]->nets[d]];
							while(traverse_nets!=NULL)
							{
								cell_partition=traverse_nets->p;  
								if(cell_partition->partition==1)
								{
									set1++;
								}
								else if(cell_partition->partition==2)
								{
									set2++;
								}
								traverse_nets=traverse_nets->next;
							}
							if(set1==0 || set2==0)
							    cuts=0;
							else cuts=1;
							if(cuts==cut_net[index[choose_swap2]->nets[d]])
							{
								cutset=cutset;
								new_cutset=cutset;							      
							}
							else if(cuts==0 && cut_net[index[choose_swap2]->nets[d]]==1)
							{
								cutset=cutset-1;
								new_cutset=cutset;							    
							}
							else if(cuts==1 && cut_net[index[choose_swap2]->nets[d]]==0)
							{
								cutset=cutset+1;
								new_cutset=cutset;							    
							}
							cut_net[index[choose_swap2]->nets[d]]=cuts;
						}
						
						cout<<"New Cutset: "<<new_cuset<<endl;
						delta=initial_cutset-new_cutset;
						
						if(exp(delta/initial_temp)<1)
						      prob=exp(delta/initial_temp);
						else prob=1;
						
						if(0.01*((unsigned)rand()%100)<=prob)
						{
							
							initial_cutset=new_cutset;				      
							cntrl=index[choose_swap1]->z;
							for(d=0;d<cntrl;d++)
							{
								prevcut_net[index[choose_swap1]->nets[d]]=cut_net[index[choose_swap1]->nets[d]];
							}
							cntrl=index[choose_swap2]->z;
							for(d=0;d<cntrl;d++)
							{
								prevcut_net[index[choose_swap2]->nets[d]]=cut_net[index[choose_swap2]->nets[d]];
							}
						      
						}
						else
						{
							new_cutset=initial_cutset;
							cntrl=index[choose_swap1]->z;
							for(d=0;d<cntrl;d++)
							{
								cut_net[index[choose_swap1]->nets[d]]=prevcut_net[index[choose_swap1]->nets[d]];
							}
							cntrl=index[choose_swap2]->z;
							for(d=0;d<cntrl;d++)
							{
								cut_net[index[choose_swap2]->nets[d]]=prevcut_net[index[choose_swap2]->nets[d]];
							}
						  
							if(flag==0)
							{
								  area2=area2-(index[choose_swap1]->area)+(index[choose_swap2]->area);
								  area1=area1-(index[choose_swap2]->area)+(index[choose_swap1]->area);
								  index[choose_swap1]->partition=1;
								  index[choose_swap2]->partition=2;
							}
							else if(flag==1)
							{
								area2=area2-(index[choose_swap2]->area)+(index[choose_swap1]->area);
								area1=area1-(index[choose_swap1]->area)+(index[choose_swap2]->area);
								index[choose_swap1]->partition=2;
								index[choose_swap2]->partition=1;
							}
						}
						
						
						
					}
					else
					{
						new_cutset=initial_cutset;
						if(flag==0)
						{
							  area2=area2-(index[choose_swap1]->area)+(index[choose_swap2]->area);
							  area1=area1-(index[choose_swap2]->area)+(index[choose_swap1]->area);
							 
						}
						else if(flag==1)
						{
							area2=area2-(index[choose_swap2]->area)+(index[choose_swap1]->area);
							area1=area1-(index[choose_swap1]->area)+(index[choose_swap2]->area);
							
						}
					}
				}
				initial_temp=0.9*initial_temp;
				
			}
			#endif
			#ifdef BSD
			getrusage(RUSAGE_SELF,&time);
			//printf("Execution time is:[%f] seconds.\n",(double)(1.0*time.ru_utime.tv_sec+0.000001*time.ru_utime.tv_usec));
			output<<"Execution time: "<<(double)(1.0*time.ru_utime.tv_sec+0.000001*time.ru_utime.tv_usec)<<"seconds"<<endl;
			#endif
			#ifdef WIN
			ex_time= clock()-ex_time;
			printf ("Execution time is: [%f]seconds.\n",ex_time,((float)ex_time)/CLOCKS_PER_SEC);
			#endif
			
			perc_change=100*(start_cut-new_cutset)/start_cut;
			ratio_cut=100*area1/total_area;
			
			output<<"Starting cut: "<<start_cut<<endl;
			output<<"Final cut: "<<new_cutset<<endl;
			output<<"Percentage change: "<<perc_change<<endl;
			output<<"Ratio cut: "<<ratio_cut<<endl;
			
	net_file.close();
	are_file.close();
	output.close();
	return 0;
}
  
//**********************************************************************************************************************************************//  
//**********************************************************************************************************************************************//
