#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Linked List for Customer Management
typedef struct Shipment {
    int shipmentID;
    char date[11];
    char status[20]; // Delivered / Not Delivered
    int deliveryTime;
    struct Shipment *next;
} Shipment;

typedef struct Customer {
    int customerID;
    char name[50];
    Shipment *shipmentHistory;
    Shipment *shipmentStack[5]; // son 5 gönderi icin Stack
    int stackTop;
    struct Customer *next;
} Customer;

Customer *customerList = NULL;

void addCustomer(int customerID, const char *name) {
    Customer *newCustomer = (Customer *)malloc(sizeof(Customer));
    newCustomer->customerID = customerID;
    strcpy(newCustomer->name, name);
    newCustomer->shipmentHistory = NULL;
    newCustomer->stackTop = -1; // Initialize stack
    newCustomer->next = customerList;
    customerList = newCustomer;
}

void pushShipment(Customer *customer, Shipment *shipment) {
    if (customer->stackTop < 4) {
        customer->stackTop++;
        customer->shipmentStack[customer->stackTop] = shipment;
    } else {
        // Shift stack to make room for new shipment
        for (int i = 0; i < 4; i++) {
            customer->shipmentStack[i] = customer->shipmentStack[i + 1];
        }
        customer->shipmentStack[4] = shipment;
    }
}

void addShipment(int customerID, int shipmentID, const char *date, const char *status, int deliveryTime) {
    Customer *current = customerList;
    while (current && current->customerID != customerID) {
        current = current->next;
    }

    if (current) {
        Shipment *newShipment = (Shipment *)malloc(sizeof(Shipment));
        newShipment->shipmentID = shipmentID;
        strcpy(newShipment->date, date);
        strcpy(newShipment->status, status);
        newShipment->deliveryTime = deliveryTime;
        newShipment->next = current->shipmentHistory;
        current->shipmentHistory = newShipment;
        pushShipment(current, newShipment);
    } else {
        printf("Customer not found!\n");
    }
}

void displayCustomers() {
    printf("PRINTING Customers:\n");
    Customer *current = customerList;
    while (current) {
        printf("Customer ID: %d, Name: %s\n", current->customerID, current->name);
        Shipment *shipment = current->shipmentHistory;
        while (shipment) {
            printf("  Shipment ID: %d, Date: %s, Status: %s, Delivery Time: %d\n",
                   shipment->shipmentID, shipment->date, shipment->status, shipment->deliveryTime);
            shipment = shipment->next;
        }
        current = current->next;
    }
}

void displayLast5Shipments(int customerID) {
    printf("PRINTING LAST 5 SHIPMENTS.\n");
    Customer *current = customerList;
    while (current && current->customerID != customerID) {
        current = current->next;
    }

    if (current) {
        if (current->stackTop == -1) {
            printf("No shipments found for Customer ID: %d\n", customerID);
            return;
        }
        printf("Last 5 shipments for Customer ID: %d:\n", customerID);
        for (int i = current->stackTop; i >= 0; i--) {
            printf("  Shipment ID: %d, Date: %s, Status: %s, Delivery Time: %d\n",
                   current->shipmentStack[i]->shipmentID, current->shipmentStack[i]->date,
                   current->shipmentStack[i]->status, current->shipmentStack[i]->deliveryTime);
        }
    } else {
        printf("Customer not found!\n");
    }
}


// Priority Queue for Delivery Prioritization
typedef struct PriorityQueue {
    int shipmentID;
    int deliveryTime;
    char status[20]; // Processing / In Transit / Delivered
    struct PriorityQueue *next;
} PriorityQueue;

PriorityQueue *priorityQueue = NULL;

void enqueue(int shipmentID, int deliveryTime, const char *status) {
    PriorityQueue *newNode = (PriorityQueue *)malloc(sizeof(PriorityQueue));
    newNode->shipmentID = shipmentID;
    newNode->deliveryTime = deliveryTime;
    strcpy(newNode->status, status);
    newNode->next = NULL;

    if (!priorityQueue || priorityQueue->deliveryTime > deliveryTime) {
        newNode->next = priorityQueue;
        priorityQueue = newNode;
    } else {
        PriorityQueue *current = priorityQueue;
        while (current->next && current->next->deliveryTime <= deliveryTime) {
            current = current->next;
        }
        newNode->next = current->next;
        current->next = newNode;
    }
}

