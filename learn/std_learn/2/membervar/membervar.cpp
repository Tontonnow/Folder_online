#include <iostream>

using namespace std;
template<class T> 
class Array{
public:
	T& operator[](size_t i){
		return m_arr[i];
	}
private:
	T m_arr[10];
};
template<class D> class Sum{
	public:
	Sum(Array<D> &s):m_s(s){}
	D add(){
			D sum = 0;
		for(int i=0;i<10;i++)
				sum += m_s[i];
		return sum;
	}
	private:
		Array<D> m_s;

};

int main(){
	Array<int> a;
	for(int i=0;i<10;i++)
		a[i] = i+1;
	Sum<int> s(a);
	cout << s.add()<< endl;

	return 0;
}