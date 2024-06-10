#include "place.h"

int place_main(FFs ffs, Gates gates, Inputs inputs, Outputs outputs, Insts insts, Nets nets, PlacementsRowsSet placementRowsSet, TimingSlacks timeSlacks, DisplacementDelay displacementDelay, Bin bin, FFPlace* rt){
	
	void* iter;
	Inst* curInst;
	char* key;
	void* value;
	void* item[16];
	FF* curFFLib;
	Gate* curGateLib;
	PlacementRows* curRows;
	Insts* rt_insts = &insts;
	uint32_t max;
	uint8_t** occupy;
	int64_t i, j, k;
	uint32_t ffInstCnt =0;
	char** ffInstNames;

	ffInstNames = malloc(sizeof(char*));
	occupy = malloc(placementRowsSet.count*sizeof(uint8_t*));

	for(i = 0; i < placementRowSet.count; i++){
		occupy[i] = calloc((placementRowsSet.items[i]->totalNumOfSites+7)/8, sizeof(uint8_t));
	}
	for(curInst = insts.map; cutInst; curInst = curInst->hh.next){
		key = curInst->lib_cell_name;
		HASH_FIND_STR(gates.map, key, value);
		if(!value){
			HASH_FIND_STR(ffs.map, key, value);
			if(!value) continue;
			ffInstCnt++;
			item[0]
			item[0] = realloc(ffInstNames, ffInstCnt);
			if(item[0]){
				perror("realloc");
				fprintf(stderr, "%02x\n", errno);
				exit(-1);
			}
			ffInstNames = item[0];
			ffInstNames[ffInstCnt-1] = curInst->inst_name;
			continue;
		}
		curGateLib = value;
		for(i = 0; i < placementRowSet.count; i++){
			if(placmentRowsSet.items[i]->start_y < curInst->y) continue;
			if(placementRowsSet.items[i]->start_y > curGateLib->height-1+curInst->y) break;
			curRows = placementRowsSet.items[i];
			for(
					j = (curInst->x-curRows->start_x)/curRows->width;
					j < (curInst->x-curRows->start_x+surGateLib->height-1)/curRows->width;
					j++
					){
				occupy[i][j/8] |= 0x01<<(j%8);
			}
		}
	}

	for(i = ffInstCnt/2; i >= 0; i--){
		j = i;

	}
	
	
	for(i = 0; i < placementRowSet.count; i++){
		free(occupy[i]);
	}
	free(ffInstCnt);
	free(occupy)
	return 0;
}
