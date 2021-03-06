#include "./scenegraph.h"

GameObject getGameObject(glm::vec3 position, std::string name, short id){
  auto physicsOptions = physicsOpts {
    .enabled = false,
    .isStatic = true,
    .hasCollisions = true,
    .shape = BOX,
  };
  GameObject gameObject = {
    .id = id,
    .name = name,
    .position = position,
    .scale = glm::vec3(1.0f, 1.0f, 1.0f),
    .rotation = glm::quat(0, 0, 0, 1.0f),
    .physicsOptions = physicsOptions,
  };
  return gameObject;
}

void addObjectToScene(Scene& scene, glm::vec3 position, std::string name, short id, short parentId){
  auto gameobjectObj = getGameObject(position, name, id);

  auto gameobjectH = GameObjectH {
    .id = gameobjectObj.id,
    .parentId = parentId,
  };

  scene.idToGameObjectsH[gameobjectObj.id] = gameobjectH;
  scene.idToGameObjects[gameobjectObj.id] = gameobjectObj;
  scene.nameToId[name] = gameobjectObj.id;
}

glm::vec3 parseVec(std::string positionRaw){;
  float x, y, z;
  std::istringstream in(positionRaw);
  in >> x >> y >> z;
  return glm::vec3(x, y, z);
}

Scene createSceneFromTokens(
  std::vector<Token> tokens,  
  std::function<void(short, std::string, std::string, std::string)> addObject, 
  std::vector<Field> fields,
  short (*getNewObjectId)()
){
  Scene scene;

  for (Token tok : tokens){
    std::string objectName = tok.target;

    std::string activeType = "default";
    for (Field field : fields){
      if (tok.target[0] == field.prefix ){
        activeType = field.type;
      }
    }

    if (!(scene.nameToId.find(objectName) != scene.nameToId.end())){
      addObjectToScene(scene, glm::vec3(1.0f, 1.0f, 1.0f), objectName, getNewObjectId(), -1);
      addObject(scene.nameToId[objectName], activeType, "", "");
    }

    short objectId = scene.nameToId[objectName];
    if (tok.attribute == "position"){
      scene.idToGameObjects[objectId].position = parseVec(tok.payload);
    }else if (tok.attribute == "scale"){
      scene.idToGameObjects[objectId].scale = parseVec(tok.payload);
    }else if (tok.attribute == "rotation"){
      glm::vec3 eulerAngles = parseVec(tok.payload);
      glm::quat rotation = glm::quat(glm::vec3(eulerAngles.x + 0, eulerAngles.y + 0, (eulerAngles.z + M_PI)));
      scene.idToGameObjects[objectId].rotation = rotation;
    }else if (tok.attribute == "physics"){
      auto physicsOptions = scene.idToGameObjects[objectId].physicsOptions;
      if (tok.payload == "enabled"){
        physicsOptions.enabled = true;
      }
      if (tok.payload == "disabled"){
        physicsOptions.enabled = false;
      }
      if (tok.payload == "dynamic"){
        physicsOptions.isStatic = false;
      }
      if (tok.payload == "nocollide"){
        physicsOptions.hasCollisions = false;
      }
      if (tok.payload == "shape_sphere"){
        physicsOptions.shape = SPHERE;
      }
      scene.idToGameObjects[objectId].physicsOptions = physicsOptions;
    }else if (tok.attribute == "parent"){
      if (!(scene.nameToId.find(tok.payload) != scene.nameToId.end())){
        short parentId = getNewObjectId();
        addObjectToScene(scene, glm::vec3(1.0f, 1.0f, 1.0f), tok.payload, parentId, -1);
        addObject(parentId, "default", "", "");
      }
      scene.idToGameObjectsH[objectId].parentId = scene.nameToId[tok.payload];
      scene.idToGameObjectsH[scene.nameToId[tok.payload]].children.insert(scene.idToGameObjectsH[objectId].id);
    }

    for (Field field: fields){
      if (field.type != activeType){
        continue;
      }
      for (std::string field : field.additionalFields){
        if (tok.attribute == field){
          addObject(objectId, activeType, field, tok.payload);
          break;
        }
      }
    }
  }

  for( auto const& [id, gameobjectH] : scene.idToGameObjectsH ){
    if (gameobjectH.parentId == -1){
      scene.rootGameObjectsH.push_back(gameobjectH.id);
    }
  }
  return scene;
}

