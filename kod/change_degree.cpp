#include "defines.hpp"
void my_graph::initialize_edge_index(){
  e_index = get(edge_index, g);
  edge_count = 0;
  for(boost::tie(ei, ei_end) = edges(g); ei != ei_end; ++ei)
    put(e_index, *ei, edge_count++);
}

//funkcja ta przechodzi przez wszystkie sciany w grafie i je zapisuje w wektorze wektorow input_faces
void my_graph::planar_graph_traverse(){
  if (!boyer_myrvold_planarity_test(boyer_myrvold_params::graph = g,
                                   boyer_myrvold_params::embedding = 
                                       &embedding[0]
                                   )
      ){
    std::cout << "ERROR graph is not planar" << std::endl;
    exit(1);
  }
  output_visitor v_vis(*this);
  planar_face_traversal(g, &embedding[0], v_vis);
}
//dla kazdego wierzcholka oblicza indeksy scian do ktorych on nalezy
void my_graph::compute_faces_to_which_vertex_belongs(){
  for(int i = 0; i < (int)input_faces.size(); i++){
    for(int j = 0; j < (int)input_faces[i].size(); j++){
      vert_faces[input_faces[i][j]].push_back(std::make_pair(i, j));
    }
  }
}

//dla kazdej sciany sprawdzamy, czy nalezy do sciany zewnetrznej
//sciany zewnetrzne charakteryzuja sie tym, ze sa ich wierzcholki 
//zapisane sa w kolejnosci zgodnej z ruchem wskazowek zegara, jezeli popatrzymy na embedding
//funkcja planar_face_traversal wyznacza sciane zewnetrzna w ten sam sposob, w ktory
//robi to funkcja compute_outer_faces,tzn wierzcholek 0 i jego pierwszy sasiad z embeddingu naleza do sciany zewnetrznej
void my_graph::check_which_faces_are_outer(){
  for(int i = 0; i < (int)of_storage.size(); i++){
    int x = of_storage[i][0], y = of_storage[i][1];
    for(int j = 0; j < (int)vert_faces[x].size(); j++){
      int tmp = input_faces[vert_faces[x][j].first].size(); 
      if((int)input_faces[vert_faces[x][j].first][(vert_faces[x][j].second + 1) % tmp] == y){
        FACES_AGE[vert_faces[x][j].first] = 0;
      }
    }
  }
  for(int i = 0; i < (int)input_faces.size(); i++){
    if(FACES_AGE[i] == 0){
      for(int j = 0; j < (int)input_faces[i].size(); j++){
        visited.push(input_faces[i][j]);
        VERT_AGE[input_faces[i][j]] = 0;
      } 
    }
  }
}

//dla kazdego wierzcholka i dla kazdej sciany wyliczamy warstwe, do ktorej nalezy
//sciana zewnetrzna i wierzcholki na niej lezace naleza do warstwy 0. Nastepnie postepujemy zgodnie z procedura
//jezeli jakis wierzcholek lezy na warstwie i-tej to wszystkie sciany do niego przylegle, ktore jeszcze
//nie maja przydzielonej warstwy, beda nalezaly do warstwy (i+1)-wszej, a wierzchoki, ktore naleza do tych scian
//i ktore jeszcze nie maja przydzielonej warstwy, beda nalezaly do warstwy (i+1)-wszej. 
//Procedure powtarzamy tak dlugo az sa jakies nieodwiedzone sciany lub wierzcholki
void my_graph::compute_VERT_AGE(){
  max_face_age = 0;
  max_vert_age = 0;
  while(!visited.empty()){
    int x = visited.front();
    visited.pop();
    for(auto it = vert_faces[x].begin(); it != vert_faces[x].end(); it++){
      if(FACES_AGE[(*it).first] == -1){
        FACES_AGE[(*it).first] = VERT_AGE[x]+1;
        max_face_age = std::max(max_face_age, FACES_AGE[(*it).first]);
        for(auto fa = input_faces[(*it).first].begin(); fa != input_faces[(*it).first].end(); fa++){
          if(VERT_AGE[*fa] == -1){
            VERT_AGE[*fa] = FACES_AGE[(*it).first];
            max_vert_age = std::max(max_vert_age, VERT_AGE[*fa]);
            visited.push(*fa);
          }
        }
      }
    }
  }
}

