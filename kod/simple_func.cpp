#include "defines.hpp"

//wpisz wartosci poczatkowe do tablic
void my_graph::prepare(){
  f_stor = faces_storage();
  of_storage = faces_storage();
  for(int i = 0; i < MAX_SIZE; i++){
    FACES_AGE[i] = VERT_AGE[i] = -1;
    //VIS[i] = -1;
    T[i] = -1;
    Level[i] = -1;
  }
  fori(i, 0, MAX_NUM_OF_LAYERS){
    fori(j, 0, MAX_SIZE)
      STR_GRAPH[i][j].clear();
  }
  for(int i = 0; i < MAX_SIZE; i++){
    P[i][0] = i;
    for(int j = 1; j < 20; j++)
      P[i][j] = -1;
  }
}


//wczytaj liczbe krawedzi i krawedzi
//oraz dodaj je do grafow i map, ktore ke przchowuja
void my_graph::add_edges(){
  K a, b, m;
  std::cin>>m;
  for(int i = 0; i < (int)m; i++){
    std::cin>>a>>b;
    if(a > b) swap(a,b);
    add_edge(a, b, g);
    add_edge(a, b, g_copy);
    add_edge(a, b, g_constant);
    Graph_Map.insert(std::make_pair(std::make_pair(a, b), 0));
    Change_edge_map.insert(std::make_pair(std::make_pair(a, b), std::make_pair(0,0)));
  }
}

//wyczysc drzewo, wyczysc jego sasiedztwo i zbiory w wierzcholkach dekompozycji drzewowej
void my_graph::clear_tree(){
  Edge_Map.clear();
  fori(i, 0, tree_size){
    Tree[i].clear();
    TDV[i].clear();
  }
}

//sprawdz czy wierzcholek lezy w interesujacych nas warstwach 
bool my_graph::check_if_in_layers(int x, int beg, int end){
  if(VERT_AGE[x] < beg || VERT_AGE[x] > end-1) return false;
  return true;
}

void my_graph::refresh_g(){
  fori(i, 0, SIZE){
    embedding[i].clear();
    clear_vertex(i, g);
  }
  for(boost::tie(ei, ei_end) = edges(g_constant); ei != ei_end; ++ei){
    add_edge(source(*ei,g_constant), target(*ei, g_constant), g);
  }
  
  boyer_myrvold_planarity_test(boyer_myrvold_params::graph = g,
                                   boyer_myrvold_params::embedding = &embedding[0]); 
  
}

//pozostale funkcje wypisuja zawartosci roznych tablic
void my_graph::display_of_storage(){
  std::cout<<"of_storage\n";
  for(int i = 0; i < (int)of_storage.size(); i++){
    fori(j, 0, of_storage[i].size()){
      std::cout<<of_storage[i][j]<<" ";
    }
    std::cout<<"\n";
  }
  
}

void my_graph::display_VERT_AGE(){
  std::cout<<"vertex age:\n";
  int counter = 0;
  int licznik = 0;
  while(counter < SIZE){
    std::cout<<licznik<<std::endl;
    for(int i = 0; i < SIZE; i++){
      if(VERT_AGE[i] == licznik){
        std::cout<<i<<": "<<VERT_AGE[i]<<" ";
        counter++;
      }
    }
    licznik++;
    std::cout<<"\n";
    
  }
  for(int i = 0; i < SIZE; i++){
    std::cout<<i<<": "<<VERT_AGE[i]<<" ";
  }
  std::cout<<"\n";
}

void my_graph::display_FACES_AGE(){
  std::cout<<"faces:\n";
  for(int i = 0; i < (int)input_faces.size(); i++){
    std::cout<<i<<": "<<FACES_AGE[i]<<" ";
  }  
  std::cout<<"\n";
}

void my_graph::display_NEIGHBOURS(){
  std::cout<<"sasiedzi:\n";
  for(int i = 0 ; i < SIZE; i++){
    std::cout<<i<<": "<<NEIGHBOURS[i].first.first<<" "<<NEIGHBOURS[i].second.first<<"\n";
  }
}

