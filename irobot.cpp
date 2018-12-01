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
const int limit_node_count=1000;

/**
 * Element stored within the Binary Heap.
 */
typedef struct elt {

  /** user-defined information to be stored by id */
  int     id;            

  /** Key which represents the priority. */
  int     priority;
} ELEMENT, *ELEMENT_PTR;


class BinaryHeap {

 public:
  BinaryHeap (int);
  ~BinaryHeap ();

  bool isEmpty() { return (_n == 0); }
  int smallest();
  void insert (int, int); 
  void decreaseKey (int, int);

 private:
  int          _n;           // number of elements in binary heap
  ELEMENT_PTR  _elements;    // values in the heap
  int          *_pos;        // pos[i] is index into elements for ith value    

  long         _numComparisons;
  long         _numInsert;
  long         _numSwaps;
  long         _numSmallest;
  long         _numDecrease;

};

/** allocate heap of n elements */
BinaryHeap::BinaryHeap (int i) {
  _n = 0;  // initially none in the heap

  // simplify algorithm to consider position 1 as being the 'root'
  _elements = (ELEMENT *) calloc (i+1, sizeof (ELEMENT));
  _pos      = (int *) calloc (i+1, sizeof (int));
}

/** Destructor frees all internal storage. */
BinaryHeap::~BinaryHeap () {
  free (_elements);
  free (_pos);
  _n = -1;
}


/**
 * Return the vertex identifier of the smallest vertex in heap and 
 * readjust the heap.
 */
int BinaryHeap::smallest () {
  int id = _elements[1].id;
  int pIdx;

  //INC_SMALL;

  // heap will have one less entry, and we want to place leftover one
  // in proper location.
  ELEMENT last = _elements[_n];
  _n--;

  _elements[1] = last;

  pIdx = 1;
  int child = pIdx*2;
  while (child <= _n) {
    // select smaller of two children
    ELEMENT sm = _elements[child];
    if (child < _n) {
      //INC_COMP;
      if (sm.priority >  _elements[child+1].priority) {
	sm = _elements[++child];
      }
    }

    // are we in the right spot? Leave now
    //INC_COMP;
    if (last.priority <= sm.priority) { break; }

    // otherwise, swap and move up
    //INC_SWAP;
    _elements[pIdx] = sm;
    _pos[sm.id] = pIdx;

    pIdx = child;
    child = 2*pIdx;
  }

  // insert into spot vacated by moved element (or last one)
  _elements[pIdx] = last;
  _pos[last.id] = pIdx;
  return id;
}


/**
 * Insert the given value into the tree with priority. Ties are broken
 * in favor of insert.
 * \param id        id of information to be stored
 * \param priority  priority to associate with this id.
 */
void BinaryHeap::insert (int id, int priority) {
  int i;

  //INC_INSERT;

  // add to end of the heap. If 1 then the first element.
  i = ++_n;
  while (i > 1) {
    int       pIdx = i/2;
    ELEMENT   p    = _elements[pIdx];

    // are we in the right spot? Leave now
    //INC_COMP;
    if (priority > p.priority) { break; }

    // otherwise, swap and move up
    //INC_SWAP;
    _elements[i] = p;
    _pos[p.id] = i;
    i = pIdx;
  }

  // insert into spot vacated by moved element (or last one)
  _elements[i].id = id;
  _elements[i].priority = priority;
  _pos[id] = i;
}


/**
 * Find the vertex with identifier [id] and reduce its priority to the
 * given value. It is the responsibility of the caller to ensure that
 * this function is only invoked when newPriority is indeed smaller than
 * the existing priority associated with the id.
 * \param  id            information to have priority reduced.
 * \param  newPriority   priority which must be smaller than existing priority.
 */
