#ifndef _COMMON_DEFINES_H_
#define _COMMON_DEFINES_H_

#include <iostream>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/properties.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/ref.hpp>
#include <vector>
#include <set>
#include <map>
#include <queue>
#include <stack>
#include <climits>
#include <boost/graph/make_biconnected_planar.hpp>
#include <boost/graph/make_maximal_planar.hpp>
#include <boost/graph/planar_face_traversal.hpp>
#include <boost/graph/boyer_myrvold_planar_test.hpp>
#include <boost/graph/make_connected.hpp>
#include <boost/graph/planar_canonical_ordering.hpp>
#include <boost/graph/chrobak_payne_drawing.hpp>
#include <boost/graph/connected_components.hpp>

#define DBG 0
#define DBG2 0
#define MAX_SIZE 2000
#define MAX_NUM_OF_LAYERS 50
#define MAX_NUMBER_OF_NEIGHBOURS 4
#define NO_VERTEX 10000
#define fori(i, k, n) for(int i = (int)k; i < (int)n; i++)
#define K_MAX ULONG_MAX
#define MAX_AMOUNT_OF_SETS 1100000
#define NUM_OF_BITS 64
#define MIN_INF LLONG_MIN
#define ACCURACY 3 //dokladnosc techniki bakera

using namespace boost;
struct coord_t;
typedef long long Kl;
typedef adjacency_list
    < vecS, vecS, undirectedS, 
      property<vertex_index_t, int>, 
      property<edge_index_t, int>
    > input_graph;
typedef std::vector< graph_traits<input_graph>::edge_descriptor > vec_t;
typedef std::vector<graph_traits<input_graph>::vertex_descriptor>  vertex_storage_t;
typedef std::vector<vertex_storage_t> faces_storage;
typedef std::set<graph_traits<input_graph>::vertex_descriptor> deleted_vertex_set;
typedef std::vector< coord_t > straight_line_drawing_storage_t;
typedef graph_traits<input_graph>::vertex_descriptor vert_desc;
typedef boost::iterator_property_map
    < straight_line_drawing_storage_t::iterator, 
      property_map<input_graph, vertex_index_t>::type 
    > straight_line_drawing_t;
typedef vert_desc K;
typedef std::pair<K, K> para;
typedef std::map<para, vert_desc>::iterator pos_map_iter;
typedef std::set<K>::iterator set_it_t;
typedef std::pair<K, K> paraK;

struct coord_t
{
  std::size_t x;
  std::size_t y;
};

struct my_edge_tree{
  K dest;
  int color,ctp, layer, in_tree;
  my_edge_tree(K dest = 0, int color = 0, int ctp = 0, int layer = 0, int in_tree = 0): 
    dest(dest), color(color), ctp(ctp), layer(layer), in_tree(in_tree){ // color - 1 lezy na cyklu 0 wpp 
  }
};

struct my_edge{
  K t, c, used; //target, counterpart
  my_edge(int t=0, int c=0): t(t), c(c), used(0){
  }
};

#endif