void my_graph::display_vert_map(){
  std::cout<<"VERT MAP"<<std::endl;
  for(auto it = vert_map.begin(); it != vert_map.end(); it++){
    std::cout<<(*it).first<<" "<<(*it).second<<std::endl;
  }
}

void my_graph::display_auxiliary_graph(){
  std::cout<<"auxiliary graph"<<std::endl;
  for(int i = 0; i < auxiliary_gr_size; i++){
    std::cout<<i<<" "<<V[i].size()<<": ";
    for(int j = 0; j < (int)V[i].size(); j++){
      std::cout<<V[i][j].t<<" ";
    }
    std::cout<<std::endl;
  }
}

void my_graph::display_embedding(){
  std::cout<<"EMBEDDING"<<std::endl;
   for(int i = 0; i < (int)num_vertices(g); i++){
    std::cout<<i<<": ";
    for(auto it = embedding[i].begin(); it != embedding[i].end(); it++)
      std::cout<<*it<<" ";
    std::cout<<"\n";
  }
}
void my_graph::display_embedding_copy(){
  std::cout<<"EMBEDDING COPY"<<std::endl;
   for(int i = 0; i < (int)num_vertices(g); i++){
    std::cout<<i<<": ";
    for(auto it = embedding_copy[i].begin(); it != embedding_copy[i].end(); it++)
      std::cout<<*it<<" ";
    std::cout<<"\n";
  }
}

void my_graph::clear_input_graph_g(){
  fori(i, 0, SIZE){
    clear_vertex(i, g);
  }
  for(int i = SIZE-1;i >= 0; i--){
    remove_vertex(i, g);
  }
}

void my_graph::clear_input_graph_g_copy(){
  fori(i, 0, SIZE){
    clear_vertex(i, g_copy);
  }
  for(int i = SIZE-1;i >= 0; i--){
    remove_vertex(i, g_copy);
  }
}

void my_graph::display_input_faces(){
  std::cout<<"input_faces"<<std::endl;
  fori(i, 0, input_faces.size()){
    fori(j, 0, input_faces[i].size()){
      std::cout<<input_faces[i][j]<<" ";
    }
    std::cout<<std::endl;;
  }
}

void my_graph::display_layers(){
  std::cout<<"LAYERS"<<std::endl;
  fori(i, 0, f_stor.size()){
    fori(j, 0, f_stor[i].size()){
      std::cout<<f_stor[i][j]<<" ";
    }
    std::cout<<std::endl;
  }
  
}

void my_graph::display_f_stor(){
  std::cout<<"F_STOR"<<std::endl;
  fori(i, 0, f_stor.size()){
    std::cout<<"warstwa "<<i<<": ";
    fori(j, 0, f_stor[i].size()){
      std::cout<<f_stor[i][j]<<" ";
    }
    std::cout<<"\n";
  }
}

void my_graph::display_STR_GRAPH(){
  fori(i, 0, f_stor.size()+1){
    std::cout<<"warstwa "<<i<<":"<<std::endl;;
    fori(j, 0, auxiliary_gr_size){
      std::cout<<j<<": "<<std::endl;;
      fori(k, 0, STR_GRAPH[i][j].size()){
        std::cout<<STR_GRAPH[i][j][k].dest<<"("<<STR_GRAPH[i][j][k].ctp<<") ";
      }
      std::cout<<std::endl;;
    }
  }
}

//sprawdzamy, czy graf jest zewnetrznie planarny
//bedzie tak, jezeli istnieje sciana na ktorej beda lezaly wszystkie wierzcholki
bool my_graph::check_if_is_1_outerface(){
  int tmpsize = 0;
  int tmp[SIZE];
  fori(j, 0, SIZE){
    tmp[j] = 0;
  }
  fori(i, 0, input_faces.size()){
    tmpsize=0;
    fori(j, 0, input_faces[i].size()){
      if(tmp[input_faces[i][j]] == 0){
        tmp[input_faces[i][j]] = 1;
        tmpsize++;
        if(tmpsize == SIZE){ 
          return true;
        }
      }
    }
    fori(j, 0, SIZE){
      tmp[j] = 0;
    }
  }
  return false;
}
