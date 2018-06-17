#include "defines.hpp"
//sprawdz czy graf jest 1-zewnetrznie planarny
bool is_1_outer(my_graph& gr){
  gr.initialize_edge_index();
  gr.planar_graph_traverse(); //przejdz po scianach grafu
  if(gr.check_if_is_1_outerface()){
    gr.make_td_for_1_outerplanar(); //jezeli graf jest 1-zewnetrznie planarny, to oblicz jego dekompozycje w inny sposob
    return true;
  }
  return false;  
}

//stworz dekompozycje drzewowa dla warstw pomiedzy warstwami beg a end i wylicz w nich maksymalny zbior niezalezny 
int td_on_layers(my_graph& gr, int beg, int end){
  gr.make_tree(beg, end); //stworz drzewo
  gr.make_tree_decomposition(beg, end); //na podstawie drzewa stworz dekompozycje drzewowa
  TreeDecomposition& t = gr.get_tree_decomposition(); 
  t.make_nice(); //z dekompozycji drzewowej zrob ladna dekompozycje drzewowa
  gr.clear_tree();
  return t.compute_independent_set();
}

int baker_technique(my_graph& gr, int acc){ //acc - dokladnosc
  if(acc >= gr.get_num_of_layers()){
    return td_on_layers(gr, 0, gr.get_num_of_layers());
  }
  int result = 0;
  fori(i, 0, acc){
    int tmp = 0;
    if(i > 0){
      int x = td_on_layers(gr, 0, i);
      tmp += x;
    }
    for(int j  = i; j < gr.get_num_of_layers(); j += acc){
      if(j + 1 >= gr.get_num_of_layers()) continue;
      int x = td_on_layers(gr, j+1, std::min(j+acc, gr.get_num_of_layers()));
      tmp += x;
      gr.clear_tree();
    }
    result = std::max(result, tmp);
  }
  return result;
}

int main(int argc, char** argv)
{
  int n;
  std::cin>>n; // wczytaj ilosc wierzcholkow grafu
  my_graph gr(n); 
  gr.add_edges(); //wczytaj pozostala zawartosc grafu
  gr.prepare(); //przygotuj tablice
  if(is_1_outer(gr)){ //oblicz maksymlany zbior niezalezny dla grafu 1-zewnetrznie planarnego
    TreeDecomposition& t = gr.get_tree_decomposition();
    t.make_nice();
    std::cout<<t.compute_independent_set()<<std::endl;;  
  }
  else{
    gr.refresh_g(); //ustaw g na wartosci poczatkowe
    gr.test_for_planarity_and_compute_embedding(); //oblicz embedding
    gr.make_embedding_map(); //stworz mape dla krawedzi z embeddingu
    gr.compute_outer_faces(); //oblicz sciane zewnetrzna
      
    //gr.refresh_g();
    gr.compute_faces_to_which_vertex_belongs(); 
    
    gr.check_which_faces_are_outer(); //oblicz, dla wierzcholka do jakich scian nalezy
    gr.compute_VERT_AGE(); //oblicz wartwy wierzcholkow i scian
    gr.compute_youngest_face_for_each_vertice();
    gr.compute_neighbours_on_youngest_face();
    gr.create_auxiliary_graph(); //stworz pomocniczy graf, potrzebny podczas rozszerzania wierzcholkow
    gr.compute_layers(); //oblicz warstwy
    gr.create_stripping_graph(); //stworz dla kazdej warstwy graf, przez nia tworzony
    
    gr.change_vertices_with_degree_greater_than_3(); //rozszerz wierzcholki
    gr.modify_stripping_graph(); //dodaj do grafu warstw rozszerzajace wierzcholki i usun wierzcholki rozszerzane
    //oblicz dekompozycje drzewowa i maks_ind_set oraz oblicz aproksymacje technika bakera
    int val = td_on_layers(gr, 0, gr.get_num_of_layers()), tmp = 0;
    std::cout<<val<<std::endl;
    if(ACCURACY*(tmp = baker_technique(gr, ACCURACY)) < (ACCURACY-1)*val){
      std::cout<<"BLAD"<<std::endl;
      std::cout<<val<<" "<<tmp<<std::endl;
    }
  }
  return 0;
}

