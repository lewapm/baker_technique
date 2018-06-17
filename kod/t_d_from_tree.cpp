#include "defines.hpp"

//uzupelnanie tablicy przodka w drzewie
void my_graph::ancestor(int i){
  for(int k=2; k<20; k++){
    P[i][k]=P[P[i][k-1]][k-1];
  }
}

//bfs, ktory ma na celu wyliczenie tablicy P
void my_graph::bfs(int root){
  std::queue<int> Q;
  Q.push(root);
  Level[root] = 0;
  for(int i =0; i < 20; i++) P[root][i] = root;
  while(!Q.empty()){
    int x = Q.front();
    Q.pop();
    for(auto it = Tree[x].begin(); it != Tree[x].end(); it++){
      if(P[(*it).dest][1] == -1){
        Q.push((*it).dest);
        P[(*it).dest][1] = x;
        Level[(*it).dest] = Level[x] + 1;
        ancestor((*it).dest);
      }
    }
  }
}

//przejdz wierzcholkiem a na poziom p
int my_graph::poziom(int a, int p){
    int l=0, o=a;
    while(Level[o]!=p){
        if(Level[P[a][l]]>p){
            l++;
            o=P[a][l];
        }
        if(Level[P[a][l]]<p){
            l--;
            a=P[a][l];
            o=a;
            l=0;
        }
    }
return o;
}
//wylicz lca dla wierzcholkow a i b
vert_desc my_graph::lca(vert_desc a, vert_desc b)
{
    vert_desc la, lb, l=0;
    if(Level[a]<Level[b])
        swap(a, b);
    a=poziom(a, Level[b]);
    la=a;
    lb=b;
    while(la!=lb){
        if(P[a][l]!=P[b][l]){
            l++;
            la=P[a][l];
            lb=P[b][l];
        }
        if(P[a][l]==P[b][l] && l>1){
            a = P[a][l-1];
            b=P[b][l-1];
            la=a;
            lb=b;
            l=0;
        }
    }
return la;
}

//na sciezce pomiedzy begin a end dodaj wierzcholek av do zbiorow
void my_graph::append_vertices(int begin, int end, int av, bool type){
  int pocz = begin, par, a, b;
  while(pocz != end){
    par = P[pocz][1];
    a = pocz, b = par;
    if(a > b) swap(a, b);
    auto it = Edge_Map.find(std::make_pair(vert_desc(a), vert_desc(b)));
    TDV[it->second].add_vertice(av);
    TDV[pocz].add_vertice(av);
    pocz = P[pocz][1];
  }
  TDV[end].add_vertice(av);
}

void my_graph::make_tree_decomposition(int beginxx, int endxx){
  int dst;
  vert_desc a, b;
  std::map<int, int> TreeMap;
  int root_tree = 0;
  //szukamy wierzcholka, ktory moze byc korzeniem naszzego drzewa
  fori(i, 0, auxiliary_gr_size){
    if(reduce.find(i) == reduce.end() && check_if_in_layers(i, beginxx, endxx)){
      root_tree = i;
      break;
    }
  }
  TreeMap.insert(std::make_pair(T[root_tree], root_tree)); // dodajemy krawedzie tak, aby drzewo bylo spojne
  fori(i, 0, auxiliary_gr_size){                            // z wykluczeniem rozszerzonym wierzcholkow
    if(check_if_in_layers(i, beginxx, endxx) && (reduce.find(i) == reduce.end()) && (TreeMap.find(T[i]) == TreeMap.end())){
      TreeMap.insert(std::make_pair(T[i], i));
      Tree[i].push_back(my_edge_tree(root_tree, 0, Tree[root_tree].size()));
      Tree[root_tree].push_back(my_edge_tree(i, 0, Tree[i].size()-1));
    }
  }
  Edge_Map.clear();
  //do kazdego wezla reprezentujacego wierzcholek dodaj ten wierzcholek
  //dla kazdej krawedzi pomiedzy wezlami stworz nowy wezel i dodaj do niego krance krawedzi
  for(int i = 0; i < auxiliary_gr_size; i++){
    if(reduce.find(i) != reduce.end() || !check_if_in_layers(i, beginxx, endxx)) continue;
    TDV[i].add_vertice(i);
    P[i][0] = i;
    fori(j, 0, Tree[i].size()){
      if(i < (int)Tree[i][j].dest){
        a = i;
        b = Tree[i][j].dest;
        dst = auxiliary_gr_size + Edge_Map.size();
        Edge_Map.insert(std::make_pair(std::make_pair(a, b), dst));
        TDV[dst].add_vertice(a);
        TDV[dst].add_vertice(b);
      }
    }
  }
  bfs(root_tree);
  fori(i, 0, auxiliary_gr_size){
    fori(j, 0, Tree[i].size()){
      int a = i, b = Tree[i][j].dest;
      if(a > b) swap(a,b);
      auto it = Edge_Map.find(std::make_pair(a, b));
      Tree[it->second].push_back(my_edge_tree(i, 0, j));
      Tree[i][j].ctp = Tree[it->second].size()-1;
      Tree[i][j].dest = it->second;
    }
  }
  vert_desc acl;
  int poczatek = 0, koniec = f_stor.size();
  poczatek = f_stor.size() - endxx;
  koniec = f_stor.size() - beginxx;
  //dodaj wszystkie krawedzie niedrzewowe, ktore leza pomiedzy warstwami beginxx a endxx
  //dla kazdej krawedzi znajdz lca jej krancow i na sciezkach pomiedzy nimi a lca dodaj wybrany jeden kraniec 
  fori(lay, poczatek, koniec+1){
    for(int i = 0; i < auxiliary_gr_size; i++){
      if(!check_if_in_layers(i, beginxx, endxx)) continue;
      fori(j, 0, STR_GRAPH[lay][i].size()){
        if(check_if_in_layers(STR_GRAPH[lay][i][j].dest, beginxx, endxx) && i < (int)STR_GRAPH[lay][i][j].dest && STR_GRAPH[lay][i][j].in_tree == 0){
          a = i;
          b = STR_GRAPH[lay][i][j].dest;
          acl = lca(a,b);
          append_vertices(a, acl, a, false);
          append_vertices(b, acl, a, true);
         }
      }
    }
  }
  tree_size = 2*auxiliary_gr_size - 1;
  //przejdz przez dekompozycje i zamien wierzcholki rozszerzajace na wierzcholki rozszerzane
  fori(i, 0, tree_size){
    auto pi = TDV[i].get_iterators();
    for(auto it = pi.first; it != pi.second; it++){
      std::map<vert_desc, vert_desc>::iterator its;
      if((its = vert_map.find(*it)) != vert_map.end()){
        TDV[i].del_vertice((*its).first);
        TDV[i].add_vertice((*its).second);
      }
    }
  }
  make_t_d_object(root_tree);
  clear_STR_GRAPH_cycle(auxiliary_gr_size, beginxx, endxx);
}
