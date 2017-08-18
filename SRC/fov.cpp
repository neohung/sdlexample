
#include "game.h"
#include "fov.h"

#include <assert.h>
#include <math.h>

typedef struct {
	u32 x, y;
} FovCell;

typedef struct {
	float startSlope;
	float endSlope;
} Shadow;

#define FOV_DISTANCE	10
u8 shadowCount = 0;

FovCell map_cell_for_local_cell(u8 sector, FovCell heroMapCell, FovCell cellToTranslate) {
	switch (sector) {
		case 1: {
			FovCell mapCell = {heroMapCell.x + cellToTranslate.x, heroMapCell.y - cellToTranslate.y};
			return mapCell;
		}
		break;
		case 2: {
			FovCell mapCell = {heroMapCell.x + cellToTranslate.y, heroMapCell.y - cellToTranslate.x};
			return mapCell;
		}
		break;
		case 3: {
				FovCell mapCell = {heroMapCell.x + cellToTranslate.y, heroMapCell.y + cellToTranslate.x};
				return mapCell;
			}
			break;
			case 4: {
				FovCell mapCell = {heroMapCell.x + cellToTranslate.x, heroMapCell.y + cellToTranslate.y};
				return mapCell;
			}
			break;
			case 5: {
				FovCell mapCell = {heroMapCell.x - cellToTranslate.x, heroMapCell.y + cellToTranslate.y};
				return mapCell;
			}
			break;
			case 6: {
				FovCell mapCell = {heroMapCell.x - cellToTranslate.y, heroMapCell.y + cellToTranslate.x};
				return mapCell;
			}
			break;
			case 7: {
				FovCell mapCell = {heroMapCell.x - cellToTranslate.y, heroMapCell.y - cellToTranslate.x};
				return mapCell;
			}
			break;
			case 8: {
				FovCell mapCell = {heroMapCell.x - cellToTranslate.x, heroMapCell.y - cellToTranslate.y};
				return mapCell;
			}
			break;
			default:
				assert(false);
				FovCell c = {0,0};
				return c;
	}
}


float fov_distance_between(u32 x1, u32 y1, u32 x2, u32 y2) {
	return sqrt(pow(x2-x1, 2) + pow(y2-y1, 2));
}

void fov_calculate(u32 heroX, u32 heroY, u32 fovMap[][MAP_HEIGHT]) {
	// Reset FOV to default state (hidden)
	for (u32 x = 0; x < MAP_WIDTH; x++) {
		for (u32 y = 0; y < MAP_HEIGHT; y++) {
			fovMap[x][y] = 0;
		}
	}

	// Mark hero cell visible
	fovMap[heroX][heroY] = 1;
	//
	for (u8 sector = 1; sector <= 8; sector++) {
		bool prev_blocking = false;
		shadowCount = 0;
		float shadowStart = 0.0;
		float shadowEnd = 0.0;
		// For each distance from 1 to FOV range
		for (int cellY = 1; cellY < FOV_DISTANCE; cellY++) {
			prev_blocking = false;
			for (int cellX = 0; cellX <= cellY; cellX++) {
				FovCell heroCell = {heroX, heroY};
				//From 1~9
				FovCell cellToTranslate = {cellX, cellY};
				FovCell mapCell = map_cell_for_local_cell(sector, heroCell, cellToTranslate);
				if ((mapCell.x < MAP_WIDTH) && (mapCell.y < MAP_HEIGHT)) {
					// Is cell within view distance?
					if (fov_distance_between(0, 0, cellX, cellY) <= FOV_DISTANCE) {
						//float cellSlope = line_slope_between(0, 0, cellX, cellY);
						//if (!cell_in_shadow(cellSlope)) {
							fovMap[mapCell.x][mapCell.y] = 1;
						//}
					}
				}
			}
		}
	}
}
