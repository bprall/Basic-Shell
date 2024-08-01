#include "../include/config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>



void merge(char *arr[], size_t left, size_t mid, size_t right) {
    size_t n1 = mid - left + 1;
    size_t n2 = right - mid;

    char *L[n1], *R[n2];
    for (size_t i = 0; i < n1; i++) L[i] = arr[left + i];
    for (size_t j = 0; j < n2; j++) R[j] = arr[mid + 1 + j];

    size_t i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        if (strcmp(L[i], R[j]) <= 0) arr[k++] = L[i++];
        else arr[k++] = R[j++];
    }

    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];
}

void merge_sort(char *arr[], size_t left, size_t right) {
    if (left < right) {
        size_t mid = left + (right - left) / 2;
        merge_sort(arr, left, mid);
        merge_sort(arr, mid + 1, right);
        merge(arr, left, mid, right);
    }
}