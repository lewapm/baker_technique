#include <iostream>
#include <map>
#include <vector>
using namespace std;
typedef pair<int, int> para;
#define mp(a, b) make_pair(a, b)
map<para, int> Edge;
int n;
void make_set(int set, vector<int>& v){
	for(int i = 0; i < n; i++){
		if((set & (1<<i)) != 0)
			v.push_back(i);
	}
}

int sprawdz(int set){
	vector<int> elem;
	make_set(set, elem);
	for(int i = 0; i < (int)elem.size(); i++){
		for(int j = 0; j < (int)elem.size(); j++){
			if(Edge.find(mp(elem[i], elem[j])) != Edge.end()) return 0;
		}
	}
	return (int)elem.size();
}

int main(){
	int m, a, b;
	cin>>n>>m;
	for(int i = 0; i < m; i++){
		cin>>a>>b;
		if(a > b) swap(a, b);
		Edge.insert(mp(mp(a,b), 0));
	}
	int sets = (1 << (n+1))-1;
	int best = 0, res;
	for(int i = 0; i < sets+1; i++){
		res = sprawdz(i);
		best = max(best, res);
	}
  cout<<best<<endl;
}
