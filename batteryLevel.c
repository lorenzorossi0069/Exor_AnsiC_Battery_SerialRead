/* program to read battery level from serial data from COM2 port */
/* port is fixed to: /dev/com2 (symlink to /dev/ttymxc1) */

#define PORT "/dev/com2"
#define CHARGE_STORE "/var/volatile/tmp/charge100.txt"
#define BATT_HI 16200
#define BATT_LO 13000
//#define TOKEN "C="
#define MSG_MAX_BUF_LEN 256
#define MSG_MAX_MESSAGE_LEN 1024

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>
#include <errno.h>

int GetChargeRawValue(char* readStr);
int GetChargePercentage(int rawVal);
int writeToFile(const char *filename, int charge);

int main(int argc, char *argv[]) {
    int fd;
    struct termios tty;
    char buffer[MSG_MAX_BUF_LEN];
    char msg[MSG_MAX_MESSAGE_LEN];
    int msg_len = 0;
    ssize_t n;
    
    int chargeRaw;
    int charge100;
    int dummy=0;
    
    const char *filename = CHARGE_STORE;
    
    int firstMarkerFound = 0;
    
    // Open serial port 
    fd = open(PORT, O_RDONLY | O_NOCTTY );
    if (fd < 0) {
        perror("Error opening serial port");
        return 1;
    }

    //Get parameters
    if (tcgetattr(fd, &tty) != 0) {
        perror("tcgetattr error");
        close(fd);
        return 1;
    }

    //Serial config: 115200 baud, 8 bit, no parity, 1 stop bit
    cfsetospeed(&tty, B115200);
    cfsetispeed(&tty, B115200);

    tty.c_cflag &= ~PARENB;        // mo parity
    tty.c_cflag &= ~CSTOPB;        // 1 stop bit
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;            // 8 bit
    tty.c_cflag &= ~CRTSCTS;       // no flow control
    tty.c_cflag |= CREAD | CLOCAL; // enable RX

    tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // raw mode
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);         // no SW flow control
    tty.c_oflag &= ~OPOST;                          // raw output

    tty.c_cc[VMIN]  = 64;   // minimum number of bytes per read(), limit of 64 is due to UART HW buffer
    tty.c_cc[VTIME] = 1;    // timeout (0.1 sec units)

    // Apply configuration
    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        perror("tcsetattr error");
        close(fd);
        return 1;
    }
       
    //printf("Port %s is open. Reading data\n",PORT);

    //Reading loop till '*'
    int doRead = 1;
    while (doRead) {
        n = read(fd, buffer, sizeof(buffer) - 1);
        printf("DBG read n=%d\n",n);
        if (n > 0) {
		for (int i = 0; i < n; i++) {
		
            		/*
            		//discard received Bytes until a '*' is found and then start reading
            		if (!firstMarkerFound && buffer[i] != '*') {
				//printf(" waiting first '*'\n");
				continue;
			}
			else {
				//printf("found first '*'\n");
				firstMarkerFound = 1;
			}
			*/
            
                	if (msg_len < (int)(sizeof(msg) - 1)) {
                    		msg[msg_len++] = buffer[i];
                	}

                if (buffer[i] == '*') {
                    msg[msg_len] = '\0';
                    printf("Messaggio completo: %s\n", msg);
                    msg_len = 0; // resetta per il prossimo messaggio
                    chargeRaw=GetChargeRawValue(msg);
                    
                    charge100= GetChargePercentage(chargeRaw);
                    printf("chargeRaw = %d ==> charge100 = %d\n",chargeRaw,charge100);               
                    

                    writeToFile(filename, charge100);

                    
                    doRead=0;
                }
            }
        } else if (n < 0 && errno != EAGAIN) {
            perror("Error reading serial port");
            break; 
        }
    }

    close(fd);
    return 0;
}

// V=0 I=0 C=16914 T=65535  WPC=0 BC=0 chg_stat_R=0 current_limit_read= 150 IBAT_mA=-1 *


int GetChargeRawValue(char* readStr) {
	int C=0;
	//int V = 0, I = 0, T = 0;
    	//int WPC = 0, BC = 0, chg_stat_R = 0;
    	//int current_limit_read = 0, IBAT_mA = 0;
	
	char *saveptr;
	char *tok = strtok_r(readStr, " ", &saveptr);	
	while (tok != NULL ) {
	//printf("tok:  %s \n", tok);
	
		if (sscanf(tok, "C=%d", &C) == 1) {}
		/*else if (sscanf(tok, "V=%d", &V) == 1) {}
		else if (sscanf(tok, "I=%d", &I) == 1) {}
		else if (sscanf(tok, "T=%d", &T) == 1) {}
        	else if (sscanf(tok, "WPC=%d", &WPC) == 1) {}
        	else if (sscanf(tok, "BC=%d", &BC) == 1) {}
        	else if (sscanf(tok, "chg_stat_R=%d", &chg_stat_R) == 1) {}
        	else if (sscanf(tok, "current_limit_read=%d", &current_limit_read) == 1) {}
        	else if (sscanf(tok, "IBAT_mA=%d", &IBAT_mA) == 1) {}*/
	
		tok = strtok_r(NULL, " ", &saveptr);
	}
	
	/*
	printf("C = %d\n", C);
	
	printf("V = %d\n", V);
    	printf("I = %d\n", I);   	
    	printf("T = %d\n", T);
    	printf("WPC = %d\n", WPC);
    	printf("BC = %d\n", BC);
    	printf("chg_stat_R = %d\n", chg_stat_R);
    	printf("current_limit_read = %d\n", current_limit_read);
    	printf("IBAT_mA = %d\n", IBAT_mA);
    	*/
    	
    	return C;
}

int GetChargePercentage(int rawVal) {
	int vBatt;
	
	if (rawVal > BATT_HI)
		vBatt = 100;
	else if (rawVal <= BATT_LO)
		vBatt = 0;
	else {
		//vBatt = (rawVal*100)/BATT_HI;	
		//formula (integer numbers have no float points, so multiply before divide)
		//vBatt = (rawVal - BATT_LO)/(BATT_HI - BATT_LO)*(100-0) + 0
		vBatt = ((rawVal - BATT_LO)*100)/(BATT_HI - BATT_LO);
	}
	
	return vBatt;
}

int writeToFile(const char *filename, int charge) {
	FILE *fp;
	
	fp = fopen(filename, "w");
	if (fp == NULL) {
		printf("Error opening file %s\n",filename);
		return EXIT_FAILURE;
	}
	
	if (fprintf(fp,"%d\n", charge) < 0) {
		printf("Error writing to file %s\n",filename);
		return EXIT_FAILURE;
	}
	
	if (fflush(fp) != 0) {
		printf("Error flushing file %s\n",filename);
		return EXIT_FAILURE;
	}
	
	if (fclose(fp) != 0) {
		printf("Error closing file %s\n",filename);
		return EXIT_FAILURE;
	} 
}



