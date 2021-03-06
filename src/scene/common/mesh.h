#ifndef MOD_MESH
#define MOD_MESH

#include <iostream>
#include <glm/glm.hpp>
#include <iostream>
#include <vector>
#include <stdexcept>
#include <stb_image.h>
#include <stb_image.h>
#include "glad/glad.h"
#include "./util/loadmodel.h"
#include "./util/types.h"

struct Texture {
   unsigned int textureId;
};

Texture loadTexture(std::string textureFilePath);

struct Mesh {
  unsigned int VAOPointer;
  unsigned int VBOPointer;
  Texture texture; 
  unsigned int numElements;
  BoundInfo boundInfo;
};

Mesh loadMesh(std::string modelPath, std::string defaultTexture);		 // loads model and returns mesh/bound texture data
Mesh load2DMesh(std::string imagePath);		 // loads single quad mesh with texture centered around -1 to 1 x/y
Mesh load2DMeshTexCoords(std::string imagePath, float offsetx, float offsety, float width, float height); // 2DMesh with subimage selection
Mesh loadMeshFrom3Vert2TexCoords(std::string imagePath, std::vector<float> vertices, std::vector<unsigned int> indicies);
Mesh loadSpriteMesh(std::string imagePath);  // loads a 2d mesh with vertex centered around 0 to 1 x/y
void drawMesh(Mesh);  						 // draws mesh and related texture info (no shader data supplied)
void drawLines(std::vector<Line> allLines);

#endif 
