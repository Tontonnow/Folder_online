#include <iostream>

#include <chrono>
using namespace std;

class Point {
	public:
		Point(float x = 0.0,float y = 0.0):_x(x),_y(y){}
		//virtual float z(){return 0.0;}
	private:
		float _x,_y;
};

class Point3d :  public  Point {
	public:
		Point3d(float x = 0.0 , float y = 0.0, float z = 0.0):Point(x,y),_z(z){}

	protected:
		float _z;
};

class Vertex :  public virtual Point {
	public:
		Vertex(Vertex * next = 0 ):_next(next){}
		Vertex & operator=(const Vertex & v){
			Point::operator=(v);
			_next = v._next;
			return *this;
		}
	protected:
		Vertex *  _next;
};

// class Vertex3D :  public  Point3d,public Vertex {
// 	public:
// 		Vertex3D(){}
// 		Vertex3D & operator=(const Vertex3D & v){
// 			Point::operator=(v);
// 			Point3d::operator=(v);
// 			Vertex::operator=(v);
// 			return *this;
// 		}
	
// };


template <typename Ret,class Fx,class... Args>
Ret Time_Record(Fx func,Args... args){
	
	// 获取程序开始时间点
    auto start_time = std::chrono::high_resolution_clock::now();

    // 在这里放置你的代码，进行计算或处理
	Ret ret = 	func(args...);
    // 获取程序结束时间点
    auto end_time = std::chrono::high_resolution_clock::now();

    // 计算时间差
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();

    // 输出运行时间
    std::cout << "程序运行时间: " << duration << " 微秒" << std::endl;
	return ret;
}
template <typename Fx,class... Args>
void Time_Record(Fx func,Args... args){
	
	// 获取程序开始时间点
    auto start_time = std::chrono::high_resolution_clock::now();

    // 在这里放置你的代码，进行计算或处理
		func(args...);
    // 获取程序结束时间点
    auto end_time = std::chrono::high_resolution_clock::now();

    // 计算时间差
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();

    // 输出运行时间
    std::cout << "程序运行时间: " << duration << " 微秒" << std::endl;
	
}
Point3d lots_of_copies(Point3d a,Point3d b){
	Point3d pC = a;
	pC = b;
	b = a;
	return pC;
}
void _main(){
	
	Point3d pA(1.725,0.875,0.478);
	Point3d pB(0.315,0.317,0.838);
	Point3d pC;

	for( int iters = 0; iters < 10000000 ; iters++){
			pC = lots_of_copies(pA,pB);

	}
}

int main(int argc,char ** argv){
	
	Time_Record(_main);
		
	return 0;
}

