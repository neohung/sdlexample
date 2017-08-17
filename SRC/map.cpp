#include <stdio.h>
#include "game.h"
#include "ui.h"
#include "map.h"
//mapCells is two Dimension Array

bool **map_new(int w,int h)
{
	bool **Array, *pData;
	int m=h;
	int n=w;
	int index;
	Array = (bool **)malloc(m*sizeof(bool *)+m*n*sizeof(bool));
	for (index = 0, pData = (bool *)(Array+m); index < m; index++, pData += n)
		Array[index]=pData;
	return Array;
}

bool map_carve_room(u32 x, u32 y, u32 w, u32 h, bool **map) {
	// Determine if all the cells within the given rectangle are filled
	for (u8 i = x-1; i < x + (w + 1); i++) {
		for (u8 j = y-1; j < y + (h + 1); j++) {
			if (map[j][i] == false) {
				//Means is not all fill block, Don't do Carve
				//printf("Can't found block in %d,%d\n",x,y);
				return false;
			}
		}
	}
	// Carve out the room
	for (u8 i = x; i < x + w; i++) {
		for (u8 j = y; j < y + h; j++) {
			map[j][i] = false;
		}
	}
	return true;
}

Point rect_random_point(UIRect rect) {
	u32 px = (rand() % (rect.w - 1)) + rect.x;
	u32 py = (rand() % (rect.h - 1)) + rect.y;
	Point ret = {px, py};
	return ret;
}


void map_get_segments(List *segments, Point from, Point to, UIRect *rooms, u32 roomCount) {
	// Walk between our two points and find all the spans between rooms
}

void map_generate(bool **mapCells) {
	// Mark all the map cells as "filled"
	for (u32 x = 0; x < MAP_WIDTH; x++) {
		for (u32 y = 0; y < MAP_HEIGHT; y++) {
			mapCells[y][x] = true;
		}
	}
	// Carve out non-overlapping rooms that are randomly placed, and of
	// random size.
	bool roomsDone = false;
	UIRect rooms[100];
	u32 cellsUsed = 0;
	u32 roomCount = 0;
	float unsuccess_count = 0;
	float end_condition_rate = 0.45;
	while (!roomsDone) {
		// Generate a random width/height for a room
		u32 w = (rand() % 10) + 5;
		u32 h = (rand() % 10) + 5;
		u32 x = rand() % (MAP_WIDTH - w - 1);
		u32 y = rand() % (MAP_HEIGHT - h - 1);
		if (x == 0) x = 1;
		if (y == 0) y = 1;
		//printf("w=%dmh=%d,x=%d,y=%d\n",w,h,x,y);

		bool success = map_carve_room(x, y, w, h, mapCells);
		if (success) {
			UIRect r = {(int)x, (int)y, (int)w, (int)h};
			rooms[roomCount] = r;
			roomCount += 1;
			cellsUsed += (w * h);
		}

		float used_rate = ((float)cellsUsed / (float)(MAP_HEIGHT * MAP_WIDTH));
		if (success){
			unsuccess_count = 0;
		}else{
			unsuccess_count++;
		}
		if (unsuccess_count > 50){
			end_condition_rate -= 0.01;
			unsuccess_count = 0;
		}
		//printf("cellused %f\n",((float)cellsUsed / (float)(MAP_HEIGHT * MAP_WIDTH)));
		// Exit condition - more that desired % of cells in use
		if (((float)cellsUsed / (float)(MAP_HEIGHT * MAP_WIDTH)) > end_condition_rate) {
			roomsDone = true;
		}

	}
	// Join all rooms with corridors, so that all rooms are reachable
	List *hallways = list_new(free);
	for (u32 r = 1; r < roomCount; r++) {
		Point fromPt = rect_random_point(rooms[r-1]);
		Point toPt = rect_random_point(rooms[r]);
		//Create segments
		List *segments = list_new(free);
		// Break the proposed hallway into segments joining rooms
		map_get_segments(segments, fromPt, toPt, rooms, roomCount);

	}
	// Clean up
	list_destroy(hallways);
}
