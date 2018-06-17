#include "common_defs.hpp"
#include "tree_vert_defs.hpp"
#include "defines.hpp"

struct pos{
  K origin, first, second;
  pos(K origin, K first, K second): origin(origin), first(first), second(second){
  }
};

 
bool my_graph::is_edge(K x, K y){
  fori(i, 0, V[x].size()){
    if(V[x][i].t == y) return true;
  }
  return false;
}

//jezeli stopien wierzcholka jest co najwyzej 2, to przejdz po jego liscie sasiedztwa
//i podaj wierzcholki, ktore jeszcze nie zostaly przetworzone przez algorytm
//oraz zmien ich rzeczywisty stopien, po usunieca wierzcholka x
para my_graph::find_vertices(int x, int* DEG){
  K a = K_MAX, b = K_MAX;
  fori(i, 0, V[x].size()){
    if(V[x][i].used == 0){
      if(a == K_MAX) a = V[x][i].t;
      else b = V[x][i].t;
      V[x][i].used = 1;
      V[V[x][i].t][V[x][i].c].used = 1;
      DEG[x]--;
      DEG[V[x][i].t]--;
    }
  }
  return std::make_pair(a, b);
}

//stworz pomocniczy graf V, ktory ma dla kazdej krawedzi zapisane, gdzie jest jej opowiednik
//na liscie sasiedztwa drugiego jej konca
void my_graph::make_v_for_1_outerplanar(){
  //policz spojne skladowe i dla kazdej policz wierzcholki, ktore do niej naleza
  std::vector<int> component(SIZE);
  int num_of_components = connected_components(g_constant, &component[0]);
  
  components.clear();
  for(int i = 0; i < num_of_components; i++){
    components.push_back(vertex_storage_t());
  }
  for(int i = 0; i < (int)component.size(); i++){
    components[component[i]].push_back(i);
  }  
  //przejdz po krawedziach grafu i stworz graf pomocniczy V
  for(tie(ei, ei_end) = edges(g_constant); ei != ei_end; ++ei){
    K a = source(*ei, g_constant);
    K b = target(*ei, g_constant); 
    V[a].push_back(my_edge(b, V[b].size()));
    V[b].push_back(my_edge(a, V[a].size()-1));
    Edge_Map.insert(std::make_pair(std::make_pair(a, b), 0));
    Edge_Map.insert(std::make_pair(std::make_pair(b, a), 0));
  }
  
}

