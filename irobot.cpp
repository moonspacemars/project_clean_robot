#include<iostream>
#include<stack>
#include<fstream>
#include<vector>
#include<stdio.h>
#include<algorithm>
#include<exception>


using namespace std;
const int room_map_INIT=1;
const int BAT_SITE=2;
const int max_weight=2000000;
const int init=105;

enum direction {UP,DOWN,LEFT,RIGHT};

class step{
    public:
        int row;
        int column;
        int sequence;
    
};

class node{
    public:
        int vert;
        int dist;
};

struct offset{
    int horizontal,vertical;
};

class Graph{
private:
    int total_vertex;
    int total_edge;
    int bat,currPower=0,bat_node;
    vector<vector<int>> adjGroup;
    vector<int> Tdist,Tpred;

    bool *visited;
    void singlePathForTravel(int source);
   
public:
    vector< int > singleSourceDist,singlePred,allPath;
    Graph(int n,int b):total_vertex(n),bat(b){
        total_edge=0;
        adjGroup.resize(total_vertex);
        currPower=bat;
        Tdist.resize(total_vertex);
        Tpred.resize(total_vertex);
        //adjmatrix.resize(total_vertex);
        
        singlePred.resize(total_vertex);
     
        singleSourceDist.resize(total_vertex);
        for(int i=0;i< total_vertex; i++){
            //adjmatrix[i].resize(total_vertex);
         
            singleSourceDist[i]=max_weight;
            singlePred[i]=-1;
        }
        /*for (int i=0; i<total_vertex;i++)
				for(int j=0;j<total_vertex; j++)
				adjmatrix[i][j]=init;*/


        visited =  new bool[total_vertex];
    }
    void AddEdge(int start, int end, int weight);
 
    void SingleMinPath(int source);

    
    
    void Print1D(vector<int> a);

    void DFS(int v);

    void Travel();

    void gohome(int start);

    bool checkVisit();

    void TravelInit();

    bool TravelOrder(vector<int> sortNodes);
};

bool NodeCompare(node n1, node n2){
    return (n1.dist > n2.dist);
}

bool StepSeqCompare(step s1, step s2){
    return (s1.sequence < s2.sequence);
}

void Graph::singlePathForTravel(int source){
   
    Tdist[source]=0;
    bool leaveEarly=true;
    int newLen=0;
    int adjNode=0;
    for (int k=0;k<total_vertex; k++){
        leaveEarly=true;
        for(int i=0;i<total_vertex;i++){
            for (int j=0;j<adjGroup[i].size();j++){
                adjNode=adjGroup[i][j];
                newLen=Tdist[i]+1;//adjmatrix[i][adjNode];
                if (newLen<Tdist[adjNode]){
                    Tdist[adjNode]=newLen;
                    Tpred[adjNode]=i;
                    leaveEarly=false;
                }
            }
        }
        if (leaveEarly){
            break;
        }



    }
    
}

void Graph::Travel(){

    node tmpnode;
    vector<node> nodeGroup;
    for (int i=0;i<total_vertex;i++){
        tmpnode.vert=i;
        tmpnode.dist=singleSourceDist[i];
        nodeGroup.push_back(tmpnode);
    }

    vector<int> sortedNode;

    sort(nodeGroup.begin(), nodeGroup.end(), NodeCompare);

    for(int i=0; i<nodeGroup.size();i++){
        sortedNode.push_back(nodeGroup[i].vert);
    }    

    vector<int> tmpDist;
    tmpDist=sortedNode;
    
    bool finished=false;
    int orderidx=0;
    for(int i=0; i<total_vertex; i++){
        finished=TravelOrder(tmpDist);
        if (finished==false){
            orderidx++;
            int newidx=0;
            if (i==total_vertex-1)
                break;
            for(int k=0;k<total_vertex;k++){
                newidx=k-orderidx;
                if(newidx<0){
                    newidx=newidx+total_vertex;
                }
                tmpDist[k]=sortedNode[newidx];
            }
        }
        else
        {
           // cout<<"done !"<<endl;
            break;
        }
    }   

    //cout<<"all path:"<<endl;
   // Print1D(allPath);
}