//dla kazdego wierzcholka wyznacz sciane z nim sasiadujaca, 
//ktora lezy na najwczesniejszej warstwie
void my_graph::compute_youngest_face_for_each_vertice(){
  for(int i = 0; i < SIZE; i++){
    VERT_YOUNGEST[i] = vert_faces[i][0].first;
    for(int j = 1; j < (int)vert_faces[i].size(); j++){
      if(FACES_AGE[VERT_YOUNGEST[i]] > FACES_AGE[vert_faces[i][j].first]){
        VERT_YOUNGEST[i] = vert_faces[i][j].first;
      }
    }
  }
}

//dla kazdego wystapienia wierzcholka zapisz jego sasiadow na wybranej scianie 
//z najwczesniejszej warstwy
void my_graph::compute_neighbours_on_youngest_face(){
  for(int i = 0; i < (int)input_faces.size(); i++){
    for(int j = 0; j < (int)input_faces[i].size(); j++){
      if(VERT_YOUNGEST[input_faces[i][j]] == i){
        NEIGHBOURS[input_faces[i][j]].first.first = input_faces[i][(input_faces[i].size() + j - 1) % input_faces[i].size()];
        NEIGHBOURS[input_faces[i][j]].second.first = input_faces[i][(j+1) % input_faces[i].size()];
      }
    }
  }
}

//stworz pomocniczy graf V, w ktorym sasiadujace wierzcholki zostana zapisane 
//w kolejnosci embeddingu oraz dla kazdego konca krawedzi zapisujemy, gdzie ta krawedz
//lezy w embeddingu drugiego jej konca
//w mapie pos_map zapisz dla krawedzi jej pozycje embeddingu dla pierwszego konca wierzcholka
//jezeli pojawi sie drugi koniec krawedzi to zapamietaj odpowiednie wartosci w grafie V.
void my_graph::create_auxiliary_graph(){
  for(int i = 0; i < SIZE; i++){
    for(int j = 0; j < (int)embedding[i].size(); j++){
      V[i].push_back(my_edge());
    }
  }
  for(int i = 0; i < SIZE; i++){
    if(embedding[i].size() > 3){ //jezeli wierzcholek ma wiecej niz 3 sasiadow, to musimy go rozszerzyc
      reduce.insert(i);
    }
    for(int j = 0; j < (int)embedding[i].size(); j++){
      auto it = embedding[i][j];
      long unsigned int k = source(it, g), t = target(it, g);
      long unsigned int w = (k == (long unsigned int)i) ? t: k;
      pos_map_iter p;
      if(w == NEIGHBOURS[i].first.first) NEIGHBOURS[i].first.second = j;
      if(w == NEIGHBOURS[i].second.first) NEIGHBOURS[i].second.second = j; 
      V[i][j].t = w;
      if((p = pos_map.find(std::make_pair(k, t))) != pos_map.end()){
        V[i][j].c = (*p).second;
        V[w][(*p).second].c = j;
      }
      else{
        pos_map.insert(std::make_pair(std::make_pair(k, t), j));
      }
    }
  }
  
}

//dla kazdej krawedzi, ktorej konce zostaly zmienione podczas rozszerzania 
//wierzcholko, zapamietaj jakie sa jej nowe wierzcholki koncowe
void my_graph::add_changed_edge(K x, K neighbour, K new_value){
  if(neighbour < (K)SIZE){
    if(x > neighbour)
      Change_edge_map.insert(std::make_pair(std::make_pair(neighbour, x), std::make_pair(neighbour, new_value)));
    else
      Change_edge_map.insert(std::make_pair(std::make_pair(x, neighbour), std::make_pair(new_value, neighbour)));
  }
  else{
    auto it = vert_map.find(neighbour);
    if(x > it->second){
      Change_edge_map.erase(std::make_pair(it->second, x));
      Change_edge_map.insert(std::make_pair(std::make_pair(it->second, x), std::make_pair(neighbour, new_value)));
    }
    else{
      Change_edge_map.erase(std::make_pair(x, it->second));
      Change_edge_map.insert(std::make_pair(std::make_pair(x, it->second), std::make_pair(new_value, neighbour)));
    }
  }
  
}

