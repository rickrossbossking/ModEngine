#include "./vectorgfx.h"

void drawCube(float width, float height, float depth){
  std::vector<Line> allLines;                    
  allLines.push_back(Line { .fromPos = glm::vec3(0, 0, 0),           .toPos = glm::vec3(width, 0, 0)          });
  allLines.push_back(Line { .fromPos = glm::vec3(0,  height, 0),     .toPos = glm::vec3(width, height, 0)     });
  allLines.push_back(Line { .fromPos = glm::vec3(0,  0, depth),      .toPos = glm::vec3(width, 0, depth)      });
  allLines.push_back(Line { .fromPos = glm::vec3(0,  height, depth), .toPos = glm::vec3(width, height, depth) });
  allLines.push_back(Line { .fromPos = glm::vec3(0, 0, 0),           .toPos = glm::vec3(0, height, 0)         });
  allLines.push_back(Line { .fromPos = glm::vec3(width, 0, 0),       .toPos = glm::vec3(width, height, 0)     });
  allLines.push_back(Line { .fromPos = glm::vec3(0, 0, depth),       .toPos = glm::vec3(0, height, depth)     });
  allLines.push_back(Line { .fromPos = glm::vec3(width, 0, depth),   .toPos = glm::vec3(width, height, depth) });
  allLines.push_back(Line { .fromPos = glm::vec3(0, 0, 0),           .toPos = glm::vec3(0, 0, depth)          });
  allLines.push_back(Line { .fromPos = glm::vec3(0, height, 0),      .toPos = glm::vec3(0, height, depth)     });
  allLines.push_back(Line { .fromPos = glm::vec3(width, 0, 0),       .toPos = glm::vec3(width, 0, depth)      });
  allLines.push_back(Line { .fromPos = glm::vec3(width, height, 0),  .toPos = glm::vec3(width, height, depth) });
  drawLines(allLines);
}

void drawSphere(){                  // lots of repeat code here, should generalize
  static unsigned int resolution = 30;
  std::vector<Line> allLines;

  float lastX  = 1;
  float lastY = 0;
  for (unsigned int i = 1; i <= resolution;  i++){
    float radianAngle = i * ((2 * M_PI) / resolution);
    float x = cos(radianAngle);
    float y = sin(radianAngle);
    allLines.push_back(Line{ .fromPos = glm::vec3(lastX, lastY, 0), .toPos = glm::vec3(x, y, 0) });
    lastX = x;
    lastY = y;
  }
  
  lastX  = 1;
  lastY = 0;
  for (unsigned int i = 1; i <= resolution;  i++){
    float radianAngle = i * ((2 * M_PI) / resolution);
    float x = cos(radianAngle);
    float y = sin(radianAngle);
    allLines.push_back(Line{ .fromPos = glm::vec3(lastX, 0, lastY), .toPos = glm::vec3(x, 0, y) });
    lastX = x;
    lastY = y;
  }

  lastX  = 1;
  lastY = 0;
  for (unsigned int i = 1; i <= resolution;  i++){
    float radianAngle = i * ((2 * M_PI) / resolution);
    float x = cos(radianAngle);
    float y = sin(radianAngle);
    allLines.push_back(Line{ .fromPos = glm::vec3(0, lastX, lastY), .toPos = glm::vec3(0, x, y) });
    lastX = x;
    lastY = y;
  }

  drawLines(allLines);
}

void drawGridVertical(int numCellsWidth, int numCellsHeight, float cellSize, float offsetX, float offsetY, float offsetZ){   
  std::vector<Line> allLines;
  for (unsigned int i = 0 ; i < (numCellsHeight + 1); i++){
    allLines.push_back(Line {                                                   
      .fromPos = glm::vec3(0 + offsetX, (i * cellSize) + offsetY, 0 + offsetZ), 
      .toPos = glm::vec3((numCellsWidth * cellSize) + offsetX, (i * cellSize) + offsetY, 0 + offsetZ),
    });
  }
  for (unsigned int i = 0 ; i < (numCellsWidth + 1); i++){
    allLines.push_back(Line { 
      .fromPos = glm::vec3((i * cellSize) + offsetX, 0 + offsetY, 0 + offsetZ), 
      .toPos = glm::vec3((i * cellSize) + offsetX, (numCellsHeight * cellSize) + offsetY, 0 + offsetZ),
    });
  }
  drawLines(allLines);
}

void drawGridHorizontal(int numCellsWidth, int numCellsHeight, float cellSize, float offsetX, float offsetY, float offsetZ){
  std::vector<Line> allLines;
  for (unsigned int i = 0 ; i < (numCellsHeight + 1); i++){
    allLines.push_back(Line {                                                   
      .fromPos = glm::vec3(0 + offsetX, offsetY, (i * cellSize) + offsetZ), 
      .toPos = glm::vec3((numCellsWidth * cellSize) + offsetX, offsetY, (i * cellSize) + offsetZ),
    });
  }
  for (unsigned int i = 0 ; i < (numCellsWidth + 1); i++){
    allLines.push_back(Line { 
      .fromPos = glm::vec3((i * cellSize) + offsetX, 0 + offsetY, 0 + offsetZ), 
      .toPos = glm::vec3((i * cellSize) + offsetX, offsetY, (numCellsHeight * cellSize) +  offsetZ),
    });
  }
  drawLines(allLines);
}

void drawGrid3D(int numCellsWidth, float cellSize, float offsetX, float offsetY, float offsetZ){
  for (int i = 0; i <= numCellsWidth; i++){
    drawGridHorizontal(numCellsWidth, numCellsWidth, cellSize, offsetX, i * cellSize + offsetY, offsetZ);
  }
  for (int i = 0; i <= numCellsWidth; i++){
    drawGridVertical(numCellsWidth, numCellsWidth, cellSize, offsetX, offsetY, i * cellSize + offsetZ);
  }
}

void drawGrid3DCentered(int numCellsWidth, float cellSize, float offsetX, float offsetY, float offsetZ){
  float offset = numCellsWidth * cellSize / 2.f;
  drawGrid3D(numCellsWidth, cellSize, -offset + offsetX, -offset + offsetY, -offset + offsetZ);
}

void drawCoordinateSystem(float size){
  std::vector<Line> allLines;
  allLines.push_back(Line { .fromPos = glm::vec3(-1.f * size, 0.f, 0.f), .toPos = glm::vec3(1.f * size, 0.f, 0.f) });
  allLines.push_back(Line { .fromPos = glm::vec3(0.f, -1.f * size, 0.f), .toPos = glm::vec3(0.f, 1.f * size, 0.f) });
  allLines.push_back(Line { .fromPos = glm::vec3(0.f, 0.f, -1.f * size), .toPos = glm::vec3(0.f, 0.f, 1.f * size) });
  drawLines(allLines);
}