bool Graph::TravelOrder(vector<int> sortNodes){
    TravelInit();
    stack<int> tempPath;
    int checknode;
    bool wrongpath;
    int j;
    int temp_i=0;
    for(int i=0; i<2*total_vertex;i++){
        if (i>=total_vertex)
            temp_i=i-total_vertex;
        else
            temp_i=i;

        j=sortNodes[temp_i];
            if (!visited[j] && j!=bat_node){
                stack<int>().swap(tempPath);
                tempPath.push(j);
                wrongpath=false;

                if (allPath.size()>1){
                     
                    int source=allPath[allPath.size()-2];

                    for(int ini_idx=0; ini_idx<total_vertex;ini_idx++){
                        Tpred[ini_idx]=-1;
                        Tdist[ini_idx]=max_weight;
                    }
                   
                   
                    singlePathForTravel(source);


                    if (Tdist[j]<=singleSourceDist[j]){
                        int pred3=Tpred[j];
                        if (pred3==-1)
                            tempPath.push(pred3);
                        else
                        while (pred3!=-1){                            
                            tempPath.push(pred3);
                            pred3=Tpred[pred3];
                        }    
                    }
                    else{
                        wrongpath=true;
                    }

                }
                else{              
                 
                    int pred_idx=singlePred[j];

                    int pred_2=0;
                   
                    while(singlePred[pred_idx]!=-1){

                        tempPath.push(pred_idx);

                        pred_2=singlePred[pred_idx];
                        checknode=singlePred[pred_2];
                        if (checknode==bat_node && allPath.size()>=2){  //go from R dir check
                            if (pred_2!=allPath[allPath.size()-2]){
                                wrongpath=true;
                            }
                    
                        }
                        else if(pred_2==bat_node && allPath.size()>=2){  //go from R dir check
                            if (pred_idx!=allPath[allPath.size()-2]){
                                wrongpath=true;                            
                            }                  
                        }
                        pred_idx=pred_2;
                    }
                    
                }


                if (wrongpath){
                   // cout<<"wrong path:"<<j<<endl;
                    stack<int>().swap(tempPath);
                    continue;
                }
                
                //cout<<tempPath.size()<<endl;

                int nextp;
                while(!tempPath.empty())
                {
                    currPower--;
                    nextp=tempPath.top();
                    visited[nextp]=true;                    
                    allPath.push_back(nextp);
                    tempPath.pop();

                }

                int start=j;
                //cout<<"start:"<<start<<endl;
                bool findway=true,arrivehome=false;
                int nexti=0;
                while(findway)
                {
                    findway=false;    
                    for (int ni=0;ni<adjGroup[start].size(); ni++){
                        nexti=adjGroup[start][ni];
                        if (!visited[nexti]){
                            int gohomepow=currPower-1;
                           // cout<<"gp:"<<gohomepow<<endl;
                            if (gohomepow >=singleSourceDist[nexti])
                            {
                               // cout<<"findway"<<endl;
                                start=nexti;
                                visited[nexti]=true;
                                currPower--;
                                findway=true;
                                allPath.push_back(nexti);


                                if (nexti==bat_node){
                                    visited[nexti]=false;
                                    currPower=bat;
                                    findway=false;
                                    arrivehome=true;

                                }
                                break;
                            }                       
                        }          
                    }                     
                }

                if (!arrivehome){                    
                    gohome(start);
                }
              
            }
       
    } 

    //cout<<"tmp path:"<<endl;
    //Print1D(allPath);    
    return checkVisit();   
}


void Graph::gohome(int start){
    int pred=singlePred[start];
    while(pred!=-1){
        visited[pred]=true;
        allPath.push_back(pred);
        pred=singlePred[pred];   


    }
    currPower=bat;
    visited[bat_node]=false;
    //cout<<"gohome"<<endl;
}



bool Graph::checkVisit(){
    bool allvisit=true;
    for (int i=0; i<total_vertex; i++){
        if (i!=bat_node && !visited[i])
        {
            allvisit=false;
            break;
        }

    }
    return allvisit;
}


void Graph::TravelInit(){
    fill(visited, visited+total_vertex,false);
    currPower=bat;
    vector<int>().swap(allPath);
}


void Graph::Print1D(vector<int> a){
    for(int i=0;i<a.size();i++)
        cout<<a[i]<<" ";
    cout<<endl;
}

void Graph::AddEdge(int start, int end, int weight){
    total_edge++;
	//adjmatrix[start][end]=weight;		
	//adjmatrix[end][start]=weight;

    adjGroup[start].push_back(end);
} 

void Graph::SingleMinPath(int source){
    bat_node=source;
    singleSourceDist[source]=0;
    int newLen=0;
    int adjNode=0;
    bool leaveEarly=true;
    for (int k=0;k<total_vertex; k++){
        leaveEarly=true;
        for(int i=0;i<total_vertex;i++){
            for (int j=0;j<adjGroup[i].size();j++){
                adjNode=adjGroup[i][j];
                newLen=singleSourceDist[i]+1;/*adjmatrix[i][adjNode]*/          
                if (newLen<singleSourceDist[adjNode]){
                    singleSourceDist[adjNode]=newLen;
                    singlePred[adjNode]=i;
                    leaveEarly=false;
                }
            }
        }
        if (leaveEarly){
            break;
        }
    }
    //Print1D(singleSourceDist);
    //Print1D(singlePred);    
}












