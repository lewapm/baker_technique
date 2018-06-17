#include "tree_decomp_defs.hpp"

void TreeDecomposition::display(){
  fori(i, 0, size){
    std::cout<<i<<" ("<<Vert[i].get_type()<<", "<<Vert[i].get_special_vert()<<"): ";
    Vert[i].display();
    std::cout<<std::endl;
  }
  fori(i, 0, size){
    std::cout<<i<<": ";
    fori(j, 0, Graph[i].size()){
      std::cout<<Graph[i][j].dest<<"("<<Graph[i][j].ctp<<"), ";
    }
    std::cout<<std::endl;
  }
}
  
void TreeDecomposition::make_nice(){
  if(nice) return;
  m_n_dfs(root, root); 
  beg_size = size;
  std::vector<K> tmp;
  std::vector<my_edge_tree> re;
  fori(i, 0, size){ //jezeli m_n_dfs usunelo jeden z sasiednich wezlow, ktore mialy takie same zbiory
                    //to napraw vectory sasiedztwa
    DVis[i] = -1;
    tmp.clear();
    re.clear();
    fori(j, 0, Graph[i].size()){
      if(Graph[i][j].dest != (K)-1) tmp.push_back(j);
    }
    if(tmp.size() != Graph[i].size()){
      fori(j, 0, tmp.size()){
        re.push_back(Graph[i][tmp[j]]);
      }
      Graph[i].clear();
      fori(j, 0, re.size()){
        auto tmp = re[j];
        Graph[tmp.dest][tmp.ctp].ctp = j;
        Graph[tmp.dest][tmp.ctp].dest = i;
        Graph[i].push_back(re[j]);
      }
    }
  }
  m_n_2_dfs(root, root); //rozszerz drzewo, tak aby powstaly wierzcholki laczace
  fori(i, 0, size)
    DVis[i] = -1;
  set_type_and_value_dfs(root, root); //ustal typy wezlow
  fori(i, 0, size){
    Vert[i].assign_number_to_vertex();
  }
  nice = true;
}

void TreeDecomposition::m_n_2_dfs(int x, int p){
  DVis[x] = 1; // ustaw, ze odwiedzony
  if(((int)Graph[x].size() == 1 && (int)Graph[x][0].dest == p) || ((int)Graph[x].size() == 0 && (K)x == root)){ 
    //jezeli jestes w lisciu to wroc
    return; 
  }
  int counter = 0;
  fori(i, 0, Graph[x].size()){ 
    if(DVis[Graph[x][i].dest] == 1){ 
      counter++; 
      continue;
    }
    m_n_2_dfs(Graph[x][i].dest, x);
  }
  if(Graph[x].size() - counter >= 2){ //jezeli wierzcholek ma wiecej niz jedno dziecko, to rozszerz
    make_2_neighbours(x, p);
  }
}

K TreeDecomposition::find_diff(K x, K y, bool val){ //znajdz wierzcholek na ktorym roznia sie zbiory w wezlach
  auto px = Vert[x].get_iterators();
  auto py = Vert[y].get_iterators();
  //dopoki sa takie same to idz dalej
  while((px.first != px.second && py.first!= py.second) &&*(px.first) == *(py.first)){
    px.first++; py.first++;
  }
  if(val) //jezeli wezel jest zapominajacy i py.first wskazuje na wierzcholek ktorego nie ma w x
    return *(py.first);
  return *(px.first);
}

