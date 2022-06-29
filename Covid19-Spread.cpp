#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <queue>
#include <cctype>
#include <random>

#define SUSCEPTIBLE "Susceptible"
#define INFECTED "Infected"
#define RECOVERED "Recovered"
#define N 100

using namespace std;

//BinaryHeapNode
struct Node
{
    int NodeId;
    int TimeStamp;
    int InfectedTime;
    string EventType;
    int MinDistance;

} Population[N];//This is the population

//This is the adjacent matrix of the graph
int Graph[N][N];
//Initialising sets
set<int> S, I, R;

random_device os_seed;
const uint_least32_t seed = os_seed();
mt19937 generator(seed);
void Initialise()
{
    uniform_int_distribution<uint_least32_t>distribute(0,1);
    for (int i = 0; i < N; i++)
    {
        for (int j = i + 1; j < N; j++)
        {
            int toss = distribute(generator);
            if (toss)
            {
                Graph[i][j] = 1;
            }
            else
            {
                Graph[i][j] = 0;
            }
            Graph[j][i] = Graph[i][j];
        }
        Population[i].NodeId = i;
        Population[i].EventType = SUSCEPTIBLE;
        Population[i].TimeStamp = 0;
        Population[i].InfectedTime=0;
        Population[i].MinDistance=0;
        S.insert(i);
    }
}

//BFS for the distances 
void BFS(int root)
{
    //Queue which traverses through the graph
    queue<int> Dist;
    Dist.push(root);//pushing the initial root
    //Loop till the queue is empty
    while(!Dist.empty())
    {
        //taking the first element from the queue and removing it from the queue
        int temp=Dist.front();
        Dist.pop();
        //adding all the neighbours of the first element of the queue to the queue 
        for(int i=0;i<N;i++)
        {
            if(Graph[temp][i]&&!Population[i].MinDistance&&i!=root)
            {
                //Changing the min distance of the neighbours
                Population[i].MinDistance=Population[temp].MinDistance+1;
                Dist.push(Population[i].NodeId);
            }
        }
    }
}


auto cmp = [](Node left, Node right){ return (left.TimeStamp) > (right.TimeStamp); };
priority_queue<Node, vector<Node>, decltype(cmp)> Q(cmp);
int main()
{

    Initialise();
    uniform_int_distribution<uint_least32_t>distribute(0,N-1);
    int random = distribute(generator);
    S.erase(random);
    Population[random].EventType = INFECTED;
    Q.push(Population[random]);
    while (!Q.empty())
    {
        Node e = Q.top();
        Q.pop();
        if (e.EventType == RECOVERED)
        {
            R.insert(e.NodeId);
            I.erase(e.NodeId);
        }
        else if (e.EventType == INFECTED)
        {
            I.insert(e.NodeId);
            S.erase(e.NodeId);
            for (int i = 0; i < N; i++)
            {
                //Checking whether the node is susceptible and if there is any edge is there between them
                if (Population[i].EventType == SUSCEPTIBLE && Graph[i][e.NodeId])
                {
                    int count = e.TimeStamp;
                    int NoOfToss = 0;
                    bool isHead=false;
                    uniform_int_distribution<uint_least32_t>distribute(0,1);
                    //5 coin tosses
                    for (int j = 0; j < 5; j++)
                    {
                        if (Population[i].EventType == SUSCEPTIBLE)
                        {
                            int toss = distribute(generator);
                            NoOfToss++;
                            if (toss)
                            {
                                isHead=true;
                                break;
                            }
                        }
                    }
                    if (isHead)
                    {
                        uniform_int_distribution<uint_least32_t>distribute(1,6);
                        //Infecting the population
                        Node tempNode;
                        Population[i].TimeStamp = e.TimeStamp + NoOfToss;
                        Population[i].InfectedTime=Population[i].TimeStamp;
                        Population[i].EventType = INFECTED;
                        Q.push(Population[i]);
                        //Recovering the population
                        Population[i].EventType = RECOVERED;
                        // tempNode = Population[i];
                        // srand((unsigned)time(0));
                        Population[i].TimeStamp = Population[i].TimeStamp +distribute(generator);
                        Q.push(tempNode);
                    }
                }
            }
        }
        // cout<<S.size() <<" "<< I.size()<<" "<<R.size()<<endl;
    }
    BFS(random);
    int max_time=0;
    //checking for the time taken for the virus to go completely
    for(int i=0;i<N;i++)
    {
        max_time=(Population[i].TimeStamp>max_time) ? Population[i].TimeStamp : max_time;
    }
    max_time++;
    int InfectedPeople[max_time],RecoveredPeople[max_time],SusceptiblePeople[max_time];
    //Initialising the number of people to 0 
    for(int i=0;i<max_time;i++)
    {
        InfectedPeople[i]=0;
        RecoveredPeople[i]=0;
        SusceptiblePeople[i]=0;
    }
    //counting the number of people infected and recovered in each time 
    for(int i=0;i<N;i++)
    {
        InfectedPeople[Population[i].InfectedTime]++;
        RecoveredPeople[Population[i].TimeStamp]++;
    }
    RecoveredPeople[0]--;
    //Finding cumulative recovered people and cumulative infected people
    for(int i=1;i<max_time;i++)
    {
        RecoveredPeople[i]=RecoveredPeople[i]+RecoveredPeople[i-1];
        InfectedPeople[i]=InfectedPeople[i]+InfectedPeople[i-1];
    }
    //finding the number of people infected in that time and finding the susceptible people in that time
    for(int i=0;i<max_time;i++)
    {
        InfectedPeople[i]=InfectedPeople[i]-RecoveredPeople[i];
        SusceptiblePeople[i]=N-InfectedPeople[i]-RecoveredPeople[i];
    }
    //Printing the number of people infected, recovered and susceptible in each time
    cout<<"Time \t \t Infected \t Recovered \t Succeptible "<<endl;
    for(int i=0;i<max_time;i++)
    {
        cout<<i<<" \t \t "<<InfectedPeople[i]<<" \t \t "<<RecoveredPeople[i]<<" \t \t "<<SusceptiblePeople[i]<<endl;
    }

    cout<<endl<<endl;
    //Printing the NodeId, InfectdTime, Mindistance from root 
    cout<<"NodeId \t \t Infected Time \t Shortest Distance"<<endl;
    for(int i=0;i<N;i++)
    {
        cout<<Population[i].NodeId<<" \t \t "<<Population[i].InfectedTime<<" \t \t "<<Population[i].MinDistance<<endl;
    }

    return 0;
}