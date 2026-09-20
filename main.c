#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PATIENTS 100

void initializeBeds();
void registerPatient();
void showMenu();
void printBill(int idx);
void viewTriageList();
void generateAnalytics();
void saveDataToFile();
void saveBedsToFile();
void loadBedsFromFile();

// Specialty
const char SPECIALTY_NAMES[4][30] = {"General Practice (OPD)", "Paediatrics", "Cardiology", "Neurology"};
const float BASE_FEES[4] = {1500.00, 2500.00, 4500.00, 5000.00};
const int CONSULT_TIMES[4] = {15, 20, 30, 30};
const int SPECIALTY_CAPACITIES[4] = {30, 20, 12, 10};
int queueCounts[4] = {0, 0, 0, 0};

// Ward Data
const char WARD_NAMES[4][30] = {"General Ward", "Paediatric Ward", "Surgical Ward", "ICU"};
const float WARD_RATES[4] = {3000.00, 6000.00, 12000.00, 25000.00};
const int WARD_CAPACITIES[4] = {20, 10, 10, 5};

int bedOccupancy[4][20];

void initializeBeds() {
    for (int w = 0; w < 4; w++) {
        for (int b = 0; b < 20; b++) {
            bedOccupancy[w][b] = 0;
        }
    }
}

void loadBedsFromFile() {
    FILE *fp = fopen("beds_status.txt", "r");
    if (fp == NULL) {
        initializeBeds();
        return;
    }
    for (int w = 0; w < 4; w++) {
        for (int b = 0; b < WARD_CAPACITIES[w]; b++) {
            fscanf(fp, "%d", &bedOccupancy[w][b]);
        }
    }
    fclose(fp);
}

int patientIDs[MAX_PATIENTS];
char patientNames[MAX_PATIENTS][70];
int patientAges[MAX_PATIENTS];
int urgencyLevels[MAX_PATIENTS];
int assignedSpecialties[MAX_PATIENTS];
int assignedWards[MAX_PATIENTS];
int assignedBeds[MAX_PATIENTS];
int daysAdmitted[MAX_PATIENTS];
int totalPatients = 0;
float finalAmounts[MAX_PATIENTS];
float discounts[MAX_PATIENTS];

void registerPatient() {
    if (totalPatients >= MAX_PATIENTS) {
        printf("\nError: Hospital Capacity Reached!\n");
        return;
    }

    int idx = totalPatients;
    patientIDs[idx] = 1001 + idx;

    printf("\n--- New Patient Intake (PAT-%d) ---\n", patientIDs[idx]);
    printf("Enter Patient Name: ");

    fgets(patientNames[idx], 70, stdin);
    patientNames[idx][strcspn(patientNames[idx], "\n")] = 0; // enter new line removal

    printf("Enter Age: ");
    scanf("%d", &patientAges[idx]);

    printf("Urgency Level (1 = Normal, 2 = Urgent, 3 = Critical): ");
    scanf("%d", &urgencyLevels[idx]);

    // Select Specialty
    int specChoice;
    do {
        printf("Specialty ID (1-OPD, 2-Paediatrics, 3-Cardiology, 4-Neurology): ");
        scanf("%d", &specChoice);
    } while (specChoice < 1 || specChoice > 4);

    if (queueCounts[specChoice - 1] >= SPECIALTY_CAPACITIES[specChoice - 1]) {
        printf("Sorry, %s has reached its daily patient cap!\n", SPECIALTY_NAMES[specChoice - 1]);
        return;
    }

    assignedSpecialties[idx] = specChoice - 1;

    // Ward Admission
    int needWard;
    printf("Admit to Ward? (1 = Yes, 0 = No): ");
    scanf("%d", &needWard);

    if (needWard == 1) {
        printf("Select Ward (1-General, 2-Paediatric, 3-Surgical, 4-ICU): ");
        int wardChoice;
        scanf("%d", &wardChoice);
        int wIdx = wardChoice - 1;
        assignedWards[idx] = wIdx;

        printf("Enter Days Admitted: ");
        scanf("%d", &daysAdmitted[idx]);

        // Bed allocation
        int allocatedBed = -1;
        for (int b = 0; b < WARD_CAPACITIES[wIdx]; b++) {
            if (bedOccupancy[wIdx][b] == 0) {
                bedOccupancy[wIdx][b] = 1; // Mark as occupied
                allocatedBed = b + 1;
                break;
            }
        }
        assignedBeds[idx] = allocatedBed;
    } else {
        assignedWards[idx] = -1;
        assignedBeds[idx] = -1;
        daysAdmitted[idx] = 0;
    }
    printBill(idx);

    totalPatients++;
    printf("Patient PAT-%d Registered Successfully!\n", patientIDs[idx]);
}

