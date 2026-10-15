#include "PopulationSpatialIndex.h"
#include <iostream>
#include <random>
#include <stdexcept>

static void Require(bool ok) { if (!ok) throw std::runtime_error("spatial index differs from brute-force radius count"); }
int main()
{
    struct Point { uint32_t map, zone; float x,y; };
    std::mt19937 random(19);
    std::uniform_real_distribution<float> coord(-1500,1500);
    std::vector<Point> points;
    for (unsigned i=0; i<6000; ++i) points.push_back({i%2,i%7,coord(random),coord(random)});
    for (float radius : {1.f, 50.f, 1000.f})
    {
        PopulationSpatialIndex index(radius);
        for (auto const& p : points) index.Add(p.map,p.zone,p.x,p.y);
        for (unsigned i=0; i<500; ++i)
        {
            Point q{i%2,i%7,coord(random),coord(random)};
            uint32_t count=0;
            for (auto const& p : points)
                if (p.map==q.map && p.zone==q.zone &&
                    std::hypot(double(p.x)-q.x,double(p.y)-q.y)<=radius) ++count;
            for (uint32_t cap : {0u,1u,5u,count,count+1})
                Require(index.AtLeast(q.map,q.zone,q.x,q.y,cap)==(count>=cap));
        }
    }
    PopulationSpatialIndex edge(10);
    edge.Add(0,1,-10,0); edge.Add(0,1,10,0);
    edge.Add(1,1,0,0); edge.Add(0,2,0,0);
    Require(edge.AtLeast(0,1,0,0,2));
    Require(!edge.AtLeast(0,1,0,0,3));
    PopulationSpatialIndex disabled(0);
    disabled.Add(0,1,0,0);
    Require(!disabled.AtLeast(0,1,0,0,1));
    std::cout << "PASS: spatial queries match brute force, 6k population, map/zone isolation and boundary cases\n";
}
