#ifndef _DEFINES_H_
#define _DEFINES_H_

#include "common_defs.hpp"
#include "tree_vert_defs.hpp"
#include "tree_decomp_defs.hpp"

class my_graph{
  input_graph g, g_copy, g_constant; //graf + jego kopie
  std::vector<vec_t> embedding, embedding_copy; 
  property_map<input_graph, edge_index_t>::type e_index;
  graph_traits<input_graph>::edges_size_type edge_count = 0;
  graph_traits<input_graph>::edge_iterator ei, ei_end;
  faces_storage f_stor; //struktura przechowujaca warstwy
  faces_storage of_storage; //sciany zewnetrzne naszego grafu
  faces_storage input_faces, components; //wszystkie sciany naszego grafu i struktura przechowujaca spojne skladowe
  vertex_storage_t cur_ver; //wierzcholki lezace na zewnetrznej scianie
  std::set<vert_desc> reduce; //zbior przechowujacy wierzcholki, ktore beda rozszerzane
  std::queue<int> visited;
  std::vector<para> vert_faces[MAX_SIZE];
  int FACES_AGE[MAX_SIZE], VERT_AGE[MAX_SIZE], VERT_YOUNGEST[MAX_SIZE]; //warstwa sciany, warstwa wierzcholka i sciana z nawczesniejszej warstwy dla danego wierzcholka
  //int VIS[MAX_SIZE]; //czy juz odwiedzony podczas tworzenia drzewa
  int D[MAX_SIZE]; //tablica odwiedzonych w dfs
  int C[MAX_SIZE]; //czy lezy na cyklu
  int T[MAX_SIZE]; //numer drzewa do ktorego nalezy wierzcholek podczas tworzenia
  int Type[MAX_SIZE]; //jakiego koloru jest dany wierzcholek podczas tworzenia drzewa
  int AlreadyVisitedTrees[MAX_NUM_OF_LAYERS*MAX_SIZE]; //odwiedzone drzewa podczas tworzenia drzewa nastepnej warstwy
  int VisitedVertice[MAX_SIZE]; //odwiedzone wierzcholki podczas tworzenia nastpengo drzewa
  std::vector<my_edge_tree> Tree[MAX_SIZE]; //pamieta konstrowane drzewo
  std::pair<para,para> NEIGHBOURS[MAX_SIZE]; //dla kazdego wierzcholka przechowuje jego sasiadow w scianie z najwczesniejszej warstwy
  std::vector<my_edge> V[MAX_SIZE]; //pomocniczy graf
  std::map<para, vert_desc> pos_map; 
  std::map<para, para> Change_edge_map; //mapa przechowujaca zmiany koncow krawedzi podczas rozszerzania oraz  
                                        //na poczatku indeksy krawedzi w embeddingu
  int SIZE, auxiliary_gr_size;
  std::map<K, K> vert_map; //mapa trzymajaca dla rozszerzajacych wierzcholkow informacje kogo rozszerzaja
  int P[MAX_SIZE][21], Level[MAX_SIZE]; //parent i poziom w drzewie
  std::map<para, int> Edge_Map; //Mapa przechowujaca dla krawedzi numer wezla, ktory ja reprezentuje
  std::map<para, K> Graph_Map; //Mapa trzymajaca krawedzi grafu
  TreeDecompositionVertex TDV[MAX_SIZE]; //drzewo dekompozycji
  std::vector<my_edge_tree> STR_GRAPH[MAX_NUM_OF_LAYERS][MAX_SIZE]; //grafy zbudowane na warstwach
  int tree_size, num_of_trees; 
  TreeDecomposition TreeDecomp; //obiekt dekompozycji drzewowej
  int max_face_age = 0, max_vert_age = 0;
  public:
  my_graph(int SIZE): g(SIZE), g_copy(SIZE), g_constant(SIZE), embedding(SIZE), 
                      embedding_copy(SIZE), SIZE(SIZE), auxiliary_gr_size(SIZE)
                      {
  }
  void prepare(); 
  void add_edges();
  void refresh_g();
  void test_for_planarity_and_compute_embedding();
  void make_connect();
  void make_biconnect();
  void make_maxi_connect();
  void compute_layers();
  void compute_outer_faces();
  bool check_if_is_1_outerface();
  void make_embedding_map();
  void clear_embedding_map(){ Change_edge_map.clear();}
  void compute_next_edge(K*, K*);
  void display_edges(int, int);
  
  bool is_edge(K x, K y);
  para find_vertices(int x,int*);
  void make_v_for_1_outerplanar();
  void make_td_for_1_outerplanar();
  
  void clear_cur_ver(){ cur_ver.clear(); }
  void add_cur_ver(){ input_faces.push_back(cur_ver); }
  void add_vertex_to_cur_ver(vert_desc v) { cur_ver.push_back(v);}
  void initialize_edge_index();
  void planar_graph_traverse();
  void compute_faces_to_which_vertex_belongs();
  void check_which_faces_are_outer();
  void create_auxiliary_graph();
  void compute_VERT_AGE();
  void compute_youngest_face_for_each_vertice();
  void compute_neighbours_on_youngest_face();
  void change_vertices_with_degree_greater_than_3();
  void clear_input_graph_g();
  void clear_input_graph_g_copy();
  void create_stripping_graph();
  bool check_if_already_neighbours(int, vert_desc, vert_desc);
  int check_if_are_neighbours(vert_desc, vert_desc);
  
  void modify_stripping_graph();
  void add_changed_edge(K, K, K);
  void display_STR_GRAPH();
  void display_VERT_AGE();
  void display_FACES_AGE();
  void display_NEIGHBOURS();
  void display_auxiliary_graph();
  void display_embedding();
  void display_embedding_copy();
  void display_vert_map();
  void display_input_faces();
  void display_of_storage();
  void display_layers();
  void display_f_stor();
  int find_vertex(int, int, int);
  int dfs(int, int, int, int, int, int);
  void tree_dfs(int, int);
  void clear_m_t(int);
  void make_tree(int, int);
    
  void ancestor(int);
  void bfs(int);
  int poziom(int, int);
  vert_desc lca(vert_desc, vert_desc);
  void append_vertices(int, int, int, bool);
  void make_tree_decomposition(int, int);
  int get_num_of_layers(){ return f_stor.size(); }
  void make_t_d_object(int root){
    TreeDecomp = TreeDecomposition(tree_size, root, TDV, Tree);
    TreeDecomp.fill_the_map(Graph_Map.begin(), Graph_Map.end());
  }
  TreeDecomposition& get_tree_decomposition(){ return TreeDecomp; }
  bool check_if_in_layers(int, int, int);
  void clear_STR_GRAPH_cycle(int, int, int);
  int count_neighbours_in_layers(int, int, int, int);
  void clear_tree();
};

struct output_visitor: public planar_face_traversal_visitor{
  my_graph &g;
  output_visitor(my_graph &g): g(g){
  }
  void begin_face() { g.clear_cur_ver();}
  void end_face() { g.add_cur_ver(); }
  template <typename Vertex> 
  void next_vertex(Vertex v) { g.add_vertex_to_cur_ver(v); }
};

#endif
