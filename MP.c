#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SEATS 20

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
    char collegeName[100];
    char collegeRoll[20];
    char collegeDistrict[50];
    char collegeState[50];

    // Payment details
    char paymentMethod[30];
    char upiID[50];
    char bankName[50];
    char accountNumber[30];
    char ifscCode[20];

    struct Passenger *next;
} Passenger;

typedef struct Flight {
    int flightID;
    char flightName[30];
    char route[60];
    int bookedSeats;
} Flight;

/* initial flights */
Flight flights[4] = {
    {101, "Air India", "Chennai → Delhi", 0},
    {102, "IndiGo", "Coimbatore → Mumbai", 0},
    {103, "SpiceJet", "Bangalore → Kolkata", 0},
    {104, "Vistara", "Madurai → Hyderabad", 0}
};

Passenger *head = NULL;
int nextPassengerID = 1;

/* -------------------- Input helpers -------------------- */
void readLine(char *buf, int size) {
    if(fgets(buf, size, stdin) == NULL) {
        buf[0] = '\0';
        return;
    }
    buf[strcspn(buf, "\n")] = '\0';
}

/* read integer robustly using fgets+sscanf */
int readInt() {
    char tmp[128];
    int val = 0;
    while(1) {
        if(fgets(tmp, sizeof(tmp), stdin) == NULL) return 0;
        if(sscanf(tmp, "%d", &val) == 1) return val;
        printf("Invalid input. Enter a number: ");
    }
}

/* -------------------- Fare calc -------------------- */
float calculateAmount(char *classType, char *tripType, int isStudent, int age) {
    float base = 0;
    if(strcmp(classType,"Economy")==0) base = 3000;
    else if(strcmp(classType,"Business")==0) base = 6000;
    else if(strcmp(classType,"First")==0) base = 9000;
    else base = 3000; // default

    float factor = 1;
    if(strcmp(tripType,"Round")==0) factor = 1.8;
    else if(strcmp(tripType,"Multi")==0) factor = 2.5;

    float total = base * factor;

    if(isStudent) total *= 0.8;

    if(age < 12) total *= 0.5;
    else if(age >= 60) total *= 0.7;

    return total;
}

/* -------------------- Flight find -------------------- */
Flight* findFlight(int id) {
    for(int i=0;i<4;i++)
        if(flights[i].flightID==id) return &flights[i];
    return NULL;
}

/* -------------------- Booking -------------------- */
void bookSeat() {
    int flightID;
    char from[50], to[50];

    printf("\nAvailable Flights:\n");
    for(int i=0;i<4;i++)
        printf("%d. %s (%s) [Seats Left: %d]\n",
               flights[i].flightID, flights[i].flightName,
               flights[i].route, MAX_SEATS - flights[i].bookedSeats);

    printf("\nEnter Flight ID to book: ");
    flightID = readInt();
    Flight *f = findFlight(flightID);
    if(!f){ printf("Invalid Flight ID!\n"); return; }

    printf("Enter FROM place: ");
    readLine(from, sizeof(from));
    printf("Enter TO place: ");
    readLine(to, sizeof(to));

    char fullRoute[100];
    snprintf(fullRoute, sizeof(fullRoute), "%s → %s", from, to);

    if(strcmp(f->route, fullRoute)!=0){
        printf("Flight route mismatch! Booking cancelled.\n");
        return;
    }

    printf("Enter number of passengers: ");
    int numPassengers = readInt();

    if(f->bookedSeats + numPassengers > MAX_SEATS){
        printf("Not enough seats available! Only %d seats left.\n",
               MAX_SEATS - f->bookedSeats);
        return;
    }

    for(int i=0;i<numPassengers;i++){
        Passenger *p = (Passenger*)malloc(sizeof(Passenger));
        if(!p){ printf("Memory error\n"); return; }
        memset(p, 0, sizeof(Passenger)); // initialize
        p->passengerID = nextPassengerID++;
        p->flightID = flightID;
        strncpy(p->from, from, sizeof(p->from)-1);
        strncpy(p->to, to, sizeof(p->to)-1);

        printf("\nPassenger %d Name: ", i+1);
        readLine(p->name, sizeof(p->name));

        printf("Age: ");
        p->age = readInt();

        printf("Class (Economy/Business/First): ");
        readLine(p->classType, sizeof(p->classType));

        printf("Trip Type (Single/Round/Multi): ");
        readLine(p->tripType, sizeof(p->tripType));

        printf("Are you a Student? (1-Yes / 0-No): ");
        p->isStudent = readInt();

        if(p->isStudent){
            printf("College Name: ");
            readLine(p->collegeName, sizeof(p->collegeName));

            printf("Roll No: ");
            readLine(p->collegeRoll, sizeof(p->collegeRoll));

            printf("District: ");
            readLine(p->collegeDistrict, sizeof(p->collegeDistrict));

            printf("State: ");
            readLine(p->collegeState, sizeof(p->collegeState));
        } else {
            strcpy(p->collegeName,"-");
            strcpy(p->collegeRoll,"-");
            strcpy(p->collegeDistrict,"-");
            strcpy(p->collegeState,"-");
        }

        p->amount = calculateAmount(p->classType, p->tripType, p->isStudent, p->age);

        /* Payment details */
        printf("\nSelect Payment Method:\n");
        printf("1. Google Pay\n2. Paytm\n3. Net Banking\nEnter choice: ");
        int payChoice = readInt();

        switch(payChoice) {
            case 1:
                strncpy(p->paymentMethod, "Google Pay", sizeof(p->paymentMethod)-1);
                printf("Enter UPI ID (example: name@bank): ");
                readLine(p->upiID, sizeof(p->upiID));
                strcpy(p->bankName, "-"); strcpy(p->accountNumber, "-"); strcpy(p->ifscCode, "-");
                break;
            case 2:
                strncpy(p->paymentMethod, "Paytm", sizeof(p->paymentMethod)-1);
                printf("Enter Paytm UPI ID (example: mobile@paytm): ");
                readLine(p->upiID, sizeof(p->upiID));
                strcpy(p->bankName, "-"); strcpy(p->accountNumber, "-"); strcpy(p->ifscCode, "-");
                break;
            case 3:
                strncpy(p->paymentMethod, "Net Banking", sizeof(p->paymentMethod)-1);
                printf("Enter Bank Name: ");
                readLine(p->bankName, sizeof(p->bankName));
                printf("Enter Account Number: ");
                readLine(p->accountNumber, sizeof(p->accountNumber));
                printf("Enter IFSC Code: ");
                readLine(p->ifscCode, sizeof(p->ifscCode));
                strcpy(p->upiID, "-");
                break;
            default:
                strncpy(p->paymentMethod, "Unknown", sizeof(p->paymentMethod)-1);
                strcpy(p->upiID, "-"); strcpy(p->bankName, "-"); strcpy(p->accountNumber, "-"); strcpy(p->ifscCode, "-");
                break;
        }

        printf("\nProcessing payment via %s...\n", p->paymentMethod);
        printf("Payment of ₹%.2f successful!\n", p->amount);

        f->bookedSeats++;
        p->next = head;
        head = p;

        printf("Passenger %d booked successfully. Amount: ₹%.2f\n", i+1, p->amount);
    }

    printf("\nAll %d passengers booked successfully!\n", numPassengers);
}

