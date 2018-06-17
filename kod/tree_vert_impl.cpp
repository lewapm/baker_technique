#include "tree_vert_defs.hpp"
//definicje skladowej klasy TreeDecompositionVertex
void TreeDecompositionVertex::add_vertice(K x){
  if(Graph.find(x) == Graph.end()){
    Graph.insert(x);
    size++;
  }
}
  
void TreeDecompositionVertex::del_vertice(K x){
  std::set<K>::iterator it;
  if((it = Graph.find(x)) != Graph.end()){
    Graph.erase(it);
    size--;
  }
}  
  
Kl TreeDecompositionVertex::find_largest(){
  Kl res = MIN_INF;
  K siz = (1<<size);
  fori(i, 0, siz){
    if(Max_indep[i] > res) res = Max_indep[i];
  }
  return res;
}
  
void TreeDecompositionVertex::allocate_memory(){
  Is_indep = new bool[MAX_AMOUNT_OF_SETS];
  Max_indep = new Kl[MAX_AMOUNT_OF_SETS];
}
  
void TreeDecompositionVertex::deallocate_memory(){
  delete[] Is_indep;
  delete[] Max_indep;
}
  
void TreeDecompositionVertex::assign_number_to_vertex(){
  for(auto it = Graph.begin(); it != Graph.end(); it++){
    Vert_map.insert(std::make_pair(*it, Vert_map.size()));
    Index_vec.push_back(*it);
  }
}
  

K TreeDecompositionVertex::get_size_of_set(K num){
  K res = 0;
  fori(i, 0, Graph.size()){
    if((num & (1<<i)) != 0)
      ++res;
  }  
  return res;
}
  
void TreeDecompositionVertex::make_set_out_of_index(K num, std::vector<K>& A){
  fori(i, 0, Graph.size()){
    if((num & (1<<i)) != 0){
      A.push_back(Index_vec[i]);
    }
  }
}
   
void TreeDecompositionVertex::display(){
  for(auto it = Graph.begin(); it != Graph.end(); it++){
    std::cout<<*it<<" ";
  }
  std::cout<<std::endl;
}
