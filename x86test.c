
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>
#include <errno.h>

typedef struct Item {
	char *item;
	int value;
} Item_t;



	int C=0;
	int V = 0, I = 0, T = 0;
    	int WPC = 0, BC = 0, chg_stat_R = 0;
    	int current_limit_read = 0, IBAT_mA = 0;

void main() {

	Item_t items[]= {
		{"V",0};
		{"I",0};
		{"C",0};
		{"T",0};
		{"WPC",0};
		{"BC",0};
		{"chg_stat_R",0};
		{"current_limit_read",0};
		{"IBAT_mA",0};
		NULL;
	}
  
	
    	
    	char serialMsg[] = "V= 1 I=2  C=3 T=4  WPC=5 BC=6 chg_stat_R=7 current_limit_read=8 IBAT_mA=-9 *";
    	printf("RX serialMsg: %s \n", serialMsg) ; 
    	
    	int associate(char *item, char *auxStringAfterEq);

	
    	char *items[] = {"V","I","C","T",NULL,"WPC","BC","chg_stat_R","current_limit_read","IBAT_mA",NULL};
    	char **itemPtr = items;


	for (char **itemPtr = items; *itemPtr!=NULL ; itemPtr++) {
	
		char *auxString1 = strstr(serialMsg,*itemPtr); //poiter after an Item 
		char *auxStringAfterEq = 1+strstr(auxString1, "=");   //poiter after  the "=" of above Item
		
		//associate(*itemPtr,auxStringAfterEq);
	
    		printf("%s [ %d  ]  in %s\n",*itemPtr,associate(*itemPtr,auxStringAfterEq),  auxStringAfterEq);  
		
    	}
    	
        		
	printf("V = %d\n", V);	
	printf("I = %d\n", I);
	printf("C = %d\n", C);
	printf("T = %d\n", T);
}

int associate(char *item, char *auxStringAfterEq) {
	int value;
	if (strcmp(item,"V"))
		sscanf(auxStringAfterEq, "%d", &value);
	if (strcmp(item,"I"))
		sscanf(auxStringAfterEq, "%d", &value);
	if (strcmp(item,"T"))
		sscanf(auxStringAfterEq, "%d", &value);
	if (strcmp(item,"C"))
		sscanf(auxStringAfterEq, "%d", &value);
		
	return value;
		
}







