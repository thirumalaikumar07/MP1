#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#define strcasecmp _stricmp
#else
#include <unistd.h>
#include <strings.h>
#endif

#define MAX_SEATS 20

// ------------------ AIRLINE SYSTEM ------------------
typedef struct Passenger {
    int passengerID;
    char name[50];
    int age;
    char from[50];
    char to[50];
    char classType[20];
    char tripType[20];
    int flightID;
    float amount;
    int isStudent;
    float discountPercent;
    char seatNumber[10];
    int seatIndex; // new: store which seat index
    char username[30];
    char password[30];
    struct Passenger *next;
} Passenger;

typedef struct Flight {
    int flightID;
    char flightName[30];
    char route[60];
    int bookedSeats;
    int seatTaken[MAX_SEATS]; // 0 = free, 1 = taken
} Flight;

Flight flights[4] = {
    {101, "Air India", "Chennai → Delhi", 0, {0}},
    {102, "IndiGo", "Coimbatore → Mumbai", 0, {0}},
    {103, "SpiceJet", "Bangalore → Kolkata", 0, {0}},
    {104, "Vistara", "Madurai → Hyderabad", 0, {0}}
};

Passenger *head = NULL;
int nextPassengerID = 1;

// ------------------ UTILITY FUNCTIONS ------------------
void readLine(char *b, int s){
    if (!fgets(b, s, stdin)) { b[0]=0; return; }
    b[strcspn(b,"\n")] = 0;
}

int readInt() {
    char buf[50];
    int x;
    while (1) {
        if (!fgets(buf,50,stdin)) return 0;
        if (sscanf(buf,"%d",&x)==1) return x;
        printf("Invalid input! Enter again: ");
    }
}

int isValidName(char *name) {
    if (name[0]=='\0') return 0;
    for(int i=0;name[i];i++)
        if(!isalpha((unsigned char)name[i]) && name[i]!=' ') return 0;
    return 1;
}

int validateClass(char *c){
    return (!strcasecmp(c,"Economy") || !strcasecmp(c,"Business") || !strcasecmp(c,"First"));
}

int validateTrip(char *t){
    return (!strcasecmp(t,"Single") || !strcasecmp(t,"Round") || !strcasecmp(t,"Multi"));
}

void generateSeatNumberFromIndex(int index, char *seat){
    int n = index + 1;
    int row = (n-1)/4 + 1;
    char col = 'A' + (n-1)%4;
    sprintf(seat,"%d%c", row, col);
}

float calculateAmount(char *c, char *t, int st, int age, float *disc){
    float base=0;
    if(!strcasecmp(c,"Economy")) base=3000;
    else if(!strcasecmp(c,"Business")) base=6000;
    else base=9000;

    float f=1;
    if(!strcasecmp(t,"Round")) f=1.8;
    else if(!strcasecmp(t,"Multi")) f=2.5;

    float p=base*f;
    *disc=0;

    if(st){ p*=0.8; *disc+=20; }
    if(age<12){ p*=0.5; *disc+=50; }
    else if(age>=60){ p*=0.7; *disc+=30; }

    return p;
}

void showLoading(){
    printf("Processing Payment:\n");
    for(int i=0;i<=20;i++){
        printf("\r[");
        for(int j=0;j<i;j++) printf("█");
        for(int j=i;j<20;j++) printf("░");
        printf("] %d%%",i*5);
        fflush(stdout);
#ifdef _WIN32
        Sleep(40);
#else
        usleep(40000);
#endif
    }
    printf("\nPayment Successful ✅\n");
}

Flight *findFlight(int id){
    for(int i=0;i<4;i++) if(flights[i].flightID==id) return &flights[i];
    return NULL;
}

// ------------------ TICKET & BILL ------------------
void printTicket(Passenger *p){
    char file[50];
    sprintf(file,"ticket_%d.txt",p->passengerID);
    FILE *fp=fopen(file,"w");
    if(!fp){
        printf("Error creating ticket!\n");
        return;
    }

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    fprintf(fp,"----- FLIGHT TICKET -----\n");
    fprintf(fp,"Passenger ID : %d\n",p->passengerID);
    fprintf(fp,"Name         : %s\n",p->name);
    fprintf(fp,"Seat No      : %s\n",p->seatNumber);
    fprintf(fp,"From         : %s\n",p->from);
    fprintf(fp,"To           : %s\n",p->to);
    fprintf(fp,"Flight ID    : %d\n",p->flightID);
    fprintf(fp,"Amount       : %.2f\n",p->amount);
    fprintf(fp,"Date & Time  : %04d-%02d-%02d %02d:%02d:%02d\n",
            tm.tm_year+1900, tm.tm_mon+1, tm.tm_mday,
            tm.tm_hour, tm.tm_min, tm.tm_sec);

    fclose(fp);
    printf("Ticket Generated: %s ✅\n",file);
}