void TreeDecomposition::set_type_and_value_dfs(int x, int p){
  DVis[x] = 1;
  //ustal, ze lisc jest wierzcholkiem wprowadzajacym
  if(((int)Graph[x].size() == 1 && (int)Graph[x][0].dest == p) || ((int)Graph[x].size() == 0 && (K)x == root)){
    Vert[x].set_type(1);
    Vert[x].set_special_vert(Vert[x].get_smallest());
    return; 
  }
  
  //ustaw typ wezla na wezel laczenia i przejdz do dzieci
  if(((K)x == root && (int)Graph[x].size() == 2) || ((K)x != root && (int)Graph[x].size() > 2)){
    fori(i, 0, Graph[x].size()){
      if(DVis[Graph[x][i].dest] == -1)
        set_type_and_value_dfs(Graph[x][i].dest, x);
    }
    Vert[x].set_type(2);
  }
  else{
    int nei = 0;
    if(Graph[x][0].dest == (K)p) nei = 1;
    fori(i, 0, Graph[x].size()){
      if(DVis[Graph[x][i].dest] == -1)
        set_type_and_value_dfs(Graph[x][i].dest, x);
    }
    if(Vert[Graph[x][nei].dest].get_size() > Vert[x].get_size()){
      //wezel zapominajacy, ustaw typ i znajdz wierzcholek zapomniany
      Vert[x].set_type(0);
      Vert[x].set_special_vert(find_diff(x, Graph[x][nei].dest, true));
    }
    if(Vert[Graph[x][nei].dest].get_size() < Vert[x].get_size()){
      //wezel wprowadzajacy, ustaw typ i znajdz wierzcholek wprowadzony
      Vert[x].set_type(1);
      Vert[x].set_special_vert(find_diff(x, Graph[x][nei].dest, false));
    }
  }
}

void TreeDecomposition::m_n_dfs(int x, int p){
  DVis[x] = 1;
  if(((int)Graph[x].size() == 1 && (int)Graph[x][0].dest == p) || Graph[x].size() == 0){
    //jezeli jestes w lisciu to sprawdz czy masz tylko jeden element w zbiorze jak nie to rozszerz
    make_path_of_kids(x);
    return; 
  }
  fori(i, 0, Graph[x].size()){
    if(Graph[x][i].dest == (K)-1 || DVis[Graph[x][i].dest] == 1 || Graph[x][i].dest >= beg_size) continue;
    m_n_dfs(Graph[x][i].dest, x); 
    append_del_add_vert(x, Graph[x][i].dest, i, Graph[x][i].ctp); 
  }
}
  
void TreeDecomposition::make_path_of_kids(int x){
  std::set<K> ds;
  auto px = Vert[x].get_iterators();
  if(px.first == px.second) return;
  ds.insert(*(px.first));
  px.first++;
  if(px.first == px.second){ 
    return;
  }
  //stworz nowy wezel dla jednego z wierzcholkow w zbiorze wierzcholka x
  Vert[size] = TreeDecompositionVertex(ds);
  auto wg = px.first;
  for(auto it = px.first; it != px.second; it++){
    wg = it; //jezeli to jest ostatni element to zakoncz petle
    wg++;
    if(wg == px.second){ 
      break;
    }
    size++;
    ds.insert(*it); //dodaj nowy wezel i polacz go z wczesniejszym
    Vert[size] = TreeDecompositionVertex(ds);
    Graph[size].push_back(my_edge_tree(size-1, 0, Graph[size-1].size()));
    Graph[size-1].push_back(my_edge_tree(size, 0, Graph[size].size()-1));
  }
  //polacz ostatni dodany wezel z wezlem x
  Graph[size].push_back(my_edge_tree(x, 0, Graph[x].size()));
  Graph[x].push_back(my_edge_tree(size, 0, Graph[size].size()-1));
  size++;
}
  
