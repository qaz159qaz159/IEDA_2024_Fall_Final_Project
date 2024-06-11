#include "place.h"

int int32Cmp(void* a, void* b){
	return (*(int32_t*)a)-(*(int32_t*)b);
}

int place_main(FFs ffs, Gates gates, Inputs inputs, Outputs outputs, Insts insts, Nets nets, PlacementsRowsSet placementRowsSet, DisplacementDelay displacementDelay, Bin bin, double alpha, double lambda, double die_st_x, double die_st_y, double die_width, double die_height){
	
	void* iter;
	Inst* curInst;
	Inst* connectInst;
	char* key;
	void* value;
	void* item[16];
	FF* curFFLib;
	FF* connectFFLib;
	Gate* curGateLib;
	Gate* connectGateLib;
	PlacementsRows* curRows;
	Insts* rt_insts = &insts;
	Pin* curPin;
	Pin* connectPin;
	TimingSlack* curSlack;
	TimingSlack* iterSlack;
	Net* curNet;
	NetPin* curNetPin;
	IO* connectIO;
	double slackDistF;
	uint32_t slackDist;
	uint32_t max;
	uint8_t** occupy;
	int64_t i, j, k;
	uint32_t ffInstCnt =0;
	char** ffInstNames;
	uint64_t* binArea;
	uint32_t binYCnt = (die.height+bin.height-1)/bin.height;
	uint32_t binXCnt = (die.width+bin.width-1)/bin.width;
	int32_t* axis;
	uint32_t axisCnt, axisLen;
	uint32_t curX;
	uint32_t curY;
	char* strPin;
	char strInst[MAX_NAME_LEN];
	char strLibPin[MAX_NAME_LEN];

	ffInstNames = malloc(sizeof(char*));
	occupy = malloc(placementRowsSet.count*sizeof(uint8_t*));
	binArea = calloc(binXCnt*binYCnt, sizeof(uint64_t));

	for(i = 0; i < placementRowsSet.count; i++){
		occupy[i] = calloc((placementRowsSet.items[i]->totalNumOfSites+7)/8, sizeof(uint8_t));
	}
	for(curInst = insts.map; curInst; curInst = curInst->hh.next){
		key = curInst->lib_cell_name;
		HASH_FIND_STR(gates.map, key, value);
		if(!value){
			HASH_FIND_STR(ffs.map, key, value);
			if(!value) continue;
			ffInstCnt++;
			item[0] = realloc(ffInstNames, ffInstCnt);
			if(!item[0]){
				perror("realloc");
				fprintf(stderr, "%02x\n", errno);
				exit(-1);
			}
			ffInstNames = item[0];
			ffInstNames[ffInstCnt-1] = curInst->inst_name;
			continue;
		}
		curGateLib = value;
		for(i = 0; i < placementRowsSet.count; i++){
			if(placementRowsSet.items[i]->start_y < curInst->y) continue;
			if(placementRowsSet.items[i]->start_y > curGateLib->height-1+curInst->y) break;
			curRows = placementRowsSet.items[i];
			for(
					j = (curInst->x-curRows->start_x)/curRows->width;
					j < (curInst->x-curRows->start_x+curGateLib->height-1)/curRows->width;
					j++
					){
				occupy[i][j/8] |= 0x01<<(j%8);
			}
		}
	}

	//assume ffInstNames is sorted by Area and the max is first element
	
	axisLen = 64;
	axis = malloc(axisLen*sizeof(uint32_t));
	for(i = 0; i < ffInstCnt; i++){
		key = ffInstNames[i];
		HASH_FIND_STR(insts.map, key, value);
		if(!value) continue;
		curInst = value;
		key = curInst->lib_cell_name;
		HASH_FIND_STR(ffs.map, key, value);
		if(!value) continue;
		curFFLib = value;
		axisCnt = 0;
		for(curPin = curFFLib->map; curPin; curPin = curPin->hh.next){
			key = curPin->name;
			for(curNet = nets.map; curNet; curNet = curNet->hh.next){
				HASH_FIND_STR(curNet->map, key, value);
				if(value) break;
			}
			if(curPin->name[0] == 'D'){
				for(curNetPin = curNet->map; curNetPin; curNetPin = curNetPin->hh.next){
					if(strPin = strchr(curNetPin->instName, '/')){
						memcpy(strInst, curNetPin->instName, strPin-curNetPin->instName);
						strInst[strPin-curNetPin->instName] = '\0';
						strcpy(strLibPin, strPin);
						if(!memcmp(strPin+1, "Q", 1)){
							for(j = 0; j < i; j++){
								if(!strcmp(strInst, ffInstNames[j])) break;
							}
							if(i >= j) continue;
							key = strInst;
							HASH_FIND_STR(insts.map, key, value);
							if(!value) continue;
							connectInst = value;
							key = connectInst->lib_cell_name;
							HASH_FIND_STR(ffs.map, key, value);
							if(!value) continue;
							connectFFLib = value;
							key = strLibPin;
							HASH_FIND_STR(connectFFLib->map, key, value);
							if(!value) continue;
							connectPin = value;
							if(axisCnt+1 > axisLen){
								axisLen *= 2;
								axis = realloc(axis, axisLen*sizeof(int32_t));
							}
							axis[axisCnt++] = (connectPin->x+connectInst->x)-curPin->x;
						}else{
							if(MAX_NAME_LEN-(strPin-curNetPin->instName) >= 4){
								if(!memcmp(strPin+1, "OUT", 3)){
									key = strInst;
									HASH_FIND_STR(insts.map, key, value);
									if(!value) continue;
									connectInst = value;
									key = connectInst->lib_cell_name;
									HASH_FIND_STR(ffs.map, key, value);
									if(!value) continue;
									connectGateLib = value;
									key = strLibPin;
									HASH_FIND_STR(connectGateLib->map, key, value);
									if(!value) continue;
									connectPin = value;
									if(axisCnt+1 > axisLen){
										axisLen *= 2;
										axis = realloc(axis, axisLen*sizeof(int32_t));
									}
									axis[axisCnt++] = (connectPin->x+connectInst->x)-curPin->x;
								}
							}
						}
					}else{
						if(!memcmp(curNetPin->instName, "OUTPUT", 6)){
							key = curNetPin->instName;
							HASH_FIND_STR(outputs.map, key, value);
							if(!value) continue;
							connectIO = value;
							if(axisCnt+1 > axisLen){
								axisLen *= 2;
								axis = realloc(axis, axisLen*sizeof(int32_t));
							}
							axis[axisCnt++] = connectIO->x-curPin->x;
						}
					}
				}
			}else if(curPin->name[0] == 'Q'){
				for(curNetPin = curNet->map; curNetPin; curNetPin = curNetPin->hh.next){
					if(strPin = strchr(curNetPin->instName, '/')){
						memcpy(strInst, curNetPin->instName, strPin-curNetPin->instName);
						strInst[strPin-curNetPin->instName] = '\0';
						strcpy(strLibPin, strPin);
						if(!memcmp(strPin+1, "D", 1)){
							for(j = 0; j < i; j++){
								if(!strcmp(strInst, ffInstNames[j])) break;
							}
							if(i >= j) continue;
							key = strInst;
							HASH_FIND_STR(insts.map, key, value);
							if(!value) continue;
							connectInst = value;
							key = connectInst->lib_cell_name;
							HASH_FIND_STR(ffs.map, key, value);
							if(!value) continue;
							connectFFLib = value;
							key = strLibPin;
							HASH_FIND_STR(connectFFLib->map, key, value);
							if(!value) continue;
							connectPin = value;
							if(axisCnt+1 > axisLen){
								axisLen *= 2;
								axis = realloc(axis, axisLen*sizeof(int32_t));
							}
							axis[axisCnt++] = (connectPin->x+connectInst->x)-curPin->x;
						}else{
							if(MAX_NAME_LEN-(strPin-curNetPin->instName) >= 3){
								if(!memcmp(strPin+1, "IN", 2)){
									key = strInst;
									HASH_FIND_STR(insts.map, key, value);
									if(!value) continue;
									connectInst = value;
									key = connectInst->lib_cell_name;
									HASH_FIND_STR(ffs.map, key, value);
									if(!value) continue;
									connectGateLib = value;
									key = strLibPin;
									HASH_FIND_STR(connectGateLib->map, key, value);
									if(!value) continue;
									connectPin = value;
									if(axisCnt+1 > axisLen){
										axisLen *= 2;
										axis = realloc(axis, axisLen*sizeof(int32_t));
									}
									axis[axisCnt++] = (connectPin->x+connectInst->x)-curPin->x;
								}
							}
						}
					}else{
						if(!memcmp(curNetPin->instName, "INPUT", 5)){
							key = curNetPin->instName;
							HASH_FIND_STR(outputs.map, key, value);
							if(!value) continue;
							connectIO = value;
							if(axisCnt+1 > axisLen){
								axisLen *= 2;
								axis = realloc(axis, axisLen*sizeof(int32_t));
							}
							axis[axisCnt++] = connectIO->x-curPin->x;
						}
					}
				}
			}
		}
		qsort(axis, axisCnt, sizeof(int32_t), int32Cmp);
		if(axisCnt%2){
			curX = axis[axisCnt/2];
		}else{
			curX = axis[axisCnt/2]+axis[axisCnt/2-1];
		}
		axisCnt = 0;
		for(curPin = curFFLib->map; curPin; curPin = curPin->hh.next){
			key = curPin->name;
			for(curNet = nets.map; curNet; curNet = curNet->hh.next){
				HASH_FIND_STR(curNet->map, key, value);
				if(value) break;
			}
			if(curPin->name[0] == 'D'){
				for(curNetPin = curNet->map; curNetPin; curNetPin = curNetPin->hh.next){
					if(strPin = strchr(curNetPin->instName, '/')){
						memcpy(strInst, curNetPin->instName, strPin-curNetPin->instName);
						strInst[strPin-curNetPin->instName] = '\0';
						strcpy(strLibPin, strPin);
						if(!memcmp(strPin+1, "Q", 1)){
							for(j = 0; j < i; j++){
								if(!strcmp(strInst, ffInstName[j])) break;
							}
							if(i >= j) continue;
							key = strInst;
							HASH_FIND_STR(insts.map, key, value);
							if(!value) continue;
							connectInst = value;
							key = connectInst->lib_cell_name;
							HASH_FIND_STR(ffs.map, key, value);
							if(!value) continue;
							connectFFLib = value;
							key = strLibPin;
							HASH_FIND_STR(connectFFLib->map, key, value);
							if(!value) continue;
							connectPin = value;
							if(axisCnt+1 > axisLen){
								axisLen *= 2;
								axis = realloc(axis, axisLen*sizeof(int32_t));
							}
							axis[axisCnt++] = (connectPin->y+connectInst->y)-curPin->y;
						}else{
							if(MAX_NAME_LEN-(strPin-curNetPin->instName) >= 4){
								if(!memcmp(strPin+1, "OUT", 3)){
									key = strInst;
									HASH_FIND_STR(insts.map, key, value);
									if(!value) continue;
									connectInst = value;
									key = connectInst->lib_cell_name;
									HASH_FIND_STR(ffs.map, key, value);
									if(!value) continue;
									connectGateLib = value;
									key = strLibPin;
									HASH_FIND_STR(connectGateLib->map, key, value);
									if(!value) continue;
									connectPin = value;
									if(axisCnt+1 > axisLen){
										axisLen *= 2;
										axis = realloc(axis, axisLen*sizeof(int32_t));
									}
									axis[axisCnt++] = (connectPin->y+connectInst->y)-curPin->y;
								}
							}
						}
					}else{
						if(!memcmp(curNetPin->instName, "OUTPUT", 6)){
							key = curNetPin->instName;
							HASH_FIND_STR(outputs.map, key, value);
							if(!value) continue;
							connectIO = value;
							if(axisCnt+1 > axisLen){
								axisLen *= 2;
								axis = realloc(axis, axisLen*sizeof(int32_t));
							}
							axis[axisCnt++] = connectIO->y-curPin->y;
						}
					}
				}
			}else if(curPin->name[0] == 'Q'){
				for(curNetPin = curNet->map; curNetPin; curNetPin = curNetPin->hh.next){
					if(strPin = strchr(curNetPin->instName, '/')){
						memcpy(strInst, curNetPin->instName, strPin-curNetPin->instName);
						strInst[strPin-curNetPin->instName] = '\0';
						strcpy(strLibPin, strPin);
						if(!memcmp(strPin+1, "D", 1)){
							for(j = 0; j < i; j++){
								if(!strcmp(strInst, ffInstName[j])) break;
							}
							if(i >= j) continue;
							key = strInst;
							HASH_FIND_STR(insts.map, key, value);
							if(!value) continue;
							connectInst = value;
							key = connectInst->lib_cell_name;
							HASH_FIND_STR(ffs.map, key, value);
							if(!value) continue;
							connectFFLib = value;
							key = strLibPin;
							HASH_FIND_STR(connectFFLib->map, key, value);
							if(!value) continue;
							connectPin = value;
							if(axisCnt+1 > axisLen){
								axisLen *= 2;
								axis = realloc(axis, axisLen*sizeof(int32_t));
							}
							axis[axisCnt++] = (connectPin->y+connectInst->y)-curPin->y;
						}else{
							if(MAX_NAME_LEN-(strPin-curNetPin->instName) >= 3){
								if(!memcmp(strPin+1, "IN", 2)){
									key = strInst;
									HASH_FIND_STR(insts.map, key, value);
									if(!value) continue;
									connectInst = value;
									key = connectInst->lib_cell_name;
									HASH_FIND_STR(ffs.map, key, value);
									if(!value) continue;
									connectGateLib = value;
									key = strLibPin;
									HASH_FIND_STR(connectGateLib->map, key, value);
									if(!value) continue;
									connectPin = value;
									if(axisCnt+1 > axisLen){
										axisLen *= 2;
										axis = realloc(axis, axisLen*sizeof(int32_t));
									}
									axis[axisCnt++] = (connectPin->y+connectInst->y)-curPin->y;
								}
							}
						}
					}else{
						if(!memcmp(curNetPin->instName, "INPUT", 5)){
							key = curNetPin->instName;
							HASH_FIND_STR(outputs.map, key, value);
							if(!value) continue;
							connectIO = value;
							if(axisCnt+1 > axisLen){
								axisLen *= 2;
								axis = realloc(axis, axisLen*sizeof(int32_t));
							}
							axis[axisCnt++] = connectIO->y-curPin->y;
						}
					}
				}
			}
		}
		qsort(axis, axisCnt, sizeof(int32_t), int32Cmp);
		if(axisCnt%2){
			curY = axis[axisCnt/2];
		}else{
			curY = axis[axisCnt/2]+axis[axisCnt/2-1];
		}
		curInst->x = curX;
		curInst->y = curY;
		for(i = 0; i < placementRowsSet.count; i++){
			if(placementRowsSet.items[i]->start_y < curInst->y) continue;
			if(placementRowsSet.items[i]->start_y > curFFLib->height-1+curInst->y) break;
			curRows = placementRowsSet.items[i];
			for(
					j = (curInst->x-curRows->start_x)/curRows->width;
					j < (curInst->x-curRows->start_x+curFFLib->height-1)/curRows->width;
					j++
					){
				occupy[i][j/8] |= 0x01<<(j%8);
			}
		}
	}
	free(axis);
	for(i = 0; i < placementRowsSet.count; i++){
		free(occupy[i]);
	}
	free(binArea);
	free(ffInstNames);
	free(occupy);
	return 0;
}