void printBill(Passenger *p) {
    printf("\n---------------- FLIGHT BILL ----------------\n");
    printf("Passenger ID   : %d\n", p->passengerID);
    printf("Name           : %s\n", p->name);
    printf("Age            : %d\n", p->age);
    printf("From           : %s\n", p->from);
    printf("To             : %s\n", p->to);
    printf("Flight ID      : %d\n", p->flightID);
    printf("Class          : %s\n", p->classType);
    printf("Trip Type      : %s\n", p->tripType);
    printf("Seat Number    : %s\n", p->seatNumber);
    printf("----------------------------------------------\n");
    printf("Base Amount    : ₹%.2f\n", p->amount / (1 - (p->discountPercent / 100)));
    printf("Discount       : %.0f%%\n", p->discountPercent);
    printf("Final Amount   : ₹%.2f\n", p->amount);
    printf("----------------------------------------------\n");
    printf("✅ BILL GENERATED SUCCESSFULLY\n");
    printf("----------------------------------------------\n");
}

// ------------------ BOOKING ------------------
void bookSeat(){
    printf("\nFlights:\n");
    for(int i=0;i<4;i++)
        printf("%d. %s (%s) Seats Left: %d\n",
            flights[i].flightID, flights[i].flightName,
            flights[i].route, MAX_SEATS-flights[i].bookedSeats);

    printf("Enter Flight ID: ");
    int id = readInt();

    Flight *f = findFlight(id);
    if(!f){ printf("Invalid Flight!\n"); return; }

    char from[50],to[50];
    printf("From: "); readLine(from,50);
    printf("To  : "); readLine(to,50);

    char rt[150];
    sprintf(rt,"%s → %s",from,to);

    if(strcasecmp(rt,f->route)){
        printf("Route mismatch!\n");
        return;
    }

    printf("How many passengers? ");
    int n=readInt();

    if(f->bookedSeats+n>MAX_SEATS){
        printf("Not enough seats!\n");
        return;
    }

    while(n--){
        Passenger *p = malloc(sizeof(Passenger));
        memset(p,0,sizeof(Passenger));

        p->passengerID = nextPassengerID++;
        p->flightID = id;
        strcpy(p->from,from);
        strcpy(p->to,to);

        while(1){
            printf("Passenger Name: ");
            readLine(p->name,50);
            if(isValidName(p->name)) break;
            printf("Invalid Name!\n");
        }

        while(1){
            printf("Age: ");
            p->age = readInt();
            if(p->age>0 && p->age<=120) break;
            printf("Invalid age!\n");
        }

        while(1){
            printf("Class (Economy/Business/First): ");
            readLine(p->classType,20);
            if(validateClass(p->classType)) break;
            printf("Invalid class!\n");
        }

        while(1){
            printf("Trip (Single/Round/Multi): ");
            readLine(p->tripType,20);
            if(validateTrip(p->tripType)) break;
        }

        printf("Student? 1/0: ");
        p->isStudent = readInt();

        p->amount = calculateAmount(p->classType,p->tripType,
                                    p->isStudent,p->age,&p->discountPercent);

        // ------------------ Find first free seat ------------------
        int seatIndex = -1;
        for(int i=0; i<MAX_SEATS; i++){
            if(f->seatTaken[i] == 0){
                seatIndex = i;
                f->seatTaken[i] = 1;
                break;
            }
        }
        if(seatIndex == -1){
            printf("No available seat found!\n");
            free(p);
            return;
        }
        p->seatIndex = seatIndex;
        generateSeatNumberFromIndex(seatIndex, p->seatNumber);

        // ------------------ Create Login ------------------
        printf("Create username for login: ");
        readLine(p->username,30);
        printf("Create password: ");
        readLine(p->password,30);

        showLoading();

        p->next=head;
        head=p;
        f->bookedSeats++;

        printf("✅ Booking Successful for %s!\n",p->name);
        printf("Seat: %s | Amount: %.2f | Discount: %.0f%%\n",
               p->seatNumber,p->amount,p->discountPercent);

        printTicket(p);
        printBill(p);
    }
}