/* -------------------- Cancel -------------------- */
void cancelSeat() {
    printf("Enter Passenger ID to cancel: ");
    int id = readInt();

    Passenger *prev=NULL,*curr=head;
    while(curr){
        if(curr->passengerID==id){
            Flight *f=findFlight(curr->flightID);
            if(f) f->bookedSeats--;
            if(prev) prev->next = curr->next;
            else head = curr->next;
            free(curr);
            printf("Booking Cancelled!\n");
            return;
        }
        prev=curr;
        curr=curr->next;
    }
    printf("Passenger ID not found!\n");
}

/* -------------------- View seats -------------------- */
void viewAvailableSeats() {
    printf("\nAvailable Seats per Flight:\n");
    for(int i=0;i<4;i++)
        printf("%d - %s : %d seats left\n",
               flights[i].flightID, flights[i].flightName,
               MAX_SEATS - flights[i].bookedSeats);
}

/* -------------------- Display list -------------------- */
void displayPassengerList() {
    if(!head){ printf("No passengers booked yet.\n"); return; }
    Passenger *p=head;
    printf("\nPassenger List:\n");
    while(p){
        printf("\nID:%d | Name:%s | Age:%d | Flight:%d | From:%s | To:%s\n",
               p->passengerID,p->name,p->age,p->flightID,p->from,p->to);
        printf("Class:%s | Trip:%s | Amount:₹%.2f\n",
               p->classType,p->tripType,p->amount);
        printf("Payment Method: %s\n", p->paymentMethod);

        if(strcmp(p->paymentMethod,"Net Banking")==0)
            printf("Bank: %s | Acc No: %s | IFSC: %s\n", p->bankName, p->accountNumber, p->ifscCode);
        else if(strcmp(p->paymentMethod,"Google Pay")==0 || strcmp(p->paymentMethod,"Paytm")==0)
            printf("UPI ID: %s\n", p->upiID);

        if(p->isStudent)
            printf("Student: YES | College:%s | Roll:%s | District:%s | State:%s\n",
                   p->collegeName,p->collegeRoll,p->collegeDistrict,p->collegeState);
        else
            printf("Student: NO\n");
        p=p->next;
    }
}

/* -------------------- Main -------------------- */
int main() {
    while(1){
        printf("\n==== Airline Seat Reservation ====\n");
        printf("1. Book Seat\n2. Cancel Booking\n3. View Available Seats\n4. Display Passenger List\n5. Exit\nEnter choice: ");
        int choice = readInt();
        switch(choice){
            case 1: bookSeat(); break;
            case 2: cancelSeat(); break;
            case 3: viewAvailableSeats(); break;
            case 4: displayPassengerList(); break;
            case 5: exit(0);
            default: printf("Invalid choice!\n");
        }
    }
    return 0;
}