void TreeDecomposition::make_2_neighbours(int x, int p){ //dodaj nowe wierzcholki tak aby 
  // wierzcholek mial 2 sasiadow majacych takie same zbiory jak on sam
  auto px = Vert[x].get_iterators();
  std::set<K> ds;
  //stworz kopie zbioru wierzcholkow wezla x
  for(auto it = px.first; it != px.second; it++){
    ds.insert(*it);
  }
  int last = -1, pocz = 0,ppos = -1, lpos = Graph[x].size()-1; //ppos - indeks ojca na liscie sasiedztwa wierzcholka x; lpos - indeks ostatniego dziecka
  auto root_pr = TreeDecompositionVertex(ds);
  //sprawdz, czy pierwszym wierzcholkiem na liscie sasiedztwa nie jest ojciec, jezeli tak, to pomin
  if((int)Graph[x][pocz].dest == p){ ppos = pocz; pocz++;}
  //stworz nowy wezel i polacz go z pierwszym sasiadem x, niebedacym ojcem
  Vert[size] = root_pr;
  Graph[size].push_back(my_edge_tree(Graph[x][pocz].dest, 0, Graph[x][pocz].ctp));
  Graph[Graph[x][pocz].dest][Graph[x][pocz].ctp].ctp = 0;
  Graph[Graph[x][pocz].dest][Graph[x][pocz].ctp].dest = size;
  last = size;
  size++;
  if(((K)x!= root && (int)Graph[x].size() == 3) || ((K)x == root && (int)Graph[x].size() == 2)){ // jezeli x ma tylko 2 synow
    my_edge_tree tmp, parent;
    if((K)x == root) lpos = 1;
    else{//sprawdz gdzie na liscie jest ojciec i zapisz, gdzie sie znajduje ostatnie dziecko i ojciec
      if(ppos == -1){ 
        lpos = (Graph[x][1].dest == (K)p) ? 2 : 1;
        ppos = (Graph[x][1].dest == (K)p) ? 1 : 2;
      }
      else{
        lpos = 2;
      }
      parent = Graph[x][ppos];
    }
    tmp = Graph[x][lpos]; //skopiuj informacje o ostatnim dziecku
    Graph[x].clear(); //wyczysc sasiedztwo x
    if(ppos != -1){ //do sasiedztwa x dodaj ojca
      Graph[x].push_back(my_edge_tree(parent.dest, 0, parent.ctp));
      Graph[parent.dest][parent.ctp].ctp = 0;
    }
    Vert[size] = root_pr; //do sasiedztwa x dodaj jego dzieci, ale najpierw dzieci polacz z wezlami, ktore sa kopia ojca
                          //polacz kopie ojca z ojcem
    Graph[size].push_back(my_edge_tree(tmp.dest, 0, tmp.ctp));
    Graph[tmp.dest][tmp.ctp].dest = size;
    Graph[tmp.dest][tmp.ctp].ctp = Graph[size].size()-1;
    Graph[size].push_back(my_edge_tree(x, 0, Graph[x].size()));
    Graph[x].push_back(my_edge_tree(size, 0, Graph[size].size()-1));
    Graph[x].push_back(my_edge_tree(last, 0, Graph[last].size()));
    Graph[last].push_back(my_edge_tree(x, 0, Graph[x].size()-1));
    size++;
    return;
  }
  Vert[size] = root_pr;
  Graph[size].push_back(my_edge_tree(size-1, 0, Graph[size-1].size()));
  Graph[size-1].push_back(my_edge_tree(size, 0, Graph[size].size()-1));
  last = size;
  size++;
  //int counter = 0;
  fori(i, pocz+1, Graph[x].size()-1){
    if((int)Graph[x][i].dest == p){ //znalazles ojca, zapisz jego pozycje
      ppos = i;
      continue;
    }
    Vert[size] = root_pr; //stworz nowy wezel i polacz z nim kolejne dziecko wezla x
    Graph[size].push_back(my_edge_tree(Graph[x][i].dest, 0, Graph[x][i].ctp));
    Graph[Graph[x][i].dest][Graph[x][i].ctp].ctp = Graph[size].size()-1;
    Graph[Graph[x][i].dest][Graph[x][i].ctp].dest = size;
    //polacz nowy wezel z ostatnim wezlem na tworzacej sie sciezce (widocznej na rysunku 3. w zalaczonej pracy; sa to wierzcholki z indeksami
    //(c+1) do (2c-2)
    Graph[size].push_back(my_edge_tree(last, 0, Graph[last].size()));
    Graph[last].push_back(my_edge_tree(size, 0, Graph[size].size()-1));
    size++;
    if((int)Graph[x][Graph[x].size()-1].dest == p && i == (int)Graph[x].size()-3){ // jezeli zostaly tylko 3 wierzcholki i ostatnim 
                                                                                  // jest ojciec to zapisz pozycje ojca i ostatniego dzieck i zakoncz petle
      ppos = Graph[x].size() - 1;
      lpos = Graph[x].size() - 2;
      break;
    }
    if(i == (int)Graph[x].size()-2){ //jezeli zostaly tylko dwa wierzcholki to zapisz pozycje ostatniego dziecka i zakoncz petle
      lpos = Graph[x].size() - 1;
      break;
    }
    Vert[size] = root_pr; //utworz nowy wierzcholek na sciezce
    Graph[size].push_back(my_edge_tree(last, 0, Graph[last].size()));
    Graph[last].push_back(my_edge_tree(size, 0, Graph[size].size()-1));
    last = size++;
  }
  //informacje o ostatnim dzeicku i ojcu
  my_edge_tree tmp = Graph[x][lpos], parent;
  if(ppos != -1)
    parent = Graph[x][ppos];
  Graph[x].clear(); // wyczysc liste sasiedztwa wierzcholka x
  if(ppos != -1){
    Graph[x].push_back(my_edge_tree(parent.dest, 0, parent.ctp));
    Graph[parent.dest][parent.ctp].ctp = 0;
  }
  Vert[size] = root_pr; //stworz kopie wezla x dla jego ostatniego dziecka i polacz ostatnie dziecko z tym wezlem
  Graph[size].push_back(my_edge_tree(tmp.dest, 0, tmp.ctp));
  Graph[tmp.dest][tmp.ctp].dest = size;
  Graph[tmp.dest][tmp.ctp].ctp = Graph[size].size()-1;
  Graph[size].push_back(my_edge_tree(x, 0, Graph[x].size())); //polacz nowy wezel z wezlem x
  Graph[x].push_back(my_edge_tree(size, 0, Graph[size].size()-1));
  //polacz wezel x z ostatnim wezlem na sciezce
  Graph[x].push_back(my_edge_tree(last, 0, Graph[last].size()));
  Graph[last].push_back(my_edge_tree(x, 0, Graph[x].size()-1));
  size++;
}
  
