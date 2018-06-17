#include "defines.hpp"
/*
Dodajac do drzewa rozpinajacego kolejne sciany zewnetrzne
dodawac bedziemy krawedzi lezace:
na sciezkach/drzewach sciany zewnetrznej
krawedzi cyklowe, ale ktore zostane moga dodac w nastepujacej procedurze:
W ustalonym kierunku przechodzimy cykl, startując z wybranego wierzchołka v. 
Jeżeli krawedz laczy wierzcholki lezace jedynie na cyklu, to je
dodajemy do drzewa; 
jezeli doszlismy do wierzcholka, ktory nalezy do jakiejs sciezki ze sciany zewnetrznej,
to te krawedz dodajemy do drzewa;
jezeli doszlismy do wierzcholka, ktory nalezy do drzewa z lasu powstalego w wczesniejszym kroku,
to sprawdzamy, czy tego drzewa juz nie odwiedzalismy.
Jezeli juz byl odwiedzony, to nie dodajmy krawedzi, ktora przeszlismy;
wpp dodajemy;
*/

int my_graph::check_if_are_neighbours(vert_desc x, vert_desc y){
  fori(i, 0, V[x].size()){
    if(V[x][i].t == y) return i;
  }
  return -1;
}

bool my_graph::check_if_already_neighbours(int lay, vert_desc x, vert_desc y){
  fori(i, 0, STR_GRAPH[lay][x].size()){
    if(STR_GRAPH[lay][x][i].dest == y) return true;
  }
  return false;
}

//stripping_graf zawiera sciany zewnetrzne kolejnych warstw
//warstwy sa numerowane w odwrotnej kolejnosci
//sciana zewnetrzna grafu k-planarnego jest w warstkie k-tej
void my_graph::create_stripping_graph(){
  for(auto it = Edge_Map.begin(); it != Edge_Map.end(); it++){
    int lay = f_stor.size() - (it->second);
    K pos_b = it->first.first, pos_e = it->first.second;
    STR_GRAPH[lay][pos_b].push_back(my_edge_tree(pos_e, 0, STR_GRAPH[lay][pos_e].size(), lay));
    STR_GRAPH[lay][pos_e].push_back(my_edge_tree(pos_b, 0, STR_GRAPH[lay][pos_b].size() - 1, lay));
  }
}

