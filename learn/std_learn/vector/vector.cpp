#include <iostream>
#include <vector>

using namespace std;

class Student{

public:
	Student(string name){
		m_name = name ;
		cout <<"默认构造 " <<m_name << " " << this << endl;
	}
	Student(Student & that){
		m_name = that.m_name;
		cout <<"复制构造 " <<m_name << " " << this << endl;
	}
	~Student(){

		cout <<"析构 " << m_name << " " << this << endl;
	}
private:
	string m_name;
};


int main(){
	vector<Student> stus;
	stus.push_back(Student("张三"));
	stus.push_back(Student("李四"));


	return 0;
}