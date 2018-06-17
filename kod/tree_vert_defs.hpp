#ifndef _TREE_DEFS_H_
#define _TREE_DEFS_H_
//deklaracja skladowej klasy TreeDecompositionVertex, ktora jest 
//wezlem dekompoztycji
#include "common_defs.hpp"

class TreeDecompositionVertex{
 std::set<K> Graph; //wierzcholki grafu
 bool* Is_indep; //czy jest niezalezny
 Kl* Max_indep; //wielkosc maksymalnego zbioru niezaleznego
 int size = 0, type; //0 - zapominajacy; 1 - wprowadzajacy; 2 - laczacy
 std::map<K, K> Vert_map; //mapa wierzcholek na pozycje w secie
 std::vector<K> Index_vec; //mapa pozycji w secie na wierzcholek
 K special_vert; //wierzcholek specjalny, tzn dodany albo usuniety
 public:
  TreeDecompositionVertex(): Graph(std::set<K>()), size(0){
  }
  TreeDecompositionVertex(const std::set<K>& g): Graph(g){
  }
  TreeDecompositionVertex(const TreeDecompositionVertex& T){
    size = T.get_size();
    auto p = T.get_iterators();
    for(auto it = p.first; it != p.second; it++){
      Graph.insert(*it);
    }
  }
  void clear(){
    Graph.clear();
    Vert_map.clear();
    Index_vec.clear();
    special_vert = type = size = 0;
  }
  void set_type(int x){ type = x; }
  int get_type(){ return type; }
  
  std::pair<set_it_t, set_it_t> get_iterators() const{
    return std::make_pair(Graph.begin(), Graph.end());
  }
  int get_size() const{ return Graph.size(); }
  K get_pos_of_vert(K x){ return Vert_map.find(x)->second; }
  K get_smallest() { return *(Graph.begin()); }
  K get_special_vert(){ return special_vert; }
  bool is_indep(K res){ return Is_indep[res]; }
  Kl max_indep(K res){ return Max_indep[res]; }
  void set_max_indep(K ind, K val){ Max_indep[ind] = val; }
  void set_is_indep(K ind, bool val){ Is_indep[ind] = val; }
  K get_num_of_sets(){ return (1<<Graph.size()); }
  void set_special_vert(K vert){ special_vert = vert; } 
  
  void add_vertice(K); //dodaj wierzcholek do zbioru
  void del_vertice(K); //usun wierzcholek ze zbioru
  Kl find_largest(); //znajdz podzbior o najwiekszej wartosci Max_indep
  void allocate_memory(); //zaaloku tablice
  void deallocate_memory(); //usun tablice
  void assign_number_to_vertex(); 
  void display(); 
  K get_size_of_set(K num); 
  void make_set_out_of_index(K num, std::vector<K>& A);//zwroc wierzcholki z maski bitowej num

};

#endif