//przejdz przez graf i sprawdz, czy konce krawedzie nie ulegly zmianie
//jezeli krance sie zmienily, to zmiana ta zostala zapisana w mapie Change_edge_map.
//jezeli tak, to zmodyfikuj stripping graf w odpowiedni sposob
//nastepnie dodaj do niego krawedzi na sciezce pomiedzy wierzcholkami rozszerzajacymi ten sam wierzcholek
void my_graph::modify_stripping_graph(){
  fori(lay, 0, f_stor.size()+1){
    fori(i, 0, SIZE){
      fori(j, 0, STR_GRAPH[lay][i].size()){
        int x = STR_GRAPH[lay][i][j].dest, ind = STR_GRAPH[lay][i][j].ctp;
        if(i < x && x < SIZE){ //zmien wierzcholki krawedzi jezeli trzeba
          std::map<para, para>::iterator it;
          if((it = Change_edge_map.find(std::make_pair(i, x))) != Change_edge_map.end()){
            para sec = it -> second;
            if(sec.first >= (K)SIZE && sec.second >= (K)SIZE){
              STR_GRAPH[lay][sec.first].push_back(my_edge_tree(sec.second, 0, STR_GRAPH[lay][sec.second].size(), lay));
              STR_GRAPH[lay][sec.second].push_back(my_edge_tree(sec.first, 0, STR_GRAPH[lay][sec.first].size()-1, lay));
              continue;
            }
            if(sec.first >= (K)SIZE && sec.second < (K)SIZE){
              STR_GRAPH[lay][sec.first].push_back(my_edge_tree(x, 0, ind, lay));
              STR_GRAPH[lay][x][ind].dest = sec.first;
              STR_GRAPH[lay][x][ind].ctp = STR_GRAPH[lay][sec.first].size()-1;
              continue;
            }
            if(sec.first < (K)SIZE && sec.second >= (K)SIZE){
              STR_GRAPH[lay][sec.second].push_back(my_edge_tree(i, 0, j, lay));
              STR_GRAPH[lay][i][j].dest = sec.second;
              STR_GRAPH[lay][i][j].ctp = STR_GRAPH[lay][sec.second].size()-1;
            }
          }
        }
      }
    }
  }
  //dodawanie krawedzi pomiedzy wierzcholkami rozszerzajacymi i dodanie ich do tej tej warstwy do ktorej nalezal wierzcholek rozszerzany
  fori(i, SIZE, auxiliary_gr_size){
    K pari = vert_map.find(i) ->second;
    fori(j, 0, V[i].size()){
      K x = V[i][j].t, parx;
      if(x >= (K)SIZE && x > (K)i){
        parx = vert_map.find(x)->second;
        if(parx == pari){
          int lay = f_stor.size() - VERT_AGE[parx];
          STR_GRAPH[lay][x].push_back(my_edge_tree(i, 0, STR_GRAPH[lay][i].size(), lay));
          STR_GRAPH[lay][i].push_back(my_edge_tree(x, 0, STR_GRAPH[lay][x].size()-1, lay));
        }
      }
    }
  }
  //wyczysc sasiedztwo rozszerzanych wierzcholkow, tylko w warstwach gdzie jest mozliwe pojawienie sie krawedzi, 
  //ktorych koncem jest rozszerzany wierzcholek, tzn warstwa wierzcholka oraz warstwy nad i pod nia
  for(auto it = reduce.begin(); it != reduce.end(); it++){
    int kon = std::max(f_stor.size() - VERT_AGE[*it] + 1, f_stor.size());
    int pocz = f_stor.size() - VERT_AGE[*it] - 1;
    fori(j, pocz, kon+1){
      STR_GRAPH[j][*it].clear();
    }
  }
}
//przejdz dfs po grafie i znajdz cykle, a nastepnie zapisz ktore krawedzie leza na cyklu
//zwracamy numer wierzcholka, ktory jako pierwszy odwiedzilismy na cyklu
int my_graph::dfs(int x, int p, int layer, int comp, int beginxx, int endxx){
  D[x] = 1;
  int k = -1;
  for(auto it = STR_GRAPH[layer][x].begin(); it != STR_GRAPH[layer][x].end(); it++){
    //jezeli sasiad nie jest ojcem oraz lezy pomiedzy interesujacymi nas warstwami
    if((int)(*it).dest == p || !check_if_in_layers(it->dest, beginxx, endxx)) continue; 
    if(D[(*it).dest] == 1){ //znalazlem cykl
      k = (*it).dest; //zapisz pierwszy wierzcholek na cyklu
      (*it).color = 1; //ustal dla krawedzi ktora rozpatrujemy ze lezy na cyklu
      STR_GRAPH[layer][(*it).dest][(*it).ctp].color = 1; //oraz dla krawedzi pomiedzy pierwszym wierzcholkiem a obecnym, ze lezy na cyklu
    }
    else{
      if(D[(*it).dest] == 0){ //jezeli jeszcze nie odwiedzalismy wierzcholka
        if(k == -1){
          k = dfs((*it).dest, x, layer, comp, beginxx, endxx); 
          if(k != -1){ //wierzcholek lezy na jakism cyklu
            (*it).color = 1;
            STR_GRAPH[layer][(*it).dest][(*it).ctp].color = 1; //zaznacz dla krawedzi, ze lezy na cyklu
          }
        }
        else
          dfs((*it).dest, x, layer,comp, beginxx, endxx);
      }
    }
  }
  if(k != -1){ //jezeli wierzcholek lezy na cyklu to to zaznacz
    C[x] = 1;
    if(k == x) k = -1; //jezeli jestem pierwszym wierzcholkiem tego cyklu, to przeszedlem caly cykl i juz zadne inne wierzcholki nie moga lezec na tym cyklu
  }
  D[x] = 2;
  components[comp].push_back(x);
  return k;
}

//przejdz po utworznym drzewie i przypisz wierzcholkom aktualny numer drzewa do ktorego naleza
void my_graph::tree_dfs(int x, int number){
  if(T[x] == number) return;
  T[x] = number;
  for(int i = 0; i < (int)Tree[x].size(); i++){
    if(T[Tree[x][i].dest] != number)
      tree_dfs(Tree[x][i].dest, number);
  }
}

void my_graph::clear_m_t(int n){
  for(int i = 0; i < n; i++)
    D[i] = C[i] = Type[i] = VisitedVertice[i] = 0;
  components.clear();
}

//sprawdz ilu wierzcholek x ma w warstwie lay, takich ze leza oni na warstwach pomiedzy beg a end
int my_graph::count_neighbours_in_layers(int lay, int x, int beg, int end){
  int res = 0;
  fori(i, 0, STR_GRAPH[lay][x].size()){
    if(check_if_in_layers(STR_GRAPH[lay][x][i].dest, beg, end)) ++res;
  }
  return res;
}

