#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"

/* run this program using the console pauser or add your own getch, system("pause") or input loop */

struct JCNode{
	struct JCNode* parent;
	struct JCNode* children;
	struct JCNode* next;
	char* key;
	int templateIndex;
	int link;
};

struct JCSlist{
	char* value;
	struct JCSlist* next;
};

int JCIndex = 1;

struct JCNode* follow(struct JCNode* root, char* key){ 
	
	struct JCNode* child = root->children;
	struct JCNode* prev = child;
	while(child){
		if(strcmp(child->key, key) == 0){
			return child;
		}
		else {
			prev = child;
			child = child->next;
		}
	} 
	if(!prev){
		child = malloc(sizeof(struct JCNode));
		child->parent = root;
		child->key = key;
		child->children = NULL;
		child->next = NULL;
		root->children = child;
	}
	else{
		prev->next = malloc(sizeof(struct JCNode));
		prev->next->parent = root;
		prev->next->key = key;
		prev->next->children = NULL;
		prev->next->next = NULL; 
		child = prev->next;
	}
	return child; 
}

void process(struct JCNode* root, cJSON* value){
	struct JCNode* node = root;
	if(value){
		while(value){
			if(value->string){
				node = follow(node, value->string);
			}
			if(value->child){
				process(root, value->child);
			} 
			value = value->next;
		}
	}
}

void setLink(struct JCNode* root, int link){
	while(root){
		root->templateIndex = JCIndex;
		root->link = link;
		if(root->children){
			if(root->children->next){
				JCIndex += 1;
			}
			setLink(root->children, root->templateIndex);
		}
		root = root->next;
		if(root){
			JCIndex += 1; 
		}
	}
}

void getLink(struct JCNode* root, int *array){
	while(root){
		if(root->templateIndex != root->link){
			array[root->templateIndex - 1] = root->link;
		}
		if(root->children){
			getLink(root->children, array);
		}
		root = root->next;
	}
}

void getKeyByIndex(struct JCNode* root, int i, struct JCSlist* list){
	if(root){
		while(root && root->templateIndex == i){
			struct JCSlist* str = malloc(sizeof(struct JCSlist));
			str->next = NULL;
			str->value = root->key;
			list->next = str;
			list = list->next;
			root = root->children;
		}
		if(root){
			getKeyByIndex(root->next, i, list);
			getKeyByIndex(root->children, i, list);
		} 
	} 
}

cJSON* createTemplates(struct JCNode* root){
	
	int* links;
	links = (int *)malloc(sizeof(int)*JCIndex);
	getLink(root, links);
	
	cJSON* templates;
	
	templates = cJSON_CreateArray();
	int i;
	for(i = 0; i < JCIndex; i++){
		struct JCSlist* list = malloc(sizeof(struct JCSlist));
		list->value = "";
		list->next = NULL;
		getKeyByIndex(root, i+1, list);
		list = list->next;
		
		cJSON* array = cJSON_CreateArray();
		cJSON_AddItemToArray(array, cJSON_CreateNumber(links[i]));
		while(list){
			cJSON_AddItemToArray(array, cJSON_CreateString(list->value));
			list = list->next;
		}
		cJSON_AddItemToArray(templates, array);
		free(list);
	}
	free(links);
	return templates;  
}

int findKeyNum(struct JCNode* root, char* lastKey){
	if(!root || !lastKey){
		return 0;
	}
	if(strcmp(root->key, lastKey) == 0){
		return root->templateIndex;
	}
	return findKeyNum(root->children, lastKey) | findKeyNum(root->next, lastKey);
}

cJSON* createValues(cJSON* value, struct JCNode* root){
	cJSON* result = cJSON_CreateObject();
	
	char* lastKey = "";
	cJSON* array = cJSON_CreateArray();
	
	if(value){
		while(value){
			if(value->child){
				cJSON_AddItemToArray(array, createValues(value->child, root));
			}
			else if(value->valuestring){
				cJSON_AddItemToArray(array, cJSON_CreateString(value->valuestring));
			} 
			lastKey = value->string;
			value = value->next;
		}
	}
	
	int num = findKeyNum(root, lastKey);
	cJSON_InsertItemInArray(array, 0, cJSON_CreateNumber(num));
	result = array;
	return result;
}

int initJCNode(struct JCNode* root, cJSON* value){
	
	root->parent = NULL;
	root->key = "";
	root->children = NULL;
	root->templateIndex = 0;
	root->link = 0;
	
	process(root, value);
	setLink(root->children, 0);
	return 0;
}

cJSON* Compress(cJSON* value){
	JCIndex = 1; 
	struct JCNode* root = malloc(sizeof(struct JCNode));
	int index = initJCNode(root, value);
	
	cJSON* templates = createTemplates(root->children);
	cJSON* values = createValues(value, root->children);
	
	free(root);
	if(templates){
		cJSON* comRes;
		
		comRes = cJSON_CreateObject();
		cJSON_AddItemToObject(comRes, "t", templates);
		cJSON_AddItemToObject(comRes, "v", values);
		
		return comRes;
	}
	else{
		return value;
	}
}