void TreeDecomposition::append_del_add_vert(int x, int y, int posy, int posx){
  auto px = Vert[x].get_iterators();
  auto py = Vert[y].get_iterators();
  auto ix = px.first, iy = py.first;
  std::vector<int> del, add;
  std::set<K> ds;
  int neighbour = y;
  //jezeli znajdziesz wierzcholek w wezle y, ktrego nie ma w wezle x, to 
  //go dodaj do wierzcholkow usuwanych
  //jezeli znajdziesz wierzcholek w wezle x, ktorgo nie ma w wezle y, to
  //go dodaj do wierzcholkow dodanych
  while(ix != px.second || iy != py.second){
    if(ix == px.second && iy != py.second){
      del.push_back(*iy); iy++;
      continue;
    }
    if(ix != px.second && iy == py.second){
      add.push_back(*ix); ix++; 
      continue;
    }
    if(*ix > *iy){
      del.push_back(*iy);
      iy++;
    }
    else{
      if(*ix < *iy){
        add.push_back(*ix);
        ix++;
      }
      else{
        ix++; iy++;
      }
    }
  }
  //stworz kopie zbioru y;
  for(auto it = py.first; it != py.second; it++){
    ds.insert(*it);
  }
  int w = del.size() + add.size();
  if(w == 0){ // wezly sa indentyczne, wiec usun y(dziecko)
    fori(i, 0, Graph[y].size()){
      K s = Graph[y][i].dest, ct = Graph[y][i].ctp;
      if(s == (K)x || s == (K)-1){ // ustaw, ze nie istnieje juz taka krawedz
        Graph[x][posy].dest = -1;
        continue;
      } 
      Graph[s][ct].dest = x; //zmien krawedz (s,y) na (s,x)
      Graph[s][ct].ctp = Graph[x].size();
      Graph[x].push_back(Graph[y][i]);
    }
    Graph[y].clear();
    return;
  }
  if(w == 1){ //wezly roznia sie tylko na jednym elemencie, zakoncz
    return;
  }
  fori(i, 0, w-1){
    if(i < (int)del.size()){ //jezeli przetwarzasz wieszcholki zapominane, to z ds usun wierzcholek del[i]
      ds.erase(del[i]);
    }
    else{ //przegladasz wierzcholki wprowadzane, wiec dodaj do ds, te wierzcholki
      ds.insert(add[i-del.size()]);
    }
    Vert[size] = TreeDecompositionVertex(ds); //stworz nowy wiezel ze zbioru ds
    if(neighbour != y){ //dodaj krawedzie pomiedzy dwoma ostatnio dodanymi wezlami
      Graph[size].push_back(my_edge_tree(neighbour, 0, Graph[neighbour].size()));
      Graph[neighbour].push_back(my_edge_tree(size, 0, Graph[size].size()-1));
    }
    else{ //jezeli jest to pierwszy dodany wezel, to polacz go z y(dzieckiem)
      Graph[size].push_back(my_edge_tree(y, 0, posx));
      Graph[y][posx].dest = size;
      Graph[y][posx].ctp = Graph[size].size()-1;
    }
    neighbour = size;
    size++;
  }
  if(neighbour != y){ //polacz ostatnio dodany wezel z wezlem x
    Graph[neighbour].push_back(my_edge_tree(x, 0, posy));
    Graph[x][posy].dest = neighbour;
    Graph[x][posy].ctp = Graph[neighbour].size()-1;
  }
}

