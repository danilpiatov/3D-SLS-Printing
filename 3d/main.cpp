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

#include "Polygons.h"

using namespace std;

int main(int argc, char* argv[]) {
   try {

       Polygons polygons;
       std::vector<Line> lines;
       std::string path;
       std::cout << "Type path to lines file\n";
       std::cin >> path;
       std::ifstream in(path);
       int linesNum;
       in >> linesNum;
       for (int i = 0; i < linesNum; ++i){
           lines.push_back(Line{});
           in >> lines.back().a.x;
           in >> lines.back().a.z;
           in >> lines.back().b.x;
           in >> lines.back().b.z;
       }
       in.close();
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