void BinaryHeap::decreaseKey (int id, int newPriority) {
  int size = _n;

  //INC_DECREASE;

  // truncate heap (temporarily) and act like the binary heap up to
  // but not including this one is all that exists (cute, huh?) 
  _n = _pos[id] - 1;

  // now we insert and the binary heap is shuffled appropriately
  insert(id, newPriority);

  // since the newPriority must be lower, we can expand back and 
  // we still have a working binary heap
  _n = size;
}




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
    int trymode=0;
    //int total_edge;
    int bat,currPower=0,bat_node, up_node=-1,down_node=-1,right_node=-1,left_node=-1;
    vector<vector<int> > adjGroup;


    bool *visited;
    void singlePathForTravel(int source, vector<int> &Adist, vector<int> &APred);
   
public:
    vector<int> Udist,Upred, Rdist, Rpred, Ldist, Lpred, Ddist, Dpred;
    vector< int > singleSourceDist,singlePred,allPath;
    Graph(int n,int b):total_vertex(n),bat(b){
        //total_edge=0;
        adjGroup.resize(total_vertex);
        currPower=bat;
        Udist.resize(total_vertex);
        Upred.resize(total_vertex);
        Ddist.resize(total_vertex);
        Dpred.resize(total_vertex);   
        Rdist.resize(total_vertex);
        Rpred.resize(total_vertex);
        Ldist.resize(total_vertex);
        Lpred.resize(total_vertex);               
        //adjmatrix.resize(total_vertex);
        
        singlePred.resize(total_vertex);
     
        singleSourceDist.resize(total_vertex);
        for(int i=0;i< total_vertex; i++){
            //adjmatrix[i].resize(total_vertex);
         
            singleSourceDist[i]=max_weight;
            singlePred[i]=-1;

            Udist[i]=max_weight;
            Upred[i]=-1;
            Ddist[i]=max_weight;
            Dpred[i]=-1;   
            Rdist[i]=max_weight;
            Rpred[i]=-1;
            Ldist[i]=max_weight;
            Lpred[i]=-1;


        }
        /*for (int i=0; i<total_vertex;i++)
				for(int j=0;j<total_vertex; j++)
				adjmatrix[i][j]=init;*/


        visited =  new bool[total_vertex];
    }
    void AddEdge(int start, int end, int weight);
 
    void SingleMinPath(int source, int Ubat, int Dbat, int Lbat, int Rbat);

    
    
    void Print1D(vector<int> &a);

    void DFS(int v);

    void Travel();

    void gohome(int start);

    bool checkVisit();

    void TravelInit();

    bool TravelOrder(vector<int> &sortNodes);

    void InitBatAroundNode();
};

bool NodeCompare(node n1, node n2){
    return (n1.dist >= n2.dist);
}

bool NodeCompareLess(node n1, node n2){
    return (n1.dist <= n2.dist);
}

bool StepSeqCompare(step s1, step s2){
    return (s1.sequence < s2.sequence);
}

void Graph::singlePathForTravel(int source, vector<int> &Adist, vector<int> &APred){
    Adist[source]=0;
    BinaryHeap pq(total_vertex);

    for (int u=0; u< total_vertex; u++){
        pq.insert(u, Adist[u]);
    }
    int newLen=0;
    int adjNode=0;   
    int smallNode=0;
    while(!pq.isEmpty()){
        smallNode=pq.smallest();
       /* if (total_vertex>limit_node_count){
            if (smallNode==bat_node){
                continue;
            }
        }*/
        if (trymode==1 || trymode==3){
            if (smallNode==bat_node){
                continue;
            }
        }

        for (int j=0;j<adjGroup[smallNode].size();j++){
            adjNode=adjGroup[smallNode][j];
            newLen=Adist[smallNode]+1;//adjmatrix[i][adjNode];
            if (newLen<Adist[adjNode]){
                pq.decreaseKey(adjNode, newLen);
                Adist[adjNode]=newLen;
                APred[adjNode]=smallNode;                
            }
        }        
    }    
}

