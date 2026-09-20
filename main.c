#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PATIENTS 100

// Specialty
const char SPECIALTY_NAMES[4][30] = {"General Practice (OPD)", "Paediatrics", "Cardiology", "Neurology"};
const float BASE_FEES[4] = {1500.00, 2500.00, 4500.00, 5000.00};
const int CONSULT_TIMES[4] = {15, 20, 30, 30};
int queueCounts[4] = {0, 0, 0, 0};

int main()
{
    printf("Smart Hospital Management System...\n");
    return 0;
}