// ------------------ CANCEL SEAT ------------------
void cancelSeat(){
    printf("Enter Passenger ID: ");
    int id = readInt();

    Passenger *p=head,*prev=NULL;

    while(p){
        if(p->passengerID==id){
            Flight *f = findFlight(p->flightID);
            if(f){
                f->seatTaken[p->seatIndex] = 0; // free that seat
                f->bookedSeats--;
            }

            if(prev) prev->next = p->next;
            else head = p->next;

            printf("\n✅ Ticket Cancelled Successfully!\n");
            printf("Passenger: %s | Seat: %s is now available.\n",p->name,p->seatNumber);

            free(p);
            return;
        }
        prev=p;
        p=p->next;
    }

    printf("❌ Passenger ID Not Found!\n");
}

// ------------------ VIEW ALL PASSENGERS ------------------
void viewAllPassengers() {
    if (!head) {
        printf("\nNo passengers booked yet!\n");
        return;
    }

    printf("\n----- ALL BOOKED PASSENGERS -----\n");
    Passenger* p = head;
    while(p) {
        printf("Passenger ID : %d\n", p->passengerID);
        printf("Name         : %s\n", p->name);
        printf("Age          : %d\n", p->age);
        printf("From         : %s\n", p->from);
        printf("To           : %s\n", p->to);
        printf("Flight ID    : %d\n", p->flightID);
        printf("Seat No      : %s\n", p->seatNumber);
        printf("Class        : %s\n", p->classType);
        printf("Trip Type    : %s\n", p->tripType);
        printf("Amount       : %.2f\n", p->amount);
        printf("--------------------------------\n");
        p = p->next;
    }
}

// ------------------ PASSENGER LOGIN ------------------
Passenger* passengerLogin(){
    char uname[30], pass[30];
    printf("----- PASSENGER LOGIN -----\n");

    for(int attempts=3; attempts>0; attempts--){
        printf("Username: "); readLine(uname,30);
        printf("Password: "); readLine(pass,30);

        Passenger* p = head;
        while(p){
            if(strcmp(p->username,uname)==0 && strcmp(p->password,pass)==0){
                printf("✅ Login Successful! Welcome %s\n", p->name);
                return p;
            }
            p=p->next;
        }

        printf("❌ Invalid credentials! Attempts left: %d\n", attempts-1);
    }

    return NULL;
}

void passengerMenu(Passenger* p){
    while(1){
        printf("\n----- PASSENGER MENU -----\n");
        printf("1. View Ticket\n");
        printf("2. View Bill\n");
        printf("3. Exit\n");
        printf("Enter Choice: ");
        int c = readInt();

        switch(c){
            case 1: printTicket(p); break;
            case 2: printBill(p); break;
            case 3: return;
            default: printf("Invalid choice!\n");
        }
    }
}

// ------------------ MAIN MENU ------------------
int main(){
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif

    while(1){
        printf("\n----- AIRLINE SYSTEM MAIN MENU -----\n");
        printf("1. Book Seat (Create Account)\n");
        printf("2. Passenger Login\n");
        printf("3. View Available Seats\n");
        printf("4. Cancel Seat\n");
        printf("5. View All Passengers\n");
        printf("6. Exit\n");
        printf("Enter Choice: ");

        int c=readInt();

        switch(c){
            case 1: bookSeat(); break;
            case 2: {
                Passenger* p = passengerLogin();
                if(p) passengerMenu(p);
                else printf("❌ Login Failed!\n");
            } break;
            case 3:
                for(int i=0;i<4;i++)
                    printf("%d - %s : %d seats left\n",
                        flights[i].flightID, flights[i].flightName,
                        MAX_SEATS-flights[i].bookedSeats);
                break;
            case 4: cancelSeat(); break;
            case 5: viewAllPassengers(); break;
            case 6: exit(0);
            default: printf("Invalid Choice!\n");
        }
    }
}