int main(int argc, char *argv[]){
    int rowCount=0, columnCount=0, battery=0,matrixRowCount=0,matrixColumnCount=0;
try{

offset move[4];
move[UP].horizontal=0;
move[UP].vertical=-1;
move[DOWN].horizontal=0;
move[DOWN].vertical=1;
move[LEFT].horizontal=-1;
move[LEFT].vertical=0;
move[RIGHT].horizontal=1;
move[RIGHT].vertical=0;



    string filePath;
    filePath.assign(argv[1]);
    filePath=filePath+"/floor.data";
  
 

    ifstream inputFile(filePath);
    if(!inputFile){
        cout<<"can not open file.\n";
    }

    inputFile>>rowCount;
    inputFile>>columnCount;
    inputFile>>battery;

    matrixRowCount=rowCount+2;
    matrixColumnCount=columnCount+2;

   
    vector< vector<int> > nodeNumberTable;
    vector< vector<int> > room_map;

    //init room_map
    room_map.resize(matrixRowCount);
    nodeNumberTable.resize(matrixRowCount);

    for (int i=0; i<matrixRowCount;i++){
        room_map[i].resize(matrixColumnCount);
        nodeNumberTable[i].resize(matrixColumnCount);
 
        for (int j=0; j<matrixColumnCount; j++){
            room_map[i][j]=room_map_INIT;
            nodeNumberTable[i][j]=-1;
        }        
    }

    step start;
    int openPathCount=0;

    int temp=0;
    bool findbattery=false;
    char buf;
    char key= 'R';
    for (int i=1; i<matrixRowCount-1; i++){
        for (int j=1; j<matrixColumnCount-1 ; j++){
            inputFile>>buf;
            if (buf==key){
                room_map[i][j]=BAT_SITE;
                start.row=i;
                start.column=j;                
            }
            else{
                room_map[i][j]=buf-'0';
            }
             


            if (room_map[i][j]==0){
                openPathCount++;
            }



        }
    }

    inputFile.close();
 

    int totalNode;
    totalNode=openPathCount+1;


    int curr_row=0,curr_col=0, neighbor_row=0,neighbor_col=0;
    Graph stepGraph(totalNode,battery);

    //cout<<totalNode<<endl;

    //create node number to (row, column) mapping    
    int nodeIdx=0,d=0;
    step tempstep;
    vector<step> nodeToArray ;
    for(int i=1; i< matrixRowCount-1;i++){
        for (int j=1; j<matrixColumnCount-1;j++){
            if (room_map[i][j]==0 || room_map[i][j]==BAT_SITE){
                    tempstep.row=i-1;
                    tempstep.column=j-1;
                    tempstep.sequence=nodeIdx;
                    nodeToArray.push_back(tempstep);
                    nodeNumberTable[i][j]=nodeIdx;
                    nodeIdx++;
                
            }
        }            
    }

    sort(nodeToArray.begin(), nodeToArray.end(), StepSeqCompare);



    for(int i=1; i< matrixRowCount-1;i++){
        for (int j=1; j<matrixColumnCount-1;j++){
            if (room_map[i][j]==0 || room_map[i][j]==BAT_SITE){

                d=0;
                while(d<4){ 
                    neighbor_row=i+move[d].vertical;
                    neighbor_col=j+move[d].horizontal;                    

                    if (room_map[neighbor_row][neighbor_col]==0 || room_map[neighbor_row][neighbor_col]==BAT_SITE){
                         stepGraph.AddEdge(nodeNumberTable[i][j], nodeNumberTable[neighbor_row][neighbor_col],1);
                    }
                    d++;
                }
            }
        }
    }
    //assert(totalNode== nodeIdx) ;
    //cout<<"node idx:"<<nodeIdx<<endl;

    
    int bat_idx;
    bat_idx=nodeNumberTable[start.row][start.column];
    stepGraph.SingleMinPath(bat_idx);




    //cout<<"finish singlePath"<<endl;


    stepGraph.Travel();


    //cout<<"f t"<<endl;


/*
    //print 
    for (int i=0; i<matrixRowCount;i++){
        
        for (int j=0; j<matrixColumnCount; j++){
            cout<<room_map[i][j]<<" ";
        }        
        cout<<endl;
    }

    cout<<"node map:"<<endl;
    for (int i=0; i<matrixRowCount;i++){
        
        for (int j=0; j<matrixColumnCount; j++){
            cout<<nodeNumberTable[i][j]<<" ";
        }        
        cout<<endl;
    }    

*/



    

    


    
    //output
    

    vector<int> finalPath;
    finalPath=stepGraph.allPath;
    string outputFile;
    outputFile.assign(argv[1]);
    outputFile=outputFile+"/final.path";
    ofstream answerOutput (outputFile);
    int stepSquence;
    if (answerOutput.is_open()){        
        answerOutput<<finalPath.size()<<"\n";
        for(int i=0; i<finalPath.size();i++)
        {
            stepSquence=finalPath[i];
            answerOutput<<nodeToArray[stepSquence].row<<" "<<nodeToArray[stepSquence].column<<"\n";
        }
    }
    else cout<<"unable to open file!";


}catch(...){
    cout<<"error"<<endl;

}


    return 0;
}