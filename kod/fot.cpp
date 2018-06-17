#include "defines.hpp"

void my_graph::test_for_planarity_and_compute_embedding(){
  if (!boyer_myrvold_planarity_test(boyer_myrvold_params::graph = g,
                                   boyer_myrvold_params::embedding = &embedding_copy[0]
                                   ) )
  {
    std::cout << "ERROR. INPUT GRAPH IS NOT PLANAR" << std::endl;
    std::exit(2);
  }
  e_index = get(edge_index, g_copy);
  edge_count = 0;
  for(tie(ei, ei_end) = edges(g_copy); ei != ei_end; ++ei)
    put(e_index, *ei, edge_count++);
  
}

void my_graph::make_embedding_map(){ //tworzenie mapy, w ktorej dla danej krawedzi zapisane sa jej pozycje w embeddingu
                                      // jej koncow
  fori(i, 0, SIZE){
    fori(j, 0, embedding_copy[i].size()){
      auto it = embedding_copy[i][j];
      K p = source(it, g_copy), q = target(it, g_copy);
      if(p < q){
        auto gt = Change_edge_map.find(std::make_pair(p, q));
        if((K)i == p)
          gt->second.first = j;
        else
          gt->second.second = j;
      }
      else{
        auto gt = Change_edge_map.find(std::make_pair(q, p));
        if((K)i == q)
          gt->second.first = j;
        else
          gt->second.second = j;
        
      }
    }
  }
}


void my_graph::make_connect(){
  make_connected(g);  
  //obliczenie indeksu krawedzi
  e_index = get(edge_index, g);
  edge_count = 0;
  for(tie(ei, ei_end) = edges(g); ei != ei_end; ++ei)
    put(e_index, *ei, edge_count++);
  //obliczenie embeddingu
  if (!boyer_myrvold_planarity_test(boyer_myrvold_params::graph = g,
                                   boyer_myrvold_params::embedding = 
                                       &embedding[0]
                                   )
      ){
    std::cout << "ERROR CONNECTED GRAPH IS NOT PLANAR" << std::endl;
    std::exit(2);
  }
}

void my_graph::make_biconnect(){
  make_biconnected_planar(g, &embedding[0]);
  //oblicznie indeksu krawedzi
  edge_count = 0;
  for(tie(ei, ei_end) = edges(g); ei != ei_end; ++ei)
    put(e_index, *ei, edge_count++);
  //obliczanie embeddingu
  if (!boyer_myrvold_planarity_test(boyer_myrvold_params::graph = g,
                                   boyer_myrvold_params::embedding = 
                                       &embedding[0]
                                   )
      ){
    std::cout << "ERROR BICONNECTED GRAPH IS NOT PLANAR" << std::endl;
    std::exit(2);
  }

}

void my_graph::make_maxi_connect(){
  make_maximal_planar(g, &embedding[0]);
  //obliczenie indeksu krawedzi
  edge_count = 0;
  for(tie(ei, ei_end) = edges(g); ei != ei_end; ++ei)
    put(e_index, *ei, edge_count++);
    
  //obliczenie embeddingu
  if (!boyer_myrvold_planarity_test(boyer_myrvold_params::graph = g,
                                   boyer_myrvold_params::embedding = 
                                       &embedding[0]
                                   )
      ){
    std::cout << "ERROR MAXI_PLANAR IS NOT PLANAR" << std::endl;
    std::exit(2);
  }
  
}

//dla krawedzi (p, q) oblicz nastepna krawedz. Nastepna krawedzia jest
//krawedz, ktora w embeddingu wierzcholka q jest kolejna po krawedzi (p,q) 
//w mapie sprawdz na jakiej pozycji jest krawedz (p,q) i wez jej nastepnik
//i zapisz pod zmienna p zmienna q, a pod zmienna q nastepny wierzcholek
void my_graph::compute_next_edge(K* p, K* q){
  K tmp_p = *p, tmp_q = *q;
  int pos_q;
  if(tmp_p > tmp_q){ 
    auto cmt = Change_edge_map.find(std::make_pair(tmp_q, tmp_p));
    pos_q = cmt->second.first;
  }
  else{
    auto cmt = Change_edge_map.find(std::make_pair(tmp_p, tmp_q));
    pos_q = cmt->second.second;
  }
  pos_q = (pos_q+1)%embedding_copy[*q].size();
  auto tmp = embedding_copy[*q][pos_q];
  *p = (target(tmp, g_copy) == *q) ? source(tmp, g_copy): target(tmp, g_copy);
  K ax = *p;
  *p = *q;
  *q = ax;
}