// @todo this parsing is sloppy and buggy... obviously need to harden this..
Scene deserializeScene(
  std::string content,  
  std::function<void(short, std::string, std::string, std::string)> addObject, 
  std::vector<Field> fields,  
  short (*getNewObjectId)()
){
  std::cout << "INFO: Deserialization: " << std::endl;

  std::vector<Token> dtokens;

  std::vector<std::string> lines = split(content, '\n');
  for(std::string line: lines){
    std::vector<std::string> tokens = split(line, '#');

    if (tokens.size() > 0){
      std::vector<std::string> validToken = split(tokens[0], ':');

      Token token = {};
      if (validToken.size() > 0){
        token.target = trim(validToken[0]);
      }
      if (validToken.size() > 1){
        token.attribute = trim(validToken[1]);
      }
      if (validToken.size() > 2){
        token.payload = trim(validToken[2]);
      }
      if (token.target.length() > 0 ){
        dtokens.push_back(token);
      }
    }
  }

  return createSceneFromTokens(dtokens, addObject, fields, getNewObjectId);
}

std::string serializeVec(glm::vec3 vec){
  return std::to_string(vec.x) + " " + std::to_string(vec.y) + " " + std::to_string(vec.z);
}
std::string serializeRotation(glm::quat rotation){
  float xx = rotation.x;
  float yy = rotation.y;
  float zz = rotation.z;
  glm::vec3 angles = eulerAngles(rotation);
  return std::to_string(angles.x) + " " + std::to_string(angles.y) + " " + std::to_string(angles.z - M_PI); 
}
std::string serializeScene(Scene& scene, std::function<std::vector<std::pair<std::string, std::string>>(short)> getAdditionalFields){
  std::string sceneData = "# Generated scene \n";
  for (auto [id, gameobjecth]: scene.idToGameObjectsH){
    GameObject gameobject = scene.idToGameObjects[id];
    std::string gameobjectName = gameobject.name;
    std::string parentName = scene.idToGameObjects[gameobjecth.parentId].name;

    sceneData = sceneData + gameobjectName + ":position:" + serializeVec(gameobject.position) + "\n";
    sceneData = sceneData + gameobjectName + ":scale:" + serializeVec(gameobject.scale) + "\n";
    sceneData = sceneData + gameobjectName + ":rotation:" + serializeRotation(gameobject.rotation) + "\n";

    for (auto additionalFields : getAdditionalFields(id)){
      sceneData = sceneData + gameobjectName + ":" + additionalFields.first + ":" + additionalFields.second + "\n";
    }

    if (parentName != ""){
      sceneData =  sceneData + gameobjectName + ":parent:" + parentName + "\n";
    }
  }
  return sceneData;
}

void addObjectToScene(Scene& scene, std::string name, std::string mesh, glm::vec3 position, short (*getNewObjectId)(), std::function<void(short, std::string, std::string, std::string)> addObject){
  addObjectToScene(scene, position, name, getNewObjectId(), -1);
  short objectId = scene.nameToId[name];
  addObject(objectId, "default", "-", mesh);
  scene.rootGameObjectsH.push_back(objectId);
}


void traverseNodes(Scene& scene, short id, std::function<void(short)> onAddObject){
  auto parentObjH = scene.idToGameObjectsH[id];
  onAddObject(parentObjH.id);
  for (short id : parentObjH.children){
    traverseNodes(scene, id, onAddObject);
  }
}

std::vector<short> getChildrenIdsAndParent(Scene& scene, short id){
  std::vector<short> objectIds;
  auto onAddObject = [&objectIds](short id) -> void {
    objectIds.push_back(id);
  };
  traverseNodes(scene, id, onAddObject);
  return objectIds;
}

void removeObjectFromScene(Scene& scene, short id){
  auto objects = getChildrenIdsAndParent(scene, id);
  for (auto id : objects){
    std::string objectName = scene.idToGameObjects[id].name;
    scene.rootGameObjectsH.erase(std::remove(scene.rootGameObjectsH.begin(), scene.rootGameObjectsH.end(), id), scene.rootGameObjectsH.end());  
    scene.idToGameObjects.erase(id);
    scene.idToGameObjectsH.erase(id);
    scene.nameToId.erase(objectName);
  }
}

std::vector<short> listObjInScene(Scene& scene){
  std::vector<short> allObjects;
  for (auto const&[id, _] : scene.idToGameObjects){
    allObjects.push_back(id);
  }
  return allObjects;
}