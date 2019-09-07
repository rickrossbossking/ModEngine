#include "./state.h"

engineState getDefaultState(unsigned int initialScreenWidth, unsigned int initialScreenHeight){
	engineState state = {
		.visualizeNormals = false,
		.showCameras = true,
		.isSelectionMode = true,
		.isRotateSelection = false,
		.selectedName = "no object selected",
		.useDefaultCamera = true,
		.moveRelativeEnabled= false,
		.mode = 0,  // 0 = translate mode, 1 = scale mode, 2 = rotate
		.axis = 0,  // 0 = x, 1 = y, 2 = z
	    .currentScreenWidth = initialScreenWidth,
		.currentScreenHeight = initialScreenHeight,
		.cursorLeft = initialScreenWidth / 2,
		.cursorTop  = initialScreenHeight / 2,
		.selectedIndex = -1,
		.activeCamera = 0,
		.additionalText = "",
	};
	return state;
}
