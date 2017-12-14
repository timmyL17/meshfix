#define MESHFIX_WITH_EIGEN
#include "meshfix.h"

#include <igl/read_triangle_mesh.h>
#include <igl/write_triangle_mesh.h>
#include <iostream>

void
loadGmsh(const std::string &fileName,
         Eigen::MatrixXd &V,
         Eigen::MatrixXi &F)
{
    std::ifstream in(fileName.c_str());

    std::string line;

    // Skip first four lines
    for (int i = 0; i < 4; ++i)
    {
        std::getline(in, line);
    }

    // Next line is # of vertices
    int numVerts;
    in >> numVerts;

    V.resize(numVerts, 3);

    // Read the vertices
    for (int i = 0; i < numVerts; ++i)
    {
        int index;
        double x, y, z;

        in >> index >> x >> y >> z;

        V.row(i) << x, y, z;
    }

    // Skip two lines
    in >> line >> line;

    // Read # of triangles
    int numFaces;
    in >> numFaces;

    F.resize(numFaces, 3);

    int fluidCounter = 0;
    // Read triangles
    for (int i = 0; i < numFaces; ++i)
    {
        int ignore, type, index, v1, v2, v3;
        in >> ignore >> ignore >> ignore;
        in >> type >> index >> v1 >> v2 >> v3;

        v1 -= 1;
        v2 -= 1;
        v3 -= 1;

        F.row(i) << v1, v2, v3;
    }
}

int main(int argc, char * argv[])
{
  // Load in libigl's (V,F) format
  Eigen::MatrixXd V,W;
  Eigen::MatrixXi F,G;
  if(argc <= 2)
  {
    std::cout<<R"(Usage:
    ./meshfix-libigl [input](.obj|.ply|.stl|.off) [output](.obj|.ply|.stl|.off)
)";
    return EXIT_FAILURE;
  }

  // Read gmsh if requested
  int len = strlen(argv[1]);

  assert(len > 3);

  if (strcmp(argv[1] + len - 3, "msh") == 0)
  {
      loadGmsh(argv[1], V, F);
  }
  else
  {
      igl::read_triangle_mesh(argv[1],V,F);
  }

  meshfix(V,F,W,G,true);
  // Write to OBJ
  igl::write_triangle_mesh(argv[2],W,G);
}