void Graph::Travel(){

    node tmpnode;
    vector<node> nodeGroup;
    vector<int> sortedNode;

    for (int trysort=0; trysort<2; trysort++){

        vector<node>().swap(nodeGroup);
        for (int i=0;i<total_vertex;i++){
            tmpnode.vert=i;
            tmpnode.dist=singleSourceDist[i];
            nodeGroup.emplace_back(tmpnode);
        }

        vector<int>().swap(sortedNode);

    
        if (total_vertex>limit_node_count){
            sort(nodeGroup.begin(), nodeGroup.end(), NodeCompareLess); 
            trymode=1;   

        }
        else{
            if (trysort==0)
            {
                sort(nodeGroup.begin(), nodeGroup.end(), NodeCompare);
                trymode=2;
            }
            else{
                sort(nodeGroup.begin(), nodeGroup.end(), NodeCompare);
                trymode=3;               

            }


        }
        InitBatAroundNode();
        if (up_node!=-1)
            singlePathForTravel(up_node,Udist, Upred);
        if (right_node!=-1)
            singlePathForTravel(right_node,Rdist, Rpred);
        if (down_node!=-1)
            singlePathForTravel(down_node,Ddist, Dpred);
        if (left_node!=-1)    
            singlePathForTravel(left_node,Ldist, Lpred);



    /*
        if (total_vertex<limit_node_count && trysort==0)
            sort(nodeGroup.begin(), nodeGroup.end(), NodeCompare);
        else        
            sort(nodeGroup.begin(), nodeGroup.end(), NodeCompareLess);*/

        
        
        
        for(int i=0; i<nodeGroup.size();i++){
            sortedNode.emplace_back(nodeGroup[i].vert);
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
                //cout<<"done !"<<endl;
                //cout<<"all path:"<<endl;
                //Print1D(allPath);                
                return;
            }
        }   
    }

}



bool Graph::TravelOrder(vector<int> &sortNodes){
    TravelInit();
    stack<int> tempPath;

    vector<int> Tdist, Tpred;
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
                    if (source==up_node){
                        Tpred=Upred;
                        Tdist=Udist;
                    }
                    else if(source==down_node){
                        Tpred=Dpred;
                        Tdist=Ddist;                       
                    }
                    else if(source==left_node){
                        Tpred=Lpred;
                        Tdist=Ldist;
                    }
                    else if(source==right_node){
                        Tpred=Rpred;
                        Tdist=Rdist;
                    }
                    else{
                        wrongpath=true;
                        continue;

                    }
                    //cout<<"1.befor init "<<j<<endl;
                  /*  for(int ini_idx=0; ini_idx<total_vertex;ini_idx++){
                        Tpred[ini_idx]=-1;
                        Tdist[ini_idx]=max_weight;
                    }*/
                    
                   
                   //cout<<"2.after init"<<endl;
                    //singlePathForTravel(source);
                    //cout<<"3.after single path"<<endl;

                    //if (Tdist[j]<=singleSourceDist[j]){

                    bool pathCheck=false;

                    if (trymode==1 || trymode==3)
                        pathCheck=(Tdist[j]+singleSourceDist[j]+1<=bat);   
                    else
                        pathCheck=(Tdist[j]<=singleSourceDist[j]);
                    /*if (total_vertex>limit_node_count){
                        pathCheck=(Tdist[j]+singleSourceDist[j]+1<=bat);
                    }
                    else{
                        pathCheck=(Tdist[j]<=singleSourceDist[j]);
                    }
*/
                    if(pathCheck){
                        int pred3=Tpred[j];
                        if (pred3==-1)
                            tempPath.push(pred3);
                        else
                        while (pred3!=-1){                            
                            tempPath.push(pred3);
                            /*
                            if (pred3==bat_node){
                                wrongpath=true;
                                break;
                            }*/
                            pred3=Tpred[pred3];
                            
                        }    
                    }
                    else{
                        wrongpath=true;
                    }

                }
                else
                {              
                 
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
                    //cout<<"wrong path:"<<j<<endl;
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
                    allPath.emplace_back(nextp);
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
                            //cout<<"gp:"<<gohomepow<<endl;
                            if (gohomepow >=singleSourceDist[nexti] && nexti!=bat_node)
                            {
                               // cout<<"findway"<<endl;
                                start=nexti;
                                visited[nexti]=true;
                                currPower--;
                                findway=true;
                                allPath.emplace_back(nexti);


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
            //cout<<"tmp path:"<<endl;
             //Print1D(allPath); 
       
    } 

    //cout<<"tmp path:"<<endl;
    //Print1D(allPath);    
    return checkVisit();   
}

