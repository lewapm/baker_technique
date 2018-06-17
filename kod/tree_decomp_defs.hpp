#ifndef _TREE_DECOMP_DEFS_H_
#define _TREE_DECOMP_DEFS_H_

#include "common_defs.hpp"
#include "tree_vert_defs.hpp"

//deklaracje klasy TreeDecompozition, przechowujacej dekomozycje drzewowa
//tworzy rowniez ladna dekomozycje drzewowa i wylicza maksymalny zbior niezalezny
class TreeDecomposition{
  TreeDecompositionVertex Vert[MAX_SIZE]; //tablica wezlow
  std::vector<my_edge_tree> Graph[MAX_SIZE]; //tablica krawedzi
  int DVis[MAX_SIZE]; //
  std::map<para, K> Edge_Map; //poczatkowy graf zapisany jest w jako mapa krawedzi 
  bool nice = false, computed_m_i_s = false; //czy jest ladna dekompozycja, czy jest obliczony maksymalny zbior niezalezny
  K size, root, beg_size, max_set; //num of vertices, tree_root
 public:
  TreeDecomposition(){
    size = 0; 
    root = -1;
  }
  TreeDecomposition(int s, int r, TreeDecompositionVertex t[MAX_SIZE], std::vector<my_edge_tree> g[MAX_SIZE]){
    size = s;
    beg_size = s;
    root = r;
    fori(i, 0, size){
      Vert[i] = t[i];
      Graph[i] = g[i];
      DVis[i] = -1;
    }
  }
  
  void fill_the_map(std::map<para, K>::iterator ibeg, std::map<para, K>::iterator iend);
  K compute_independent_set(); //oblicz maksymalny zbior niezalezny
  void display();  //wyswitle zawartosc
  void make_nice(); //z podanej dekomozycji drzewowej zrob ladna dekomozycje
  void m_n_dfs(int, int);  //przejdz po drzewie i dodaj wezly tak, aby kazde dwa roznily sie tylko na jednym wierzcholku
                           //jezeli dwa sasiednie wezly maja takie same zbiory wierzcholkow to usun jeden z nich 
  void m_n_2_dfs(int, int); //przejdz po drzewie i jezeli jest taka koniecznosc, to stworz wezly laczace
  void make_path_of_kids(int); //dodaj wezly do lisca, tak aby nowy lisc mial tylko jeden wierzcholek w zbiorze
  void make_2_neighbours(int, int); //dodaj wezly laczace
  void append_del_add_vert(int, int, int, int); //dodaj wezly tak, aby kazde dwa roznily sie tylko na jednym wierzcholku
  
  bool is_indep_with_special(K x, K s, K num, K s_num); //sprawdz, czy zbior jest niezalezny
  K add_bit_to_number(K num, K x, K son); //zmien maske bitowa dodajace do niej stary wierzcholek
  //uzywana jest podczas liczenia zbioru niezaleznego dla podzbioru wezla zapominajacego
  
  K del_bit_from_number(K num, K x, K son, K); //zmien maske bitowa usuwajac z niej nowy wierzcholek
  //uzywana jest podczas liczenia zbioru niezaleznego dla podzbioru wezla wprowadzajacego
  void ind_set_dfs(K x, K p); //policz maksymalny zbior niezalezny
  K find_diff(K x, K y, bool); //znajdz wierzcholek na ktorym dwa wezly sie roznia
  void set_type_and_value_dfs(int x, int p); //ustaw czy wezel jest wezlem zapominajacym, czy tez wezlem 
                                             //wprowadzajacym, czy tez wezlem laczacym
  ~TreeDecomposition(){
    TreeDecompositionVertex Vert[MAX_SIZE];
    fori(i, 0, size){
      Vert[i].clear();
      Graph[i].clear();
      DVis[i] = -1;
    }
    Edge_Map.clear();
  }
};

#endif