//przejdz po wszystkich wierzcholkach, ktore maja stopien wiekszy od 3
//dla kazdego z nich tworzy stopien-2 nowe wierzcholki.
//te nowe wierzcholki beda polaczane miedzy soba w sciezke, natomiast
//z krawedzaimy wychodzacymi beda polaczone w takije kolejnosci
//w jakiej prezentuje to Rysunek 1 dolaczonej pracy
void my_graph::change_vertices_with_degree_greater_than_3(){
  int graph_size = SIZE;
  for(auto it = reduce.begin(); it != reduce.end(); it++){
    int x = *it;
    int counter = 0, dir = 1, x_size = V[x].size(); //zaczynamy od pierwszego sasiada 
    //sciany z najwczesniejszej warstwy i idziemy kolejno po krawedziach z embeddingu
    //dir ustala, czy idziemy w prawo czy w lewo po sasiadach
    int j = NEIGHBOURS[x].first.second;
    if((j+1)%x_size == (int)NEIGHBOURS[x].second.second){
      dir = -1;
    }
    //przy kazdym dodawaniu nowego wierzcholka, bedziemy zmieniali
    //wartosci odpowiednikow w grafie V oraz wywolywali funkcje add_chand_edge
    //oraz dla kazdego nowego wierzcholka zapisz jaki wierzcholek on zastepuje
    
    //dodaj pierwsza krawedz, do pierwszego wierzcholka rozszerzajacego
    int n_name = V[x][j].t; //neighbour name
    add_changed_edge(x, n_name, graph_size);
    vert_map.insert(std::make_pair(graph_size, x));
    V[n_name][V[x][j].c].t = graph_size;
    V[n_name][V[x][j].c].c = V[graph_size].size();
    V[graph_size].push_back(V[x][j]);
    
    //dodaj druga krawedz, do pierwszego wierzcholka rozszerzajacego
    j = (x_size+j+dir)%x_size;
    n_name = V[x][j].t;
    add_changed_edge(x, n_name, graph_size);
    V[n_name][V[x][j].c].t = graph_size;
    V[n_name][V[x][j].c].c = V[graph_size].size();
    V[graph_size].push_back(V[x][j]);
    VERT_AGE[graph_size] = VERT_AGE[x];
    while(counter < (int)V[x].size() - 3){ // stworz V[x].size()-3 nowe wierzcholki
      graph_size++;
      vert_map.insert(std::make_pair(graph_size, x));
      j = (x_size+j+dir)%x_size;
      V[graph_size-1].push_back(my_edge(graph_size, V[graph_size].size()));
      V[graph_size].push_back(my_edge(graph_size-1, V[graph_size-1].size()-1));
      n_name = V[x][j].t;
      add_changed_edge(x, n_name, graph_size);
      V[n_name][V[x][j].c].t = graph_size;
      V[n_name][V[x][j].c].c = V[graph_size].size();
      V[graph_size].push_back(V[x][j]);
      VERT_AGE[graph_size] = VERT_AGE[x];
      counter++;
    }
    //ostatni dolaczany wierzcholek polacz z druga krawedza nalezaca do najmlodszej sciany
    j = (x_size+j+dir)%x_size;
    vert_map.insert(std::make_pair(graph_size, x));
    n_name = V[x][j].t;
    add_changed_edge(x, n_name, graph_size);
    V[n_name][V[x][j].c].t = graph_size;
    V[n_name][V[x][j].c].c = V[graph_size].size();
    V[graph_size].push_back(V[x][j]);
    graph_size++;
    auxiliary_gr_size = graph_size;
  }
  //wyczysc sasiedztwo wierzcholkow rozszerzanych
  for(auto it = reduce.begin(); it!= reduce.end(); it++){
    V[*it].clear();
  }
  
}