void Graph::InitBatAroundNode(){
        for(int i=0;i< total_vertex; i++){
         

            Udist[i]=max_weight;
            Upred[i]=-1;
            Ddist[i]=max_weight;
            Dpred[i]=-1;   
            Rdist[i]=max_weight;
            Rpred[i]=-1;
            Ldist[i]=max_weight;
            Lpred[i]=-1;


        }
}


void Graph::gohome(int start){
    int pred=singlePred[start];
    while(pred!=-1){
        visited[pred]=true;
        allPath.emplace_back(pred);
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


void Graph::Print1D(vector<int> &a){
    for(int i=0;i<a.size();i++)
        cout<<a[i]<<" ";
    cout<<endl;
}

void Graph::AddEdge(int start, int end, int weight){
    //total_edge++;
	//adjmatrix[start][end]=weight;		
	//adjmatrix[end][start]=weight;

    adjGroup[start].emplace_back(end);
} 

void Graph::SingleMinPath(int source,int Ubat, int Dbat, int Lbat, int Rbat){
    bat_node=source;
    singleSourceDist[source]=0;

    BinaryHeap pq(total_vertex);
    for(int u=0 ;u<total_vertex; u++){   
        pq.insert(u, singleSourceDist[u]);
    } 

    int newLen=0;
    int adjNode=0;
    int smallestNode=0;
    while(!pq.isEmpty()){
        smallestNode = pq.smallest();
        for (int j=0;j<adjGroup[smallestNode].size();j++){
            adjNode=adjGroup[smallestNode][j];
            newLen=singleSourceDist[smallestNode]+1;/*adjmatrix[i][adjNode]*/          
            if (newLen<singleSourceDist[adjNode]){
                pq.decreaseKey(adjNode, newLen);
                singleSourceDist[adjNode]=newLen;
                singlePred[adjNode]=smallestNode;
                
            }
        }

    }


    if (Ubat!=-1){
        //singlePathForTravel(Ubat,Udist, Upred);        
        up_node=Ubat;
    }


    if (Dbat!=-1){
        //singlePathForTravel(Dbat,Ddist, Dpred);
        down_node=Dbat;
    }
    if (Rbat!=-1){
       // singlePathForTravel(Rbat,Rdist, Rpred);
        right_node=Rbat;
    }


    if (Lbat!=-1){
        //singlePathForTravel(Lbat,Ldist, Lpred);            
        left_node=Lbat;
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
                    nodeToArray.emplace_back(tempstep);
                    nodeNumberTable[i][j]=nodeIdx;
                    nodeIdx++;
                
            }
        }            
    }

    //sort(nodeToArray.begin(), nodeToArray.end(), StepSeqCompare);



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

    
    int bat_idx,batUp,batDown,batRight,batleft;
    bat_idx=nodeNumberTable[start.row][start.column];
    batUp=nodeNumberTable[start.row-1][start.column];
    batDown=nodeNumberTable[start.row+1][start.column];
    batRight=nodeNumberTable[start.row][start.column+1];
    batleft=nodeNumberTable[start.row][start.column-1];
    stepGraph.SingleMinPath(bat_idx, batUp, batDown, batleft, batRight);


/*   string OF;
    OF.assign(argv[1]);
    OF=OF+"/nodeNumber.map";
    ofstream AO (OF);

    if (AO.is_open()){        
        for (int i=0; i<matrixRowCount;i++){
            
            for (int j=0; j<matrixColumnCount; j++){
                AO<<nodeNumberTable[i][j]<<" ";
            }        
            AO<<endl;
        }
    }
    else cout<<"unable to open file!";
*/




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
/*    cout<<"R dist:"<<endl;
    for (int i=0; i<stepGraph.Dpred.size();i++){

            cout<<stepGraph.Dpred[i]<<" ";
      
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