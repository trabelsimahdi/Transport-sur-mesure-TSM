#include <cmath>
#include <fstream>
#include <iostream>
#include <vector>
#include <queue>
#include <climits>
#include <cstring>
#include <string>
#define PI 3.14159265358979224
#include <time.h>


using namespace std;
typedef pair<int,double> Pair;


struct Node {
    int vertex;
    double weight;
};




class Station
{
    public:
        double Longi, Lati;
        string nom,nl,type;
        Station(double Lo=0, double La=0, string n="", string l="", string t="")
        {
            Longi=Lo;
            Lati=La;
            nom=n;
            nl=l;
            type = t;
        }

        double get_Longi ()const
        {
            return Longi;
        }
        double get_Lati ()const
        {
            return Lati;
        }
        string get_nl ()const
        {
            return nl;
        }
        string get_type ()const
        {
            return type;
        }
        string get_nom() const
        {
            return nom;
        }
};




class Arc
{
    public:
        int id_depart, id_arrivee;
        double dist;
        Arc(int D, int A,double d=0)
        {
            id_depart=D;
            id_arrivee=A;
            dist=d;
        }
};





class Graph
{
public:
    vector<vector<Pair>> adjList;
    Graph(vector<Arc> const &arcs, int N)
    {
        adjList.resize(N);
        for (auto &arc: arcs)
        {
            adjList[arc.id_depart].push_back(make_pair(arc.id_arrivee,arc.dist));
            adjList[arc.id_arrivee].push_back(make_pair(arc.id_depart,arc.dist));
        }
    }
};





double distance_gps (Station A, Station B)
{
    double latitudeA=A.get_Lati();
    double latitudeB=B.get_Lati();
    double longitudeA=A.get_Longi();
    double longitudeB=B.get_Longi();
    return ((acos(cos(latitudeA*PI/180)*cos(latitudeB*PI/180)*cos((longitudeB*PI/180)-(longitudeA*PI/180))+sin(latitudeA*PI/180)*sin(latitudeB*PI/180)))*6371);
}

string afficher(double x)
{

    int h=(int)(x/60);
    int m= (int)(x-h*60);
    return to_string(h)+" heures, "+to_string(m)+" minutes.";
}


void print_route(vector<int> const &prev,int source, int i,vector <Station> LStation)
{
    if (i < 0) {
        return;
    }
    print_route(prev,source, prev[i],LStation);
    if (i == source)
        cout << "Prenez le  " << LStation[i].type << " " << LStation[i].nl << " Station ( " << LStation[i].nom << " ) " ;
    else if (LStation[i].type != LStation[prev[i]].type || LStation[i].nl != LStation[prev[i]].nl){
        cout << "puis descendez a la station "<< LStation[prev[i]].nom <<endl;
        cout << "En suite prenez le " << LStation[i].type << " " << LStation[i].nl << " Station " << LStation[i].nom<<" "  ;
    }

}



struct comp
{
    bool operator()(const Node &lhs, const Node &rhs) const {
        return lhs.weight > rhs.weight;
    }
};


void PCC(Graph const &graph, int source,int i, int N,vector <Station> LStation)
{
    // creer un min-heap et pousser le noeud source ayant distance 0
    priority_queue<Node, vector<Node>, comp> min_heap;
    min_heap.push({source, 0});

    // la distance d'une source a v est infinie
    vector<double> dist(N, INT_MAX);

    // la distance d'une source à elle m est zéro
    dist[source] = 0;

    // boolean array to track vertices for which minimum
    // le cout est deja là
    vector<bool> done(N, false);
    done[source] = true;

    // stores predecessor of a vertex (to a print path)
    vector<int> prev(N, -1);

    // run till min-heap is empty
    while (!min_heap.empty())
    {
        // effacer et retourner le meilleur vertex
        Node node = min_heap.top();
        min_heap.pop();

        // avoir le numero du vertex
        int u = node.vertex;

        // faire pour chaque voisin v de u
        for (auto i: graph.adjList[u])
        {
            int v = i.first;
            double weight = i.second;

            // Relaxation
            if (!done[v] && (dist[u] + weight) < dist[v])
            {
                dist[v] = dist[u] + weight;
                prev[v] = u;
                min_heap.push({v, dist[v]});
            }
        }

        // marquer vertex u pour qu'elle ne soit pas choisie une autre fois
        done[u] = true;
    }

        if (i != source && dist[i] != INT_MAX)
        {
            cout << "PCC de (" << LStation[source].nom  <<"-->"<< LStation[i].nom << "): "<<endl;
            cout << "Duree du parcours = "<< afficher(dist[i]*60) <<endl;
            print_route(prev,source,i,LStation);
            cout << "et descendez a la station ("<< LStation[i].nom <<")"<<  endl;
        }
    }




