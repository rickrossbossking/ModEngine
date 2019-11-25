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
void drawSphere(float radius){

}

void drawGrid(int numCellsWidth, int numCellsHeight, int cellSize, glm::vec3 pos){   
  std::vector<Line> allLines;
  // horizontal lines
  for (unsigned int i = 0 ; i < (numCellsHeight + 1); i++){
    allLines.push_back(Line {                                                   
      .fromPos = glm::vec3(pos.x, i * cellSize + pos.y, pos.z), 
      .toPos = glm::vec3(numCellsWidth * cellSize + pos.x, i * cellSize + pos.y, pos.z),
    });
  }
  // vertical lines
  for (unsigned int i = 0 ; i < (numCellsWidth + 1); i++){
    allLines.push_back(Line { 
      .fromPos = glm::vec3(i * cellSize + pos.x, pos.y, pos.z), 
      .toPos = glm::vec3(i * cellSize + pos.x, numCellsHeight * cellSize + pos.y, pos.z),
    });
  }
  drawLines(allLines);
}

void drawCoordinateSystem(float size){
  std::vector<Line> allLines;
  allLines.push_back(Line {
    .fromPos = glm::vec3(-1.f * size, 0.f, 0.f),
    .toPos = glm::vec3(1.f * size, 0.f, 0.f),
  });
  allLines.push_back(Line {
    .fromPos = glm::vec3(0.f, -1.f * size, 0.f),
    .toPos = glm::vec3(0.f, 1.f * size, 0.f),
  });
  allLines.push_back(Line {
    .fromPos = glm::vec3(0.f, 0.f, -1.f * size),
    .toPos = glm::vec3(0.f, 0.f, 1.f * size),
  });
  drawLines(allLines);
}