#include "jsmn.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Global variables 
int sequenceID;
    
int color;
int x_dest;

int action;
int dist;
int intensity;

static const char *JSON_STRING =
	//" { seq : 102, color : 0, x_dest : 5 } ";
	" { seq : 105, action : 0, dist : 12, intensity : 75 } ";

// Functions
static int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
	if (tok->type == JSMN_STRING && (int) strlen(s) == tok->end - tok->start &&
			strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
		return 0;
	}
	return -1;
}

void parseJSON () {

	int r;
	jsmn_parser p;
	jsmntok_t t[128]; /* We expect no more than 128 tokens */

	jsmn_init(&p);
	r = jsmn_parse(&p, JSON_STRING, strlen(JSON_STRING), t, sizeof(t)/sizeof(t[0]));
	if (r < 0) {
		printf("Failed to parse JSON: %d\n", r); // DEBUG
	}

	/* Assume the top-level element is an object */
	if (r < 1 || t[0].type != JSMN_OBJECT) {
		printf("Object expected\n"); // DEBUG
	}

	printf("Size: %d\n", r); // DEBUG

	jsmntok_t key = t[2];
	int length = key.end - key.start;
	char keyString[length + 1];    
	memcpy(keyString, &JSON_STRING[key.start], length);
	keyString[length] = '\0';
	sequenceID = atoi(keyString);
	printf("seq: %d\n", sequenceID); // DEBUG

	if(r == 7) { // Next task to complete
		
		key = t[4];
		length = key.end - key.start;
		keyString[length + 1];    
		memcpy(keyString, &JSON_STRING[key.start], length);
		keyString[length] = '\0';
		color = atoi(keyString);
		printf("color: %d\n", color); // DEBUG

		key = t[6];
		length = key.end - key.start;
		keyString[length + 1];    
		memcpy(keyString, &JSON_STRING[key.start], length);
		keyString[length] = '\0';
		x_dest = atoi(keyString);
		printf("x_dest: %d\n", x_dest); // DEBUG
	}

	else if (r == 9) { // Action

		key = t[4];
		length = key.end - key.start;
		keyString[length + 1];    
		memcpy(keyString, &JSON_STRING[key.start], length);
		keyString[length] = '\0';
		action = atoi(keyString);
		printf("action: %d\n", action); // DEBUG

		key = t[6];
		length = key.end - key.start;
		keyString[length + 1];    
		memcpy(keyString, &JSON_STRING[key.start], length);
		keyString[length] = '\0';
		dist = atoi(keyString);
		printf("dist: %d\n", dist); // DEBUG

		key = t[8];
		length = key.end - key.start;
		keyString[length + 1];    
		memcpy(keyString, &JSON_STRING[key.start], length);
		keyString[length] = '\0';
		intensity = atoi(keyString);
		printf("intensity: %d\n", intensity); // DEBUG
	}

	else {
		printf("Invalid message!\n"); // DEBUG
	}
}

int main () {

    parseJSON();    

}