void printGraph(Graph const &graph, int N,vector <Station> LStation)
{
    for (int i = 0; i < N; i++)
    {
        // Function to print all neighboring vertices of a given vertex
        for (Pair v: graph.adjList[i])
        {
            cout << "(" << LStation[i].nom << ", " << LStation[v.first].nom <<
                    ", " << v.second << ") ";
        }
        cout << endl;
    }
}



int trouver(string s,vector <Station> LStation)
{
    for (int i=0;i<4275;i++)
    {
        if (LStation[i].get_nom()==s)
        {
            return i;
        }
    }
    return -1;
}


int main()
{






    /* heure de pointe ou pas */
    int rh;
    time_t seconds = time(NULL);
    struct tm * timeinfo = localtime(&seconds);
    int h=timeinfo->tm_hour+1;
    if (((h<=14) && (h>=11))||((h>=17)&&(h<=19)))
    {
        rh=1;
    }
    else
    {
        rh=0;
    }
    double m,b;
    if (rh==1)
    {
        m=30;
        b=15;
    }
    else
    {
        m=30;
        b=30;
    }





    /*récupération des données à partir du fichier excel de métro */
    ifstream f;
    vector <Station> LStation ;
    f.open("busmetro.csv");
    while (f.good())
    {
        string ligne,nom,longitude,lattitude,type;
        double lo,la;
        getline(f,ligne,';');
        getline(f,nom,';');
        getline(f,longitude,';');
        char lga[longitude.length()+1] ;
        strcpy(lga,longitude.c_str());
        lo=atof(lga);
        getline(f,lattitude,';');
        char laf[lattitude.length()+1] ;
        strcpy(laf,lattitude.c_str());
        la=atof(laf);
        getline(f,type);
        Station s(lo,la,nom,ligne,type);
        LStation.push_back(s);
    }
    f.close();










    /* Création de la liste d'adjacence */
    vector <Arc> ListeAdj;



    /*1ere etape */
    int i=0;
    int j;
    while (i<4275)
    {
        j=i+1;
        if (LStation[i].get_nl()==LStation[j].get_nl())
        {
            if (LStation[i].get_type()=="Metro ")
            {
                ListeAdj.push_back(Arc(i,j,distance_gps(LStation[i],LStation[j])/m));
                i=i+1;
            }
            else
            {
                ListeAdj.push_back(Arc(i,j,distance_gps(LStation[i],LStation[j])/b));
                i=i+1;
            }
        }
        else
        {
            i=i+1;
        }
    }



    /* 2eme etape */
    for (int i=0;i<4275;i++)
    {
        for (int j=0;j<4275 & j!=i;j++)
        {
            if (distance_gps(LStation[i],LStation[j])<0.5)
            {
                    ListeAdj.push_back(Arc(i,j,distance_gps(LStation[i],LStation[j])/5+0.1667));
            }
        }
    }









    /*manipulation des données */
    string source;
    string destination;
    int s=-1;
    int d=-1;
    while (s<0)
    {
        cout<< "donner votre source"<<endl;
        getline(cin,source);
        s=trouver(source,LStation);
    }
    while ( d<0)
    {
        cout << "donner votre destination"<<endl;
        getline(cin,destination);
        d=trouver(destination,LStation);
    }


    cout <<"Il est "<< timeinfo->tm_hour  << ":" <<timeinfo->tm_min<<endl;
    if (((h<=14) && (h>=11))||((h>=17)&&(h<=19)))
    {
        cout<<"C'est une heure de pointe"<<endl;
    }
    else
    {
        cout<<"Ce n'est pas une heure de pointe"<<endl;
    }




    /*création du graphe */
    Graph G(ListeAdj,4275);








    /* execution */
    PCC(G,s,d,4275,LStation);
    return 0;
}
