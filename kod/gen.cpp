#include <iostream>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/properties.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/ref.hpp>
#include <vector>
#include <stdlib.h> 
#include <time.h> 
#include <boost/graph/make_connected.hpp>
#include <boost/graph/boyer_myrvold_planar_test.hpp>
#include <boost/graph/make_biconnected_planar.hpp>
#include <boost/graph/make_maximal_planar.hpp>

using namespace boost;

#define SIZE 20
#define NUM_OF_GRAPHS 1
std::pair<int, int> choose_pair(){
  int u = 0, v = 0;
  while(u == v){
    u = rand() % SIZE;
    v = rand() % SIZE;
  }
  return std::make_pair(u, v);
}

int main(int argc, char** argv)
{
  srand(time(NULL)); 
  //std::cout<<NUM_OF_GRAPHS<<std::endl;
  for(int i = 0; i < NUM_OF_GRAPHS; i++){
    typedef adjacency_list
      < vecS,
        vecS,
        undirectedS,
        property<vertex_index_t, int>,
        property<edge_index_t, int>
      > 
      graph;
    typedef std::vector< graph_traits<graph>::edge_descriptor > vec_t;
    std::vector<vec_t> embedding(SIZE);
    graph g(SIZE);
    int counter = 0;
    int num_of_iter = 3*SIZE*SIZE;
    while((counter++) < num_of_iter){
      std::pair<int, int> res = choose_pair();
      int u = res.first, v = res.second;
      if(edge(u, v, g).second){
        remove_edge(u, v, g);
      }
      else{
        add_edge(u, v, g);
        if(!boyer_myrvold_planarity_test(boyer_myrvold_params::graph = g)){
          remove_edge(u, v, g);
        }
      }
    }
    graph_traits<graph>::edge_iterator ei, ei_end;
    property_map<graph, edge_index_t>::type e_index;
    graph_traits<graph>::edges_size_type edge_count = 0;
    
    make_connected(g);
    edge_count = 0;
    for(tie(ei, ei_end) = edges(g); ei != ei_end; ++ei)
      put(e_index, *ei, edge_count++);
    boyer_myrvold_planarity_test(boyer_myrvold_params::graph = g,
                                   boyer_myrvold_params::embedding = 
                                       &embedding[0]
                                   );
    make_biconnected_planar(g, &embedding[0]);

    edge_count = 0;
    for(tie(ei, ei_end) = edges(g); ei != ei_end; ++ei)
      put(e_index, *ei, edge_count++);
    
    boyer_myrvold_planarity_test(boyer_myrvold_params::graph = g,
                                   boyer_myrvold_params::embedding = 
                                       &embedding[0]
                                   );
    make_maximal_planar(g, &embedding[0]);
    edge_count = 0;
    for(tie(ei, ei_end) = edges(g); ei != ei_end; ++ei)
      put(e_index, *ei, edge_count++);
    
    std::cout<<num_vertices(g)<<" "<<num_edges(g)<<std::endl;
    for(tie(ei, ei_end) = edges(g); ei != ei_end; ++ei){
      std::cout<<source(*ei, g)<<" "<<target(*ei, g)<<"\n";
    }
    std::cout<<std::endl;
    
    
  }

  return 0;
}