void getKeys(cJSON* templates, struct JCSlist** keys, int KeySize){
	int i, j;
	for(i = 0; i < KeySize; i++){
		cJSON* key = cJSON_GetArrayItem(templates, i);
		int itemSize = cJSON_GetArraySize(key);
		cJSON* item = cJSON_GetArrayItem(key, 0);
		int t = item->valueint;
		struct JCSlist* curr = keys[i];
		if(t){
			struct JCSlist* prev = keys[t-1]->next;
			while(prev){
				struct JCSlist *tmp = malloc(sizeof(struct JCSlist));
				tmp->value = prev->value;
				tmp->next = NULL;
				curr->next = tmp;
				curr = curr->next;
				prev = prev->next;
			}
		}
		for(j = 1; j < itemSize; j++){
			item = cJSON_GetArrayItem(key, j);
			struct JCSlist *tmp = malloc(sizeof(struct JCSlist));
			tmp->value = item->valuestring;
			tmp->next = NULL;
			curr->next = tmp;
			curr = curr->next;
		}
	}
	
}

cJSON* getValues(struct JCSlist** keys, const cJSON* values){
	
	int i;
	cJSON* result;
	
	char* Valtext = cJSON_Print(values);
	int size = cJSON_GetArraySize(values);
	cJSON* item = cJSON_GetArrayItem(values, 0);
	int keyNum = item->valueint;

	if(keyNum){
		result = cJSON_CreateObject();
		struct JCSlist* list = keys[keyNum - 1]->next;
		for(i = 1; i < size; i++){ 
			cJSON* item = cJSON_GetArrayItem(values, i);
			int ssize = cJSON_GetArraySize(item);
			char* itemtext = cJSON_Print(item);
			cJSON* realItem = cJSON_Parse(itemtext);
			if(ssize){
				cJSON_AddItemToObject(result, list->value, getValues(keys, realItem));
			}
			else{
				cJSON_AddItemToObject(result, list->value, realItem);
			}
			if(list->next){
				list = list->next;
			}
		}
	}
	else{
		result = cJSON_CreateArray();
		for(i = 1; i < size; i++){
			item = cJSON_GetArrayItem(values, i);
			int ssize = cJSON_GetArraySize(item);
			char* itemtext = cJSON_Print(item);
			cJSON* realItem = cJSON_Parse(itemtext);
			if(ssize){
				cJSON_AddItemToArray(result, getValues(keys, realItem));
			}
			else{
				cJSON_AddItemToArray(result, realItem);
			}
		}
	}
	return result;
}

cJSON* expand(cJSON* templates, cJSON* values){
	
	int i;
	
	int KeySize = cJSON_GetArraySize(templates);
	struct JCSlist** keys;
	keys =  malloc(sizeof(struct JCSlist*)*KeySize);
	for(i = 0; i < KeySize; i++){
		keys[i] = malloc(sizeof(struct JCSlist));
		keys[i]->value = "";
		keys[i]->next = NULL;
	}
	getKeys(templates, keys, KeySize);
	cJSON* result = getValues(keys, values);
	char* comtext = cJSON_Print(result);
	
	for(i = 0; i < KeySize; i++){
		free(keys[i]);
	}
	free(keys);
	return result;
}

cJSON* Expand(cJSON* json){
	cJSON* templates = json->child;
	cJSON* values =  templates->next;
	return expand(templates, values);
}

int main(int argc, char *argv[]) {
	
	char* uncom = "{\"Alert\":[{\"type\":\"KBMouse\",\"data\":\"LOC_OPTIONS_KEY_V\",\"information\":{\"name\":\"xuzhi\",\"number\":\"123456789\"}}],\"Attack\":[{\"type\":\"KBMouse\",\"data\":\"LOC_OPTIONS_KEY_A\",\"dream\":\"superman\"}],\"AutoExplore\":[{\"type\":\"KBMouse\",\"data\":\"LOC_OPTIONS_KEY_E\"},{\"type\":\"KBCat\",\"data\":\"LOC_OPTIONS_KEY_C\"}],\"AutoFind\":\"test\"}";
	cJSON *json = cJSON_Parse(uncom);
	json = json->child;
	cJSON* com = Compress(json);
	char* comtext = cJSON_Print(com);
	printf("comJson: %s\n", comtext);
	cJSON* decom = Expand(com);
	char* decomtext = cJSON_Print(decom);
	printf("decomJson: %s\n", decomtext);
	
	char* comStr = "{\"t\":[[0,\"Alert\",\"Attack\",\"AutoExplore\",\"AutoFind\"],[0,\"type\",\"data\"],[2,\"information\"],[2,\"dream\"],[0,\"name\",\"number\"]],\"v\":[1,[0,[3,\"KBMouse\",\"LOC_OPTIONS_KEY_V\",[5,\"xuzhi\",\"123456789\"]]],[0,[4,\"KBMouse\",\"LOC_OPTIONS_KEY_A\",\"superman\"]],[0,[2,\"KBMouse\",\"LOC_OPTIONS_KEY_E\"],[2,\"KBCat\",\"LOC_OPTIONS_KEY_C\"]],\"test\"]}";
	cJSON* comcJS = cJSON_Parse(comStr);
	cJSON* decomcJS = Expand(comcJS);
	char* decomStr = cJSON_Print(decomcJS);
	printf("decomcJS: %s\n", decomStr);
	
	cJSON_Delete(com);
	cJSON_Delete(decom);
	return 0;
}