void my_graph::compute_outer_faces(){ 
  //oblicznie spojnych skladowych
  std::vector<int> component(SIZE);
  int num_of_components = connected_components(g_copy, &component[0]);
  
  components.clear();
  for(int i = 0; i < num_of_components; i++){
    components.push_back(vertex_storage_t());
  }
  for(int i = 0; i < (int)component.size(); i++){
    components[component[i]].push_back(i);
  }  
  vertex_storage_t cur_face; //wierzcholki na obecnie obliczanej scianie
  int cur_comp = -1; //obecna spojna skladowa
  //dodaj kazda ze scian zewnetrznych do zbioru zewnetrznych wierzcholkow
  vertex_storage_t next_outerface;
  while((++cur_comp) < num_of_components){
    if((int)components[cur_comp].size() == 0) continue;
    if((int) components[cur_comp].size() == 1){
      of_storage.push_back(next_outerface);
      continue;
    }
    //wez pierwszy wierzcholek ze spojnej skladowej i jego pierwszego sasiada w embeddingu i bedzie to krawedz na naszej scianie
    vert_desc vert_l = components[cur_comp][0], vert_s;
    vert_s = (source(embedding_copy[vert_l][0], g_copy) == vert_l) ? target(embedding_copy[vert_l][0], g_copy): source(embedding_copy[vert_l][0], g_copy);
    K p = vert_l, q = vert_s;
    next_outerface.push_back(vert_s);
    compute_next_edge(&p, &q); //oblicz nastepna krawedz na wybranej scianie
    while((p != vert_l || q!= vert_s)){ //dopoki nie przejdziemy jeszcze raz po pierwszej krawedzi, przechodz przez kolejne krawedzie
      next_outerface.push_back(q);
      compute_next_edge(&p, &q);
    }
    of_storage.push_back(vertex_storage_t());
    fori(i, 0, next_outerface.size()){
      of_storage[of_storage.size()-1].push_back(next_outerface[i]);
    }
    next_outerface.clear();
  }
} 

//dla kazdej krawedzi oblicz najwczesniejsza warstwe do ktorej ona nalezy
//dla kazdej warstwy zapamietaj krawedzie, ktore naleza do sciany zewnetrznej danej warstwy
void my_graph::compute_layers(){
  fori(i, 0, max_face_age+1){
    f_stor.push_back(std::vector<K>());
  }
  std::map<para, int>::iterator it;
  fori(i, 0, input_faces.size()){
    int age = FACES_AGE[i];
    K p, q;
    fori(j, 0, input_faces[i].size()){
      p = input_faces[i][j];
      q = input_faces[i][(j+1)%input_faces[i].size()];
      f_stor[age].push_back(p);
      K a = p, b = q;
      if(a > b) swap(a,b);
      it = Edge_Map.find(std::make_pair(a, b));
      if(it == Edge_Map.end()){
        Edge_Map.insert(std::make_pair(std::make_pair(a, b), age));
      }
      else{
        if(it->second > age){
          it->second = age;
        }
      }
    }
    f_stor[age].push_back(input_faces[i][0]);
  }
  clear_embedding_map();
}

void my_graph::display_edges(int pocz, int kon){
  for(auto it = Graph_Map.begin(); it != Graph_Map.end(); it++){
    K p = it->first.first;
    K q = it->first.second;
    if(VERT_AGE[p] < pocz || VERT_AGE[p] > kon) continue;
    if(VERT_AGE[q] < pocz || VERT_AGE[q] > kon) continue;
    std::cout<<p<<" "<<q<<std::endl;
  }
}