void processNextShipment() {
    printf("PRINTING Next Shipment:\n");
    if (priorityQueue) {
        PriorityQueue *toProcess = priorityQueue;
        printf("Processing Shipment ID: %d\n", toProcess->shipmentID);
        priorityQueue = priorityQueue->next;
        free(toProcess);
    } else {
        printf("No shipments to process!\n");
    }
}

// Tree for Routing
typedef struct TreeNode {
    int cityID;
    char cityName[50];
    struct TreeNode *left;
    struct TreeNode *right;
} TreeNode;

TreeNode *createCity(int cityID, const char *cityName) {
    TreeNode *newCity = (TreeNode *)malloc(sizeof(TreeNode));
    newCity->cityID = cityID;
    strcpy(newCity->cityName, cityName);
    newCity->left = newCity->right = NULL;
    return newCity;
}

void printTree(TreeNode *root, int depth) {
    if (!root) return;
    for (int i = 0; i < depth; i++) printf("");
    printf("%s\n", root->cityName);

    if (root->left) {
        for (int i = 0; i < depth; i++) printf("      ");
        printf("     |--");
        printTree(root->left, depth + 1);
    }

    if (root->right) {
        for (int i = 0; i < depth; i++) printf("      ");
        printf("     |--");
        printTree(root->right, depth + 1);
    }
}

int calculateShortestDeliveryTime(TreeNode *root) {
    if (!root) return 0;
    if (!root->left && !root->right) return 1; // Yaprak dugum

    // INT_MAX yerine doğrudan değeri yazıyoruz
    int leftDepth = root->left ? calculateShortestDeliveryTime(root->left) : 2147483647;
    int rightDepth = root->right ? calculateShortestDeliveryTime(root->right) : 2147483647;

    return 1 + (leftDepth < rightDepth ? leftDepth : rightDepth);
}

#include <stdlib.h> // malloc ve free için

void merge(Shipment *arr[], int l, int m, int r) {
    int n1 = m - l + 1;
    int n2 = r - m;

    // Dinamik bellek tahsisi
    Shipment **L = (Shipment **)malloc(n1 * sizeof(Shipment *));
    Shipment **R = (Shipment **)malloc(n2 * sizeof(Shipment *));

    // Alt dizilere elemanları kopyala
    for (int i = 0; i < n1; i++) L[i] = arr[l + i];
    for (int j = 0; j < n2; j++) R[j] = arr[m + 1 + j];

    int i = 0, j = 0, k = l;
    while (i < n1 && j < n2) {
        if (L[i]->deliveryTime <= R[j]->deliveryTime) {
            arr[k] = L[i];
            i++;
        } else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    // Kalan elemanları kopyala
    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];

    // Dinamik belleği serbest bırak
    free(L);
    free(R);
}

void mergeSort(Shipment *arr[], int l, int r) {
    if (l < r) {
        int m = l + (r - l) / 2;
        mergeSort(arr, l, m);
        mergeSort(arr, m + 1, r);
        merge(arr, l, m, r);
    }
}

void sortUnDeliveredShipments(int customerID) {
    Customer *current = customerList;
    while (current && current->customerID != customerID) current = current->next;
    if (!current) {
        printf("Customer not found!\n");
        return;
    }

    // Copy only Not Delivered shipments
    Shipment *temp = current->shipmentHistory;
    int count = 0;
    Shipment *unDelivered[100]; // Max 100 shipments for simplicity

    while (temp) {
        if (strcmp(temp->status, "Not Delivered") == 0) {
            unDelivered[count++] = temp;
        }
        temp = temp->next;
    }

    if (count == 0) {
        printf("No Undelivered Shipments Found!\n");
        return;
    }

    // Sort Not Delivered shipments by delivery time
    mergeSort(unDelivered, 0, count - 1);

    printf("Sorted Not Delivered Shipments by Delivery Time:\n");
    for (int i = 0; i < count; i++) {
        printf("Shipment ID: %d, Delivery Time: %d, Status: %s\n",
               unDelivered[i]->shipmentID, unDelivered[i]->deliveryTime, unDelivered[i]->status);
    }
}

