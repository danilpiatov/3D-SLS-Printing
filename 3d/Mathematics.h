//
// Created by dan on 5/11/23.
//

#ifndef INC_3D_MATHEMATICS_H
#define INC_3D_MATHEMATICS_H
#include <vector>
#include "Primitives.h"

std::vector<double> coefT(Point a, Point b, Point c, Point d){
     double Ktab = (d.x-c.x)/(b.x-a.x);
     double Btab = (c.x-a.x)/(b.x-a.x);
     double Ktcd = (b.z-a.z)/(d.z-c.z);
     double Btcd = (a.z-c.z)/(d.z-c.z);
     double tcd = (Ktcd*Btab + Btcd) / (1 - Ktab*Ktcd);
     double tab = Ktab*tcd + Btab;
     double x = a.x + (b.x-a.x)*tab;
     double y = a.z + (b.z-a.z)*tab;
     return std::vector<double>({x, y});
}



#endif //INC_3D_MATHEMATICS_H
