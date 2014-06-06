#ifndef EGRAPH_H
#define EGRAPH_H

#include<ros/ros.h>
#include<vector>
#include<string>
#include<boost/thread.hpp>
#include<sbpl/headers.h>

using namespace std;

class EGraph{
  public:

    class EGraphVertex : public AbstractSearchState{
      public:
        EGraphVertex(){
          id = -1;
          shortcutIteration = 0;
          component = -1;
          search_iteration = 0;
        };

        int id;
        vector<int> coord;
        vector<double> constants;
        //an adjacency list representing the graph (using the egraph ids)
        vector<EGraphVertex*> neighbors;
        vector<int> costs;
        vector<bool> valid;
        vector<int> use_frequency;
        int component;

        vector<EGraphVertex*> shortcuts;
        vector<int> shortcut_costs;
        int shortcutIteration;

        int search_iteration;
        int search_cost;
    };

    class EGraphVertexHeapElement: public AbstractSearchState{
      public:
        EGraphVertexHeapElement(){};
        ~EGraphVertexHeapElement(){};
          
        int id;
    };

    //constructor takes 4 vectors (min,max,res,names) which tells me the number of dimensions, how many values they can have, and the dimension names
    //load can be called after this in order bring up a stored E-Graph with different parameters than those stored in the file
    EGraph(vector<double>& min, vector<double>& max, vector<double>& resolution, vector<string>& names, int num_constants);

    //another constructor takes an egraph file to load
    //this will load the E-Graph using the parameters (min,max,resolution,names) stored in the file
    EGraph(string filename);

    EGraph(string filename, string stats_filename);

    ~EGraph();

    void addDimension(double min, double max, double res, string name, double initial_val);

    void clearEGraph();

    //add path takes a vector of names, and a vector of vectors of doubles (the waypoints on the path), a vector of costs
    //this add the edges to the e-graph. 
    //no longer computes all-pairs! this simplifies the e-graph data structure and drops computation between queries to almost nothing. 
    //no longer needs to compute components!
    //finally this will call setEGraph on the EGraphable's EGraphHeuristic to prepare it for the next query
    bool addPath(vector<vector<double> >& coords, vector<int>& costs);

    int getNumComponents(){return num_components_;};
    void computeComponents();

    void print();

    //save egraph
    bool save(string filename);

    //load egraph
    bool load(string filename, bool clearCurrentEGraph=true);

    bool saveStats(string filename);
    bool loadStats(string filename);

    void recordStats(vector<vector<double> >& coords);
    void resetStats();
    void prune(int max_size, int method);
    void setClusterRadius(double r){cluster_radius_ = r;};

    void updateEdge(EGraphVertex* v1, EGraphVertex* v2, bool valid, int cost);
    void invalidateVertex(EGraphVertex* v1);

    void discToCont(EGraphVertex* v, vector<double>& c);
    void contToDisc(vector<double> c, vector<int>& d);

    int getShortestPath(EGraphVertex* v1, EGraphVertex* v2, vector<EGraphVertex*>* path=NULL, vector<int>* costs=NULL);

    //an id to coordinate mapping
    vector<EGraphVertex*> id2vertex;
    EGraphVertex* getVertex(vector<int>& coord);
    int search_iteration_;
  protected:

    unsigned int inthash(unsigned int key);
    int getHashBin(vector<int>& coord);

    EGraphVertex* createVertex(vector<int>& coord, vector<double>& constants);
    void addEdge(EGraphVertex* v1, EGraphVertex* v2, int cost);

    //a coordinate to id mapping
    //map<vector<double>,int> vertex2id;
    vector<vector<EGraphVertex*> > hashtable;

    vector<double> min_;
    vector<double> max_;
    vector<double> res_;
    vector<string> names_;
    int num_constants_;
    int num_edges_;
    int num_components_;
    double cluster_radius_;

    boost::recursive_mutex egraph_mutex_;
};

#endif