void TreeDecomposition::fill_the_map(std::map<para, K>::iterator ibeg, std::map<para, K>::iterator iend){
  for(auto it = ibeg; it != iend; it++){
    Edge_Map.insert(*it);
  }
}
  
K TreeDecomposition::compute_independent_set(){ //oblicz maksymalny zbior niezalezny
  if(computed_m_i_s) return max_set;
  ind_set_dfs(root, root);
  max_set = Vert[root].find_largest();
  computed_m_i_s = true;
  return max_set;
}
  
bool TreeDecomposition::is_indep_with_special(K x, K s, K num, K s_num){
  std::vector<K> A;
  K a, b,spec = Vert[x].get_special_vert(); //spec - wierzcholek specjalny wierzcholka x
  b = spec;
  Vert[s].make_set_out_of_index(s_num, A);
  //sprawdz czy zbior s_num jest niezalezny
  if(!Vert[s].is_indep(s_num)) return false;
  //jezeli s_num jest niezaleny, to sprawdz, czy nie niezalezny razem z wierzcholekim specjalnym x
  fori(i, 0, A.size()){
    a = A[i];
    b = spec;
    if(a > b) swap(a, b);
    //jezeli istnieje krawedz pomiedzy wierzcholkiem spec a wierzlkiem ze zbioru s_num
    if(Edge_Map.find(std::make_pair(a, b)) != Edge_Map.end()){return false;}
  }
  return true;
}
   
K TreeDecomposition::add_bit_to_number(K num, K x, K son){
  K inds = Vert[son].get_pos_of_vert(Vert[x].get_special_vert());
  K tmp = (1<<(inds))-1;
  K tmp2 = ~tmp;
  K low_part = num & tmp;
  K upper_part = (num & tmp2) << 1;
  K special_vert_val = (1 << inds);
  return ((low_part | special_vert_val) |upper_part);
}
  
K TreeDecomposition::del_bit_from_number(K num, K x, K son, K inds){
  if(inds == (K)0) return num >> 1;
  K tmp = (1<<(inds))-1;
  K tmp2 = (1<<(inds+1)) -1 ;
  tmp2 = ~tmp2;
  K low_part = num & tmp;
  K upper_part = (num & tmp2) >> 1;
  return (low_part | upper_part);
}
  