//wyczysc w tablicy STR_GRAPH pomiedzy warstwami wyznaczonymi przez beginxx oraz endxx
//znacznik, ze krawedz nalezy do cyklu oraz, ze zostala uzyta w drzewie
//tak samo wyczysc tablice T oraz P i AlreadyVisitedTrees
void my_graph::clear_STR_GRAPH_cycle(int n, int beginxx, int endxx){
  int poczatek = 0, koniec = f_stor.size();
  poczatek = f_stor.size() - endxx;
  koniec = f_stor.size() - beginxx;
  fori(i, poczatek, koniec+1){
    fori(j, 0, n){
      fori(k, 0, STR_GRAPH[i][j].size()){
        STR_GRAPH[i][j][k].color = 0;
        STR_GRAPH[i][j][k].in_tree = 0;
      }
    }
  }
  clear_m_t(n);
  fori(i, 0, n){
    T[i] = -1;
    P[i][0] = i;
    for(int j = 1; j < 20; j++)
      P[i][j] = -1;
    Level[i] = 0;
  }
  fori(i, 0, num_of_trees)
    AlreadyVisitedTrees[i] = 0;
}

//skonstruj drzewo z warstw pomiedzy beginxx a endxx
void my_graph::make_tree(int beginxx, int endxx){
  int number_of_trees = 0;
  int poczatek = 0, koniec = f_stor.size();
  poczatek = f_stor.size() - endxx; //wyznacz odpowiednie numery warstw w grafie stripping graph
  koniec = f_stor.size() - beginxx;
  for(int lay = poczatek; lay < koniec+1; lay++){
    int comp = 0;
    clear_m_t(auxiliary_gr_size); //wyczysc graf
    components.clear();
    for(int i = 0; i < auxiliary_gr_size; i++){
      if(D[i] == 0 && check_if_in_layers(i, beginxx, endxx)){ //przejdz dfs po grafie, ale tylko na wierzcholkach, ktore sa w interesujacych nas warstwach
        components.push_back(vertex_storage_t());
        dfs(i, i, lay, comp++, beginxx, endxx);
      }
    }
    for(int comp_num = 0; comp_num < (int)components.size(); comp_num++){
      //jezeli wierzcholek w danej warstwie jest wierzcholkiem izolowanym i nie lezy pomiedzy interesujacymi nas warstwami to go pomin
      if(components[comp_num].size() == 1 && !check_if_in_layers(components[comp_num][0], f_stor.size()-lay, f_stor.size()-lay+2)) continue;
      std::vector<vert_desc> alpha; // wierzcholki, ktore lacza okregi z drzewem
      for(int w = 0; w < (int)components[comp_num].size(); w++){
        int i = components[comp_num][w];
        bool branch = true;
        for(int j = 0; j < (int)STR_GRAPH[lay][i].size(); j++){ //dla kazdego wierzcholka sprawdz czy lezy na cyklu
          if(STR_GRAPH[lay][i][j].color == 1) branch = false;
        }
        //ustal jego typy
        if(branch == true)  Type[i] = 4;
        else{
          if(count_neighbours_in_layers(lay, i, beginxx, endxx) == 3){ //jest na okregu i laczy galaz
            Type[i] = 1;
            alpha.push_back(i);
          }
          else{
            if(count_neighbours_in_layers(lay, i, beginxx, endxx) == 2 && T[i] != -1){ // jest na cyklu i laczy drzewo z wczesniejszej fazy
              Type[i] = 2;
            }
            else{ //jest tylko na okregu
              Type[i] = 3;
            }
          }
        }
      }
      int tree_dfs_starting_point = -1; 
      int first_from_the_cycle = -1;
      for(auto it = components[comp_num].begin(); it != components[comp_num].end(); it++){
        int i = (*it);
        if(Type[i] == 4){
          if(count_neighbours_in_layers(lay, i, beginxx, endxx) == 0){
            tree_dfs_starting_point = i;
          }
          for(int j = 0; j < (int)STR_GRAPH[lay][i].size(); j++){ //do grafu dodaj krawedzie będace galeziami
            if(i < (int)STR_GRAPH[lay][i][j].dest){
              if(Type[STR_GRAPH[lay][i][j].dest] == 4){
                if(tree_dfs_starting_point == -1) tree_dfs_starting_point = i;
                Tree[i].push_back(my_edge_tree(STR_GRAPH[lay][i][j].dest, 0, Tree[STR_GRAPH[lay][i][j].dest].size()));
                STR_GRAPH[lay][i][j].in_tree = 1;
                Tree[STR_GRAPH[lay][i][j].dest].push_back(my_edge_tree(i, 0, Tree[i].size()-1));
                STR_GRAPH[lay][STR_GRAPH[lay][i][j].dest][STR_GRAPH[lay][i][j].ctp].in_tree = 1;
              }
            }
          }
        }
        if((Type[i] == 3 || Type[i] == 2) && first_from_the_cycle == -1){ //jezeli nic nie zostalo wybrane, zapisz, ze i tezy na cyklu
          first_from_the_cycle = i;
        }
        if(Type[i] == 1){ //do drzewa dodaj krawedzie wychodzaca z wierzcholka, ktora nie lezy na cyklu
          for(int j = 0; j < (int)STR_GRAPH[lay][i].size(); j++){
            if(STR_GRAPH[lay][i][j].color == 0){
              if(Type[STR_GRAPH[lay][i][j].dest] == 1 && i > (int)STR_GRAPH[lay][i][j].dest) continue;
              if(tree_dfs_starting_point == -1) tree_dfs_starting_point = i; //jezeli jeszcze nie zostal wybrany zaden wierzcholek zaznacz, ze i lezy na cyklu
              Tree[i].push_back(my_edge_tree(STR_GRAPH[lay][i][j].dest, 0, Tree[STR_GRAPH[lay][i][j].dest].size()));
              Tree[STR_GRAPH[lay][i][j].dest].push_back(my_edge_tree(i, 0, Tree[i].size()-1));
              STR_GRAPH[lay][i][j].in_tree = 1;
              STR_GRAPH[lay][STR_GRAPH[lay][i][j].dest][STR_GRAPH[lay][i][j].ctp].in_tree = 1;
            }
          }
        }
      }
      if((int)alpha.size() == 0 && first_from_the_cycle != -1){ //jezeli okrag jest izolowany, to zacznij od dowolnego wierzcholka na nim
        alpha.push_back(first_from_the_cycle);
        tree_dfs_starting_point = first_from_the_cycle;
      }
      
      for(auto it = alpha.begin(); it != alpha.end(); it++){
        if(VisitedVertice[*it]) continue; 
        VisitedVertice[*it] = 1;
        int prev = *it, beg = 0; 
        for(int i = 0; i < (int)STR_GRAPH[lay][*it].size(); i++){ 
          if(STR_GRAPH[lay][*it][i].color == 1){
            beg = STR_GRAPH[lay][*it][i].dest;
          }
        }//dodaj krawedzie na cyklu, tylko jezeli nie lacza juz odwiedzonego drzewa z wczesniejszej warstwy
        if(Type[prev] == 2) AlreadyVisitedTrees[T[prev]] = 1;
        while(beg != (int)*it){
          VisitedVertice[beg] = 1;
          if(Type[beg] == 2){
            if(!AlreadyVisitedTrees[T[beg]]){
              AlreadyVisitedTrees[T[beg]] = 1;
              Tree[prev].push_back(my_edge_tree(beg, 0, Tree[beg].size()));
              Tree[beg].push_back(my_edge_tree(prev, 0, Tree[prev].size()-1));
              STR_GRAPH[lay][beg][find_vertex(lay, beg, prev)].in_tree = 1;
              STR_GRAPH[lay][prev][find_vertex(lay, prev, beg)].in_tree = 1;
            }
          }
          else{
            Tree[prev].push_back(my_edge_tree(beg, 0, Tree[beg].size()));
            Tree[beg].push_back(my_edge_tree(prev, 0, Tree[prev].size()));
            STR_GRAPH[lay][beg][find_vertex(lay, beg, prev)].in_tree = 1;
            STR_GRAPH[lay][prev][find_vertex(lay, prev, beg)].in_tree = 1;
          }
          int tmp = prev;
          prev = beg;
          for(int i = 0; i < (int)STR_GRAPH[lay][prev].size(); i++){
            if(STR_GRAPH[lay][prev][i].color == 1 && (int)STR_GRAPH[lay][prev][i].dest != tmp){
              beg = STR_GRAPH[lay][prev][i].dest;
            }
          }
        } 
      }
      //przejdz po drzewie i nadaj wierzcholkom odpowiedni numer
      tree_dfs(tree_dfs_starting_point, number_of_trees++);
      alpha.clear();
    }
    clear_m_t(auxiliary_gr_size);
    
  }
  num_of_trees = number_of_trees;
}

int my_graph::find_vertex(int lay, int x, int y){
  fori(i, 0, STR_GRAPH[lay][x].size()){
    if((int)STR_GRAPH[lay][x][i].dest == y) return i;
  }
  return -1;
}
