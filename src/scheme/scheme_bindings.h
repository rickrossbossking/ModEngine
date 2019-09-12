#ifndef MOD_SCHEMEBINDINGS
#define MOD_SCHEMEBINDINGS

#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include "./util/guile.h"

void createStaticSchemeBindings(
	void (*moveCamera)(glm::vec3),  
	void (*rotateCamera)(float xoffset, float yoffset),
	void (*removeObjectById)(short id),
	void (*makeObjectV)(float, float, float),
	std::vector<short> (*getObjectsByType)(std::string)
);

void startShell();

#endif