void showMenu() {
    printf("\n====================================================\n");
    printf("        SMART HOSPITAL MANAGEMENT SYSTEM \n");
    printf("====================================================\n");
    printf("1. Register New Patient Intake\n");
    printf("2. View Priority Triage Sorting List\n");
    printf("3. Performance Reports & Analytics\n");
    printf("4. Save System Data to File\n");
    printf("5. Exit System\n");
    printf("----------------------------------------------------\n");
}
void printBill(int idx){
    int sIdx = assignedSpecialties[idx];

    float base = BASE_FEES[sIdx];
    float surcharge = 0.0f;

    if (urgencyLevels[idx] == 2) surcharge = base * 0.20f;
    else if (urgencyLevels[idx] == 3) surcharge = base * 0.50f;

    float wardCost = 0.0f;
    if (assignedWards[idx] != -1) {
        wardCost = daysAdmitted[idx] * WARD_RATES[assignedWards[idx]];
    }

    float gross = base + surcharge + wardCost;

    float discount = 0.0f;
    if (patientAges[idx] < 5 || patientAges[idx] > 65) {
        discount = gross * 0.15f;
    }

    float netPayable = gross - discount;

    discounts[idx] = discount;
    finalAmounts[idx] = netPayable;

    float waitTime = queueCounts[sIdx] * CONSULT_TIMES[sIdx];
    queueCounts[sIdx]++;

    printf("\n====================================================\n");
    printf("          SMART HOSPITAL INVOICE & ADMISSION\n");
    printf("----------------------------------------------------\n");
    printf("Patient ID     : PAT-%d\n", patientIDs[idx]);
    printf("Patient Name   : %s\n", patientNames[idx]);
    printf("Age            : %d Years\n", patientAges[idx]);
    printf("Specialty      : %s\n", SPECIALTY_NAMES[sIdx]);

    if (assignedWards[idx] != -1) {
        printf("Assigned Ward  : %s (Bed #%d)\n", WARD_NAMES[assignedWards[idx]], assignedBeds[idx]);
    } else {
        printf("Assigned Ward  : None (OPD Outpatient)\n");
    }

    printf("Urgency Level  : Level %d\n", urgencyLevels[idx]);
    printf("----------------------------------------------------\n");
    printf("Base Consultation Fee : LKR %.2f\n", base);
    printf("Emergency Surcharge   : LKR %.2f\n", surcharge);
    printf("Ward Stay Cost        : LKR %.2f\n", wardCost);
    printf("----------------------------------------------------\n");
    printf("Gross Total           : LKR %.2f\n", gross);
    printf("Age Subsidy Discount  : LKR -%.2f\n", discount);
    printf("----------------------------------------------------\n");
    printf("Final Payable Amount  : LKR %.2f\n", netPayable);
    printf("Est. Waiting Time     : %.2f mins\n", waitTime);
    printf("====================================================\n\n");
}
void viewTriageList() {
    if (totalPatients == 0) {
        printf("\nNo patients currently registered.\n");
        return;
    }

    int pos[MAX_PATIENTS];
    for (int i = 0; i < totalPatients; i++) pos[i] = i;


    for (int i = 0; i < totalPatients - 1; i++) {
        for (int j = 0; j < totalPatients - i - 1; j++) {
            if (urgencyLevels[pos[j]] < urgencyLevels[pos[j + 1]]) {
                int temp = pos[j];
                pos[j] = pos[j + 1];
                pos[j + 1] = temp;
            }
        }
    }

    printf("\n--- EMERGENCY TRIAGE PRIORITY LIST ---\n");
    for (int i = 0; i < totalPatients; i++) {
        int p = pos[i];
        printf("PAT-%d | %s | Urgency Level: %d | Specialty: %s\n",
               patientIDs[p], patientNames[p], urgencyLevels[p], SPECIALTY_NAMES[assignedSpecialties[p]]);
    }
}

void generateAnalytics() {
    if (totalPatients == 0) {
        printf("\nNo records available for analytics.\n");
        return;
    }

    printf("\n--- HOSPITAL PERFORMANCE ANALYTICS ---\n");
    printf("Total Registered Patients: %d\n", totalPatients);

    for (int w = 0; w < 4; w++) {
        int occupied = 0;
        for (int b = 0; b < WARD_CAPACITIES[w]; b++) {
            if (bedOccupancy[w][b] == 1) occupied++;
        }
        printf("%s Occupancy: %d/%d beds\n", WARD_NAMES[w], occupied, WARD_CAPACITIES[w]);
    }
}
void saveBedsToFile() {
    FILE *fp = fopen("beds_status.txt", "w");

    if (fp == NULL) {
        printf("Error opening beds status file!\n");
        return;
    }

    for (int w = 0; w < 4; w++) {
        for (int b = 0; b < WARD_CAPACITIES[w]; b++) {
            fprintf(fp, "%d ", bedOccupancy[w][b]);
        }
        fprintf(fp, "\n");
    }

    fclose(fp);
}

void saveDataToFile() {
    FILE *fp = fopen("patient_records.txt", "a");
    if (fp == NULL) {
        printf("Error opening file!\n");
        return;
    }

    for (int i = savedCount; i < totalPatients; i++) {
        fprintf(fp, "PAT-%d, %s, Age: %d, Urgency: %d\n",
                patientIDs[i], patientNames[i], patientAges[i], urgencyLevels[i]);
    }
    savedCount = totalPatients;
    fclose(fp);
    printf("\nData saved successfully to 'patient_records.txt'!\n");
}

int main() {
    loadBedsFromFile();
    int choice = 0;
    do {
        showMenu();
        printf("Enter your choice (1-5): ");
        if (scanf("%d", &choice) != 1) break;
        getchar();
        switch(choice) {
            case 1:
                registerPatient();
                break;
            case 2:
                viewTriageList();
                break;
            case 3:
                generateAnalytics();
                break;
            case 4:
                saveDataToFile();
                break;
            case 5:
                printf("Exiting system...\n");
                break;
            default:
                printf("Invalid Choice!\n");
        }
    } while (choice != 5);
    saveBedsToFile();
    return 0;
}
