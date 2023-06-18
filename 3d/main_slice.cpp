#include <iostream>
#include <iomanip>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <fstream>

#include "Obj3DModel.h"
#include "Polygons.h"

using namespace std;

int main(int argc, char* argv[]) {
   try {

       Polygons polygons;
       std::string path;
       std::cout << "Type path to stl file\n";
       std::cin >> path;
       Obj3DModel model;
       model.parse(path.c_str());
       std::cout << "Type slice height\n";
       double height;
       std::cin >> height;
       std::vector<Line> lines = model.slice(height);
       Point p;
       std::cout <<"Type start point in format \"x y\"\n";
       std::cin >> p.x >> p.z;
       double width;
       std::cout << "Type laser width\n";
       std::cin >> width;
       polygons.polygonizeAll(lines);
       bool next = false;
       for(Point point: polygons.findRoute(width, p)){
           std::cout<<"("<<point.x <<"; "<<point.z << ") ";
           if (next){
               std::cout << '\n';
           }
           next = !next;
       }
    }
    catch (const std::exception& e) {
       std::cerr << "Error: " << e.what() << std::endl;
   }


    return 0;
}
