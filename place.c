#include "place.h"

int int32Cmp(const void* a, const void* b){
	return (*(int32_t*)a)-(*(int32_t*)b);
}

int place_main(FFs ffs, Gates* gates, Inputs inputs, Outputs outputs, Insts insts, Nets nets, PlacementsRowsSet placementRowsSet, DisplacementDelay displacementDelay, Bin bin, double alpha, double lambda, Die* die){
	
	void* iter;
	Inst* curInst;
	Inst* connectInst;
	char* key;
	void* value;
	void* item[1];
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
	uint32_t RowMax = 0;
	uint8_t** occupy;
	int64_t i, j, k;
	uint32_t ffInstCnt =0;
	char** ffInstNames;
	uint64_t* binArea;
	uint32_t binYCnt = (die->height+bin.height-1)/bin.height;
	uint32_t binXCnt = (die->width+bin.width-1)/bin.width;
	int32_t* axis;
	uint32_t axisCnt, axisLen;
	uint32_t curX;
	uint32_t curY;
	uint32_t curDist;
	uint32_t curDistY;
	uint8_t check;
	char* strPin;
	char strInst[MAX_NAME_LEN];
	char strLibPin[MAX_NAME_LEN];
	Output* cuOut;
	Input* cuIn;

	ffInstNames = malloc(sizeof(char*));
	occupy = malloc(placementRowsSet.count*sizeof(uint8_t*));
	binArea = calloc(binXCnt*binYCnt, sizeof(uint64_t));

	for(i = 0; i < placementRowsSet.count; i++){
		occupy[i] = calloc((placementRowsSet.items[i]->totalNumOfSites+7)/8, sizeof(uint8_t));
		if(RowMax < placementRowsSet.items[i]->totalNumOfSites){
			RowMax = placementRowsSet.items[i]->totalNumOfSites;
		}
	}
	for(curInst = insts.map; curInst; curInst = curInst->hh.next){
		if(curInst->isUsed == NOT_USED) continue;
		key = curInst->lib_cell_name;
		HASH_FIND_STR(gates->map, key, curGateLib);
		if(!curGateLib){
			HASH_FIND_STR(ffs.map, key, curFFLib);
			if(!curFFLib) continue;
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
		HASH_FIND_STR(insts.map, key, curInst);
		if(!curInst) continue;
		key = curInst->lib_cell_name;
		HASH_FIND_STR(ffs.map, key, curFFLib);
		if(!curFFLib) continue;
		axisCnt = 0;
		for(curPin = curFFLib->map; curPin; curPin = curPin->hh.next){
			sprintf(key, "%s/%s", curInst->inst_name, curPin->name);
			for(curNet = nets.map; curNet; curNet = curNet->hh.next){
				HASH_FIND_STR(curNet->map, key, curNetPin);
				if(curNetPin) break;
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
							HASH_FIND_STR(insts.map, key, connectInst);
							if(!connectInst) continue;
							key = connectInst->lib_cell_name;
							HASH_FIND_STR(ffs.map, key, connectFFLib);
							if(!connectFFLib) continue;
							key = strLibPin;
							HASH_FIND_STR(connectFFLib->map, key, connectPin);
							if(!connectPin) continue;
							if(axisCnt+1 > axisLen){
								axisLen *= 2;
								axis = realloc(axis, axisLen*sizeof(int32_t));
							}
							axis[axisCnt++] = (connectPin->x+connectInst->x)-curPin->x;
						}else{
							if(MAX_NAME_LEN-(strPin-curNetPin->instName) >= 4){
								if(!memcmp(strPin+1, "OUT", 3)){
									key = strInst;
									HASH_FIND_STR(insts.map, key, connectInst);
									if(!connectInst) continue;
									key = connectInst->lib_cell_name;
									HASH_FIND_STR(ffs.map, key, connectGateLib);
									if(!connectGateLib) continue;
									key = strLibPin;
									HASH_FIND_STR(connectGateLib->map, key, connectPin);
									if(!connectPin) continue;
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
							HASH_FIND_STR(outputs.map, key, connectIO);
							if(!connectIO) continue;
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
							HASH_FIND_STR(insts.map, key, curInst);
							key = connectInst->lib_cell_name;
							HASH_FIND_STR(ffs.map, key, curFFLib);
							key = strLibPin;
							HASH_FIND_STR(connectFFLib->map, key, curFFLib);
							if(axisCnt+1 > axisLen){
								axisLen *= 2;
								axis = realloc(axis, axisLen*sizeof(int32_t));
							}
							axis[axisCnt++] = (connectPin->x+connectInst->x)-curPin->x;
						}else{
							if(MAX_NAME_LEN-(strPin-curNetPin->instName) >= 3){
								if(!memcmp(strPin+1, "IN", 2)){
									key = strInst;
									HASH_FIND_STR(insts.map, key, curInst);
									if(!value) continue;
									connectInst = value;
									key = connectInst->lib_cell_name;
									HASH_FIND_STR(ffs.map, key, curFFLib);
									if(!value) continue;
									connectGateLib = value;
									key = strLibPin;
									HASH_FIND_STR(connectGateLib->map, key, curGateLib);
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
							HASH_FIND_STR(outputs.map, key, cuOut);
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
				HASH_FIND_STR(curNet->map, key, curNet);
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
							HASH_FIND_STR(insts.map, key, curInst);
							if(!value) continue;
							connectInst = value;
							key = connectInst->lib_cell_name;
							HASH_FIND_STR(ffs.map, key, curFFLib);
							if(!value) continue;
							connectFFLib = value;
							key = strLibPin;
							HASH_FIND_STR(connectFFLib->map, key, curFFLib);
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
									HASH_FIND_STR(insts.map, key, connectInst);
									if(!connectInst) continue;
									key = connectInst->lib_cell_name;
									HASH_FIND_STR(ffs.map, key, connectGateLib);
									if(!connectGateLib) continue;
									key = strLibPin;
									HASH_FIND_STR(connectGateLib->map, key, connectPin);
									if(!connectPin) continue;
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
							HASH_FIND_STR(outputs.map, key, connectIO);
							if(!connectIO) continue;
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
								if(!strcmp(strInst, ffInstNames[j])) break;
							}
							if(i >= j) continue;
							key = strInst;
							HASH_FIND_STR(insts.map, key, connectInst);
							if(!connectInst) continue;
							key = connectInst->lib_cell_name;
							HASH_FIND_STR(ffs.map, key, connectFFLib);
							if(!connectFFLib) continue;
							key = strLibPin;
							HASH_FIND_STR(connectFFLib->map, key, connectPin);
							if(!connectPin) continue;
							if(axisCnt+1 > axisLen){
								axisLen *= 2;
								axis = realloc(axis, axisLen*sizeof(int32_t));
							}
							axis[axisCnt++] = (connectPin->y+connectInst->y)-curPin->y;
						}else{
							if(MAX_NAME_LEN-(strPin-curNetPin->instName) >= 3){
								if(!memcmp(strPin+1, "IN", 2)){
									key = strInst;
									HASH_FIND_STR(insts.map, key, connectInst);
									if(!connectInst) continue;
									key = connectInst->lib_cell_name;
									HASH_FIND_STR(ffs.map, key, connectGateLib);
									if(!connectGateLib) continue;
									key = strLibPin;
									HASH_FIND_STR(connectGateLib->map, key, connectPin);
									if(!connectPin) continue;
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
							HASH_FIND_STR(outputs.map, key, connectIO);
							if(!connectIO) continue;
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
		for(curDist = 0, curDistY = 0; curDist < RowMax; curDist++){
			for(curDistY = 0; curDistY < curDist; curDistY++){
				check = 0;
				for(i = curDistY; i < placementRowsSet.count; i++){
					if(placementRowsSet.items[i-curDistY]->start_y < curY) continue;
					if(placementRowsSet.items[i-curDistY]->start_y > curFFLib->height-1+curY) break;
					curRows = placementRowsSet.items[i-curDistY];
					if(placementRowsSet.items[i-curDistY]->totalNumOfSites < (curInst->x-curRows->start_x+curFFLib->height-1)/curRows->width + (curDist-curDistY)){
						check = 1;
					}else{
						for(
								j = (curInst->x-curRows->start_x)/curRows->width + (curDist-curDistY);
								j < (curInst->x-curRows->start_x+curFFLib->height-1)/curRows->width + (curDist-curDistY);
								j++
								){
							if(occupy[i][j/8]&(0x01<<(j%8))){
								check = 1;
								break;
							}
						}
					}
					if(check) break;
				}
				if(!check){
					for(i = curDistY; i < placementRowsSet.count; i++){
						if(placementRowsSet.items[i-curDistY]->start_y < curY) continue;
						if(placementRowsSet.items[i-curDistY]->start_y > curFFLib->height-1+curY) break;
						curRows = placementRowsSet.items[i-curDistY];
						for(
								j = (curInst->x-curRows->start_x)/curRows->width + (curDist-curDistY);
								j < (curInst->x-curRows->start_x+curFFLib->height-1)/curRows->width + (curDist-curDistY);
								j++
								){
							occupy[i][j/8] |= 0x01<<(j%8);
						}
					}
					for(i = curDistY; i < placementRowsSet.count; i++){
						if(placementRowsSet.items[i-curDistY]->start_y < curY) continue;
						curRows = placementRowsSet.items[i-curDistY];
						curInst->y = curRows->start_y;
						curInst->x = ((curInst->x-curRows->start_x)/curRows->width + (curDist-curDistY))*curRows->width;
						break;
					}
					break;
				}
				check = 0;
				for(i = curDistY; i < placementRowsSet.count; i++){
					if(placementRowsSet.items[i-curDistY]->start_y < curY) continue;
					if(placementRowsSet.items[i-curDistY]->start_y > curFFLib->height-1+curY) break;
					curRows = placementRowsSet.items[i-curDistY];
					if((curInst->x-curRows->start_x)/curRows->width < (curDist-curDistY)){
						check = 1;
					}else{
						for(
								j = (curInst->x-curRows->start_x)/curRows->width - (curDist-curDistY);
								j < (curInst->x-curRows->start_x+curFFLib->height-1)/curRows->width - (curDist-curDistY);
								j++
								){
							if(occupy[i][j/8]&(0x01<<(j%8))){
								check = 1;
								break;
							}
						}
					}
					if(check) break;
				}
				if(!check){
					for(i = curDistY; i < placementRowsSet.count; i++){
						if(placementRowsSet.items[i-curDistY]->start_y < curY) continue;
						if(placementRowsSet.items[i-curDistY]->start_y > curFFLib->height-1+curY) break;
						curRows = placementRowsSet.items[i-curDistY];
						for(
								j = (curInst->x-curRows->start_x)/curRows->width - (curDist-curDistY);
								j < (curInst->x-curRows->start_x+curFFLib->height-1)/curRows->width - (curDist-curDistY);
								j++
								){
							occupy[i][j/8] |= 0x01<<(j%8);
						}
					}
					for(i = curDistY; i < placementRowsSet.count; i++){
						if(placementRowsSet.items[i-curDistY]->start_y < curY) continue;
						curRows = placementRowsSet.items[i-curDistY];
						curInst->y = curRows->start_y;
						curInst->x = ((curInst->x-curRows->start_x)/curRows->width - (curDist-curDistY))*curRows->width;
						break;
					}
					break;
				}
			}
			for(curDistY = 0; curDistY < curDist; curDistY++){
				check = 0;
				for(i = 0; i < placementRowsSet.count-curDistY; i++){
					if(placementRowsSet.items[i+curDistY]->start_y < curY) continue;
					if(placementRowsSet.items[i+curDistY]->start_y > curFFLib->height-1+curY) break;
					curRows = placementRowsSet.items[i+curDistY];
					if(placementRowsSet.items[i+curDistY]->totalNumOfSites < (curInst->x-curRows->start_x+curFFLib->height-1)/curRows->width + (curDist-curDistY)){
						check = 1;
					}else{
						for(
								j = (curInst->x-curRows->start_x)/curRows->width + (curDist-curDistY);
								j < (curInst->x-curRows->start_x+curFFLib->height-1)/curRows->width + (curDist-curDistY);
								j++
								){
							if(occupy[i][j/8]&(0x01<<(j%8))){
								check = 1;
								break;
							}
						}
					}
					if(check) break;
				}
				if(!check){
					for(i = 0; i < placementRowsSet.count-curDistY; i++){
						if(placementRowsSet.items[i+curDistY]->start_y < curY) continue;
						if(placementRowsSet.items[i+curDistY]->start_y > curFFLib->height-1+curY) break;
						curRows = placementRowsSet.items[i+curDistY];
						for(
								j = (curInst->x-curRows->start_x)/curRows->width + (curDist-curDistY);
								j < (curInst->x-curRows->start_x+curFFLib->height-1)/curRows->width + (curDist-curDistY);
								j++
								){
							occupy[i][j/8] |= 0x01<<(j%8);
						}
					}
					for(i = 0; i < placementRowsSet.count-curDistY; i++){
						if(placementRowsSet.items[i+curDistY]->start_y < curY) continue;
						curRows = placementRowsSet.items[i+curDistY];
						curInst->y = curRows->start_y;
						curInst->x = ((curInst->x-curRows->start_x)/curRows->width + (curDist-curDistY))*curRows->width;
						break;
					}
					break;
				}
				check = 0;
				for(i = 0; i < placementRowsSet.count-curDistY; i++){
					if(placementRowsSet.items[i+curDistY]->start_y < curY) continue;
					if(placementRowsSet.items[i+curDistY]->start_y > curFFLib->height-1+curY) break;
					curRows = placementRowsSet.items[i+curDistY];
					if((curInst->x-curRows->start_x)/curRows->width < (curDist-curDistY)){
						check = 1;
					}else{
						for(
								j = (curInst->x-curRows->start_x)/curRows->width - (curDist-curDistY);
								j < (curInst->x-curRows->start_x+curFFLib->height-1)/curRows->width - (curDist-curDistY);
								j++
								){
							if(occupy[i][j/8]&(0x01<<(j%8))){
								check = 1;
								break;
							}
						}
					}
					if(check) break;
				}
				if(!check){
					for(i = 0; i < placementRowsSet.count-curDistY; i++){
						if(placementRowsSet.items[i+curDistY]->start_y < curY) continue;
						if(placementRowsSet.items[i+curDistY]->start_y > curFFLib->height-1+curY) break;
						curRows = placementRowsSet.items[i+curDistY];
						for(
								j = (curInst->x-curRows->start_x)/curRows->width - (curDist-curDistY);
								j < (curInst->x-curRows->start_x+curFFLib->height-1)/curRows->width - (curDist-curDistY);
								j++
								){
							occupy[i][j/8] |= 0x01<<(j%8);
						}
					}
					for(i = 0; i < placementRowsSet.count-curDistY; i++){
						if(placementRowsSet.items[i+curDistY]->start_y < curY) continue;
						curRows = placementRowsSet.items[i+curDistY];
						curInst->y = curRows->start_y;
						curInst->x = ((curInst->x-curRows->start_x)/curRows->width - (curDist-curDistY))*curRows->width;
						break;
					}
					break;
				}
			}
			if(!check) break;
		}
		if(check){
			free(axis);
			for(i = 0; i < placementRowsSet.count; i++){
				free(occupy[i]);
			}
			free(binArea);
			free(ffInstNames);
			free(occupy);
			return -1;// no valid solution
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