//obliczanie dekompozycji drzewowej dla grafu 1-zewnetrznie planarnego, 
//zgodnie z algorytmem opisanym w pracy
void my_graph::make_td_for_1_outerplanar(){
  
  int DEG[MAX_SIZE]; //rzeczywisty stopien wierzcholka
  std::queue<K> two_degree; //kolejka wierzcholkow o stopniu co najwyzej 2
  std::stack<pos> peeling_stack; //krawedzi, ktore kolejno usuwamy z grafu
  make_v_for_1_outerplanar(); //stworz pomocniczy graf
  
  //przejdz po skladowych i dla kazdego wierzcholka zapisz jego stopien,
  //dodaj wierzcholki o stopniu co najwyzej 2 na kolejke two_degree
  fori(j, 0, components.size()){
    fori(k, 0, components[j].size()){
      int i = components[j][k];
      DEG[i] = V[i].size();
      if(V[i].size() > 2) continue;
      two_degree.push(i);
    } 
    
    //dopoki istnieje nieprzerobiony wierzcholek stopnia co najwyzej 2
    while(!two_degree.empty()){
      K x;
      x = two_degree.front();
      two_degree.pop();
      if(DEG[x] == 2){ // jezeli stopien usuwanego wierzchoka wynosi 2, to 
        paraK c = find_vertices(x, DEG);
        pos add_x(x, c.first, c.second);
        peeling_stack.push(add_x); //dodaj, ze usuwamy wierzcholek x i zapisz jego aktualnych sasiadow
        if(Edge_Map.find(std::make_pair(c.first, c.second)) == Edge_Map.end()){ //jezeli pomiedzy sasiadami wierzcholka x
          Edge_Map.insert(std::make_pair(std::make_pair(c.first, c.second), 0)); //nie ma krawedzi to ja dodaj i zwieksz ich stopien o 1
          Edge_Map.insert(std::make_pair(std::make_pair(c.second, c.first), 0));
          V[c.first].push_back(my_edge(c.second, V[c.second].size()));
          V[c.second].push_back(my_edge(c.first, V[c.first].size()-1));
          DEG[c.first]++;
          DEG[c.second]++;
        }
        if(DEG[c.first] == 2){ //jezeli rzeczywisty stopien ktoregos sasiada jest rowny 2 to go dodaj do wektora 
          two_degree.push(add_x.first);
        }
        if(DEG[c.second] == 2){
          two_degree.push(add_x.second);
        }
      }
      if(DEG[x] == 1){ //jezeli usuwany wierzcholek ma tylko jednego sasiada, to ustal jaki to jest wierzcholek
        paraK c = find_vertices(x, DEG);
        pos add_x(x, c.first, NO_VERTEX); //zapisz usuwana krawedz
        peeling_stack.push(add_x);
        if(DEG[c.first] == 2){ //jezeli stopien wierzcholka sie odpowiednio zmniejszyl to do dodaje na kolejke usuwanych wierzchokow
          two_degree.push(add_x.first);
        }
      }
    }
    Edge_Map.clear();

    //na podstawie zawartosci stosu bedziemy tworzyli dekompozycje drzewowa o szerokosci 2
    //jezeli pojawi sie nam nowa krawedz, to w mapie Edge_Map zapisujeemy wezel, 
    //w ktorym ona jest zawarta. Dla nowo pojawiajacego sie wierzcholka, zapisujemy w mapie vert_map, 
    //wezel do ktorego go dodajemy
    tree_size = 0;
    //wez ostatnia przetwarzana krawedz w grafie i stworz wezel zawierajacy jej konce
    pos x = peeling_stack.top();
    peeling_stack.pop();
    Edge_Map.insert(std::make_pair(std::make_pair(x.origin, x.first), tree_size));
    Edge_Map.insert(std::make_pair(std::make_pair(x.first, x.origin), tree_size));
    TDV[tree_size].add_vertice(x.origin);
    TDV[tree_size].add_vertice(x.first);
    vert_map.insert(std::make_pair(x.origin, tree_size));
    vert_map.insert(std::make_pair(x.first, tree_size));
    tree_size++;
    
    while(!peeling_stack.empty()){ //dopoki nie przetworzylismy wszystkich krawedzi i wierzchokow z poczatkowego grafu
      pos x = peeling_stack.top();
      peeling_stack.pop();
      if(x.second == NO_VERTEX){ //jezeli podczas usuwania wierzcholek x.origin mial tylko jednego sasiada
        Edge_Map.insert(std::make_pair(std::make_pair(x.origin, x.first), tree_size)); //zapisz dla przetwarzanej krawedzi wezel
        Edge_Map.insert(std::make_pair(std::make_pair(x.first, x.origin), tree_size)); //do ktorego ona nalezy
        TDV[tree_size].add_vertice(x.origin); //dodaj do niego wierzcholki
        TDV[tree_size].add_vertice(x.first);
        vert_map.insert(std::make_pair(x.origin, tree_size)); //zapisz dla wierzcholka, ktory wezel bedzie jego reprezentantem
        auto it = vert_map.find(x.first); //dodaje wezel do drzewa
        Tree[tree_size].push_back(my_edge_tree((*it).second, 0, Tree[(*it).second].size())); //laczymy go z wezelem, ktory jest reprezentatnem
        Tree[(*it).second].push_back(my_edge_tree(tree_size, 0, Tree[tree_size].size()-1)); //wierzcholka x.first
        tree_size++;
      }
      else{ //podczas usuwania wierzcholka x.origin mial dwoch sasiadow
        auto it = Edge_Map.find(std::make_pair(x.first, x.second)); //stworz nowy wezel
        TDV[tree_size].add_vertice(x.origin); //dodaj kolejne wierzcholki do tego wezla
        TDV[tree_size].add_vertice(x.first);
        TDV[tree_size].add_vertice(x.second);
        vert_map.insert(std::make_pair(x.origin, tree_size)); 
        Tree[tree_size].push_back(my_edge_tree((*it).second, 0, Tree[(*it).second].size())); //polacz nowy wezel z reprezentantem krawedzi
        Tree[(*it).second].push_back(my_edge_tree(tree_size, 0, Tree[tree_size].size()-1)); //(x.first, x.second)
        Edge_Map.insert(std::make_pair(std::make_pair(x.origin, x.first), tree_size)); //zapisz dla krawedzi jej reprezentanta
        Edge_Map.insert(std::make_pair(std::make_pair(x.first, x.origin), tree_size));
        Edge_Map.insert(std::make_pair(std::make_pair(x.origin, x.second), tree_size));
        Edge_Map.insert(std::make_pair(std::make_pair(x.second, x.origin), tree_size));
        tree_size++;
      }
      
    }
  }
  //jezeli nasz graf sklada sie z kilku spojnych skladowych to ich dekompozycje drzewowe polacza w jedna
  K x = components[0][0], y;
  auto xt = vert_map.find(x);
  fori(i, 1, components.size()){
    y = components[i][0];
    auto yt = vert_map.find(y);
    Tree[(*xt).second].push_back((*yt).second);
    Tree[(*yt).second].push_back((*xt).second);
  }
  //stworz obiekt TreeDecomposition dla utworzonej dekompozycji
  make_t_d_object(0);
}