void TreeDecomposition::ind_set_dfs(K x, K p){
  if((Graph[x].size() == 1 && Graph[x][0].dest == p) ||(Graph[x].size() == 0 && x == root) ){
    Vert[x].allocate_memory();
    Vert[x].set_max_indep(0, 0); //zbior pusty max ind 0
    Vert[x].set_max_indep(1, 1); // zbior 1el max ind 1
    Vert[x].set_is_indep(0, true);
    Vert[x].set_is_indep(1, true);
    return;
  }
  if(Vert[x].get_type() == 0){ // wezel jest wezlem zapominajacym
    K son_ind = (Graph[x][0].dest == p) ? (K)1 : (K)0;
    K son = Graph[x][son_ind].dest;
    ind_set_dfs(son, x);
    Vert[x].allocate_memory(); //zaalokuj pamiec w wezle x
    K inds = 1 << Vert[son].get_pos_of_vert(Vert[x].get_special_vert()); //liczba z zapalonym bitem wezla specjalnego w dziecku
    fori(i, 0, Vert[x].get_num_of_sets()){
      K set_with_special = add_bit_to_number(i, x, son);//liczba odpowiedzialna za zbior A po zwiekszeniu liczby bitow
      //ustaw dla zbioru i maksymalna wartosc ze zbioru z wezlem specjalnym i bez niego
      Vert[x].set_max_indep(i, std::max(Vert[son].max_indep(set_with_special ^ inds), Vert[son].max_indep(set_with_special)));
      if(Vert[son].is_indep(set_with_special^ inds)){
        Vert[x].set_is_indep(i, true);
        continue;
      }
      Vert[x].set_is_indep(i, false);
    }
    Vert[son].deallocate_memory();
  }
  if(Vert[x].get_type() == 1){ //wezel wprowadzajacy
    K son_ind = (Graph[x][0].dest == p) ? (K)1 : (K)0;
    K son = Graph[x][son_ind].dest;
    K bit_special = (1 << Vert[x].get_pos_of_vert(Vert[x].get_special_vert()));
    ind_set_dfs(son, x);
    Vert[x].allocate_memory();
    fori(i, 0, Vert[x].get_num_of_sets()){
      K set_out_special = del_bit_from_number(i, x, son, Vert[x].get_pos_of_vert(Vert[x].get_special_vert()));
      if((i & bit_special) == 0){ //jezeli v nie nalezy do i to przepiszy wynik z syna
        Vert[x].set_max_indep(i, Vert[son].max_indep(set_out_special));
        Vert[x].set_is_indep(i, Vert[son].is_indep(set_out_special));
        continue;
      }
      else{
        if(is_indep_with_special(x, son, i, set_out_special)){ //jezeli specjalny wezel nalezy i zbior jest niezalezny, to wylicz nowa wartosc
          Vert[x].set_is_indep(i, true);
          Vert[x].set_max_indep(i, 1 + Vert[son].max_indep(set_out_special));
        }
        else{
          Vert[x].set_is_indep(i, false);
          Vert[x].set_max_indep(i, MIN_INF);
        }
      }
    }
    Vert[son].deallocate_memory();
  }
  if(Vert[x].get_type() == 2){ //wezel laczacy
    K son[2], ind = 0;
    fori(i, 0, Graph[x].size()){
      if(Graph[x][i].dest != p){
        son[ind] = Graph[x][i].dest; // zapamietaj indeksy synow w liscie sasiedztwa
        ind_set_dfs(son[ind++], x);
      }
    }
    Vert[x].allocate_memory();
    fori(i, 0, Vert[x].get_num_of_sets()){
      if(!Vert[son[0]].is_indep(i) || !Vert[son[1]].is_indep(i)){ //zbior i nie jest zbiorem niezaleznym
        Vert[x].set_is_indep(i, false);
        Vert[x].set_max_indep(i, MIN_INF);
      }
      else{//wylicz wartosc dla zbioru i
        Vert[x].set_is_indep(i, true);
        Vert[x].set_max_indep(i, (Vert[son[0]].max_indep(i) + Vert[son[1]].max_indep(i) - Vert[x].get_size_of_set(i)));
      }
    }
    fori(i, 0, 2){
      Vert[son[i]].deallocate_memory();
    }
  }
}  
