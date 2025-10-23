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

    struct Passenger *next;
} Passenger;

typedef struct Flight {
    int flightID;
    char flightName[30];
    char route[60];
    int bookedSeats;
} Flight;

Flight flights[4] = {
    {101, "Air India", "Chennai → Delhi", 0},
    {102, "IndiGo", "Coimbatore → Mumbai", 0},
    {103, "SpiceJet", "Bangalore → Kolkata", 0},
    {104, "Vistara", "Madurai → Hyderabad", 0}
};

Passenger *head = NULL;
int nextPassengerID = 1;

float calculateAmount(char *classType, char *tripType, int isStudent) {
    float base = 0;
    if(strcmp(classType,"Economy")==0) base = 3000;
    else if(strcmp(classType,"Business")==0) base = 6000;
    else if(strcmp(classType,"First")==0) base = 9000;

    float factor = 1;
    if(strcmp(tripType,"Round")==0) factor = 1.8;
    else if(strcmp(tripType,"Multi")==0) factor = 2.5;

    float total = base * factor;
    if(isStudent) total *= 0.8;
    return total;
    
}

Flight* findFlight(int id) {
    for(int i=0;i<4;i++)
        if(flights[i].flightID==id) return &flights[i];
    return NULL;
}

void bookSeat() {
    int flightID;
    char from[50], to[50];
    printf("\nAvailable Flights:\n");
    for(int i=0;i<4;i++)
        printf("%d. %s (%s) [Seats Left: %d]\n",
               flights[i].flightID, flights[i].flightName,
               flights[i].route, MAX_SEATS - flights[i].bookedSeats);

    printf("\nEnter Flight ID to book: ");
    scanf("%d",&flightID);
    Flight *f = findFlight(flightID);
    if(!f){ printf("Invalid Flight ID!\n"); return; }

    getchar(); // clear newline
    printf("Enter FROM place: ");
    fgets(from,sizeof(from),stdin);
    from[strcspn(from,"\n")] = 0;
    printf("Enter TO place: ");
    fgets(to,sizeof(to),stdin);
    to[strcspn(to,"\n")] = 0;

    char fullRoute[100];
    /* ✅ FIXED: use sprintf instead of printf to build the route string */
    sprintf(fullRoute, "%s → %s", from, to);

    if(strcmp(f->route, fullRoute)!=0){
        printf("Flight route mismatch! Booking cancelled.\n");
        return;
    }

    int numPassengers;
    printf("Enter number of passengers: ");
    scanf("%d",&numPassengers);

    if(f->bookedSeats + numPassengers > MAX_SEATS){
        printf("Not enough seats available! Only %d seats left.\n",
               MAX_SEATS - f->bookedSeats);
        return;
    }

    getchar(); // clear newline
    for(int i=0;i<numPassengers;i++){
        Passenger *p = (Passenger*)malloc(sizeof(Passenger));
        p->passengerID = nextPassengerID++;
        p->flightID = flightID;
        strcpy(p->from,from);
        strcpy(p->to,to);

        printf("\nPassenger %d Name: ", i+1);
        fgets(p->name,sizeof(p->name),stdin);
        p->name[strcspn(p->name,"\n")] = 0;

        printf("Age: ");
        scanf("%d",&p->age);
        getchar(); // clear newline

        printf("Class (Economy/Business/First): ");
        fgets(p->classType,sizeof(p->classType),stdin);
        p->classType[strcspn(p->classType,"\n")] = 0;

        printf("Trip Type (Single/Round/Multi): ");
        fgets(p->tripType,sizeof(p->tripType),stdin);
        p->tripType[strcspn(p->tripType,"\n")] = 0;

        printf("Are you a Student? (1-Yes / 0-No): ");
        scanf("%d",&p->isStudent);
        getchar(); // clear newline

        if(p->isStudent){
            printf("College Name: ");
            fgets(p->collegeName,sizeof(p->collegeName),stdin);
            p->collegeName[strcspn(p->collegeName,"\n")] = 0;

            printf("Roll No: ");
            fgets(p->collegeRoll,sizeof(p->collegeRoll),stdin);
            p->collegeRoll[strcspn(p->collegeRoll,"\n")] = 0;

            printf("District: ");
            fgets(p->collegeDistrict,sizeof(p->collegeDistrict),stdin);
            p->collegeDistrict[strcspn(p->collegeDistrict,"\n")] = 0;

            printf("State: ");
            fgets(p->collegeState,sizeof(p->collegeState),stdin);
            p->collegeState[strcspn(p->collegeState,"\n")] = 0;
        } else {
            strcpy(p->collegeName,"-"); strcpy(p->collegeRoll,"-");
            strcpy(p->collegeDistrict,"-"); strcpy(p->collegeState,"-");
        }

        p->amount = calculateAmount(p->classType,p->tripType,p->isStudent);
        f->bookedSeats++;
        p->next = head;
        head = p;

        printf("Passenger %d booked successfully. Amount: ₹%.2f\n", i+1,p->amount);
    }

    printf("\nAll %d passengers booked successfully!\n", numPassengers);
}

void cancelSeat() {
    int id;
    printf("Enter Passenger ID to cancel: ");
    scanf("%d",&id);

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

void viewAvailableSeats() {
    printf("\nAvailable Seats per Flight:\n");
    for(int i=0;i<4;i++)
        printf("%d - %s : %d seats left\n",
               flights[i].flightID, flights[i].flightName,
               MAX_SEATS - flights[i].bookedSeats);
}

void displayPassengerList() {
    if(!head){ printf("No passengers booked yet.\n"); return; }
    Passenger *p=head;
    printf("\nPassenger List:\n");
    while(p){
        printf("\nID:%d | Name:%s | Age:%d | Flight:%d | From:%s | To:%s\n",
               p->passengerID,p->name,p->age,p->flightID,p->from,p->to);
        printf("Class:%s | Trip:%s | Amount:₹%.2f\n",
               p->classType,p->tripType,p->amount);
        if(p->isStudent)
            printf("Student: YES | College:%s | Roll:%s | District:%s | State:%s\n",
                   p->collegeName,p->collegeRoll,p->collegeDistrict,p->collegeState);
        else
            printf("Student: NO\n");
        p=p->next;
    }
}

int main() {
    int choice;
    while(1){
        printf("\n==== Airline Seat Reservation ====\n");
        printf("1. Book Seat\n2. Cancel Booking\n3. View Available Seats\n4. Display Passenger List\n5. Exit\nEnter choice: ");
        scanf("%d",&choice);
        getchar(); // clear newline
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