Shipment *binarySearch(Shipment *arr[], int l, int r, int targetID) {
    while (l <= r) {
        int mid = l + (r - l) / 2;
        if (arr[mid]->shipmentID == targetID) {
            return arr[mid];
        } else if (arr[mid]->shipmentID < targetID) {
            l = mid + 1;
        } else {
            r = mid - 1;
        }
    }
    return NULL;
}

void searchDeliveredShipment(int customerID, int targetID) {
    Customer *current = customerList;
    while (current && current->customerID != customerID) current = current->next;
    if (!current) {
        printf("Customer not found!\n");
        return;
    }

    // Copy Delivered Shipments to Array
    Shipment *temp = current->shipmentHistory;
    Shipment *delivered[100];
    int count = 0;

    while (temp) {
        if (strcmp(temp->status, "Delivered") == 0) {
            delivered[count++] = temp;
        }
        temp = temp->next;
    }

    if (count == 0) {
        printf("No Delivered Shipments Found!\n");
        return;
    }

    // Sort Delivered Shipments by shipmentID
    mergeSort(delivered, 0, count - 1);

    // Perform Binary Search
    printf("Searching for Delivered Shipment ID: %d\n", targetID);
    Shipment *result = binarySearch(delivered, 0, count - 1, targetID);

    if (result) {
        printf("Found Shipment: ID: %d, Date: %s, Delivery Time: %d\n",
               result->shipmentID, result->date, result->deliveryTime);
    } else {
        printf("Shipment ID %d not found in Delivered Shipments.\n", targetID);
    }
}

void menu() {
    int choice, customerID, shipmentID, deliveryTime;
    char name[50], date[11], status[20];

    while (1) {
        printf("\n--- MENU ---\n");
        printf("1. Yeni Musteri Ekle\n");
        printf("2. Kargo Gonderimi Ekle\n");
        printf("3. Teslim Edilen Kargolarin Durumunu Sorgula\n");
        printf("4. Gonderim Gecmisini Goruntule\n");
        printf("5. Teslim Edilmeyen Kargolari Listele (Sirali)\n");
        printf("6. Teslimat Rotalarini Goster\n");
        printf("0. Cikis\n");
        printf("Seciminiz: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                printf("Musteri ID: ");
                scanf("%d", &customerID);
                printf("Musteri Adi: ");
                scanf("%s", name);
                addCustomer(customerID, name);
                break;
            case 2:
                printf("Musteri ID: ");
                scanf("%d", &customerID);
                printf("Kargo ID: ");
                scanf("%d", &shipmentID);
                printf("Tarih (YYYY-MM-DD): ");
                scanf("%s", date);
                printf("Durum (1-Delivered/2-Not Delivered): ");
                int tempStatus;
                scanf("%d", &tempStatus);
                if (tempStatus == 1){
                    strcpy(status, "Delivered");
                }
                else{
                    strcpy(status, "Not Delivered");
                }
                printf("Teslimat Suresi: ");
                scanf("%d", &deliveryTime);
                addShipment(customerID, shipmentID, date, status, deliveryTime);
                break;
            case 3:
                printf("Musteri ID: ");
                scanf("%d", &customerID);
                printf("Aranacak Kargo ID: ");
                scanf("%d", &shipmentID);
                searchDeliveredShipment(customerID, shipmentID);
                break;
            case 4:
                printf("Musteri ID: ");
                scanf("%d", &customerID);
                displayLast5Shipments(customerID);
                break;
            case 5:
                printf("Musteri ID: ");
                scanf("%d", &customerID);
                sortUnDeliveredShipments(customerID);
                break;
            case 6:
                printf("Teslimat Rotalari Gosteriliyor... \n");
                TreeNode *root = createCity(1, "Istanbul");
                root->left = createCity(2, "Kocaeli");
                root->right = createCity(3, "Bursa");
                root->left->left = createCity(4, "Balikesir");
                root->left->right = createCity(5, "Manisa");
                printf("Agac Derinligi: %d\n\n",calculateShortestDeliveryTime(root));
                printTree(root,0);
                break;
            case 0:
                printf("Cikis Yapiliyor...\n");
                exit(0);
            default:
                printf("Gecersiz secim!\n");
        }
    }
}

int main() {
    menu();
    return 0;
}
