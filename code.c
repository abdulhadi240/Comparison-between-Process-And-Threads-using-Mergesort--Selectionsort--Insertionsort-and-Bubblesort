#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<unistd.h>
#include<pthread.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<sys/ipc.h>
#include<sys/shm.h>

int array[10000];

struct data
{
    int *newArray;
    int sPoint;
    int ePoint;
};

void swap(int* a, int* b)
{
    int t = *a;
    *a = *b;
    *b = t;
}

void Combine(int *p, int a, int b)
{
    int i, j;

	for(i=a; i<b; i++)
    {
		for(j=i; j<b; j++)
		{
			if(p[i]>p[j])
			{
				int temp=p[i];
				p[i]=p[j];
				p[j]=temp;
			}
		}
	}
}

void Display(int arr[], int n)
{
	int i;

	for(i=0; i<n; i++)
    {
		printf("%d\n", arr[i]);
    }
	printf("\n");
}

void Bubble_Sort(int arr[], int n)
{
    int i, j, temp;

    for(i=0; i<n-1; i++)
    {
        for(j=n-1; j>i; j--)
        {
            if(arr[j]<arr[j-1])
            {
                temp = arr[j];
                arr[j] = arr[j-1];
                arr[j-1] = temp;
            }
        }
    }
}

void *bubblesort(void *pArray)
{
    struct data var = *(struct data *)pArray;
    int *newArray = var.newArray;
    int sPoint = var.sPoint;
    int ePoint = var.ePoint;

    int i, j;

    for(i=sPoint; i<ePoint-1; i++)
    {
       for(j=0; j<ePoint-sPoint-1; j++)
       {
           if(newArray[j]>newArray[j+1])
           {
              swap(&newArray[j], &newArray[j+1]);
           }
       }
    }

    Combine(newArray, sPoint, ePoint);
    return 0;
}

void Bubble_Sort_Process()
{
    int n, status;
    int arr[10000];
    clock_t t;

    int *shm_array;
	int shm_id;
	key_t key;
	size_t shm_size;

    printf("Parent Process Executing.\n");
    printf("Enter Size of Array : ");
    scanf("%d", &n);

    srand(time(0));

    for(int i=0; i<n; i++)
    {
        arr[i] = rand()%100;
    }

    key = IPC_PRIVATE;

	shm_size = n * sizeof(int);

	if((shm_id = shmget(key, shm_size, IPC_CREAT | 0666)) == -1)
    {
		perror("shmget");
		exit(1);
	}

	if((shm_array = (int*)shmat(shm_id, NULL, 0)) == (int *) -1)
    {
		perror("shmat");
		exit(1);
	}

	for(int i=0; i<n; i++)
    {
		shm_array[i] = arr[i];
	}

	t = clock();
	Display(shm_array, n);

    int a = fork();

    if(a<0)
    {
        perror("fork");
        exit(1);
    }
    if(a==0)
    {
        printf("Child Process Executing.\n");

        Bubble_Sort(shm_array, n);

        exit(0);
    }

    waitpid(a, &status, 0);

    Display(shm_array, n);
    t = clock() - t;

    double time = ((double)t)/CLOCKS_PER_SEC;
    printf("Time : %f\n", time);
}

void Insertion_Sort(int arr[], int n)
{
    int i, j, temp;

	for(i=1; i<n; i++)
	{
		temp = arr[i];
		j = i-1;

		while(j>=0 && arr[j]>temp)
		{
			arr[j+1] = arr[j];
			j--;
		}

		arr[j+1] = temp;
	}
}

void *insertionSort(void *pArray)
{
    struct data var = *(struct data *)pArray;
    int *newArray = var.newArray;
    int sPoint = var.sPoint;
    int ePoint = var.ePoint;

    int i, key, j;

    for(i=1; i<ePoint; i++)
    {
        key = newArray[i];
        j = i-1;

        while(j>=0 && newArray[j]>key)
        {
            newArray[j+1] = newArray[j];
            j = j-1;
        }

        newArray[j+1] = key;
    }

    Combine(newArray, sPoint, ePoint);\
    return 0;
}

void Insertion_Sort_Process()
{
    int n, status;
    int arr[10000];
    clock_t t;

    int *shm_array;
	int shm_id;
	key_t key;
	size_t shm_size;

    printf("Parent Process Executing.\n");
    printf("Enter Size of Array : ");
    scanf("%d", &n);

    srand(time(0));

    for(int i=0; i<n; i++)
    {
        arr[i] = rand()%100;
    }

    key = IPC_PRIVATE;

	shm_size = n * sizeof(int);

	if((shm_id = shmget(key, shm_size, IPC_CREAT | 0666)) == -1)
    {
		perror("shmget");
		exit(1);
	}

	if((shm_array = (int*)shmat(shm_id, NULL, 0)) == (int *) -1)
    {
		perror("shmat");
		exit(1);
	}

	for(int i=0; i<n; i++)
    {
		shm_array[i] = arr[i];
	}

	t = clock();
	Display(shm_array, n);

    int a = fork();

    if(a<0)
    {
        perror("fork");
        exit(1);
    }
    if(a==0)
    {
        printf("Child Process Executing.\n");

        Insertion_Sort(shm_array, n);

        exit(0);
    }

    waitpid(a, &status, 0);

    Display(shm_array, n);
    t = clock() - t;

    double time = ((double)t)/CLOCKS_PER_SEC;
    printf("Time : %f\n", time);
}

void Merge(int arr[], int left, int mid, int right)
{
	int i, j, k;

	int x = mid-left+1;
	int y = right-mid;

	int L[x];
	int R[y];

	for(i=0; i<x; i++)
	{
		L[i] = arr[left+i];
	}

	for(j=0; j<y; j++)
	{
		R[j] = arr[mid+1+j];
	}

	i = j = 0;
	k = left;

	while(i<x && j<y)
	{
		if(L[i] <= R[j])
		{
		    arr[k] = L[i];
		    i++;
		}
		else
		{
		    arr[k] = R[j];
		    j++;
		}

		k++;
	}

	while(i<x)
	{
		arr[k] = L[i];
		i++;
		k++;
	}

	while(j<y)
	{
		arr[k] = R[j];
		j++;
		k++;
	}
}

void MergeSort(int arr[], int left, int right)
{
	int mid, status;

	int lchild = -1;
	int rchild = -1;

    if(left<right)
    {
        mid = left+(right-left)/2;

        lchild = fork();

        if(lchild<0)
        {
            perror("fork");
            exit(1);
        }
        if(lchild==0)
        {
            MergeSort(arr, left, mid);
            exit(0);
        }
        else
        {
            rchild = fork();

            if(rchild<0)
            {
                perror("fork");
                exit(1);
            }
            if(rchild==0)
            {
                MergeSort(arr, mid+1, right);
                exit(0);
            }
        }

        waitpid(lchild, &status, 0);
        waitpid(rchild, &status, 0);
        Merge(arr, left, mid, right);
    }
}

void mergessort(int *newArray, int sPoint, int ePoint)
{
    int mid = (sPoint+ePoint)/2;

	if(mid>sPoint && mid<ePoint)
	{
		mergessort(newArray, sPoint, mid);
		mergessort(newArray, mid+1, ePoint);
		Combine(newArray, sPoint, ePoint);
	}
}

void *mergesort(void *pArray)
{
    struct data var = *(struct data *)pArray;
    int *newArray = var.newArray;
    int sPoint = var.sPoint;
    int ePoint = var.ePoint;

    int mid=(sPoint+ePoint)/2;

	if(mid>sPoint && mid <ePoint)
	{
		mergessort(newArray, sPoint, mid);
		mergessort(newArray, mid+1, ePoint);
		Combine(newArray, sPoint, ePoint);
	}
	return 0;
}

void Merge_Sort_Process()
{
    int i, array[10000];
	int n = 0;
	clock_t t1, t2;

	int *shm_array;
	int shm_id;
	key_t key;
	size_t shm_size;

	printf("Enter Size of Array : ");
	scanf("%d", &n);

	srand(time(0));

    for(i=0; i<n; i++)
    {
        array[i] = rand()%100;
    }

	key = IPC_PRIVATE;

	shm_size = n * sizeof(int);

	if((shm_id = shmget(key, shm_size, IPC_CREAT | 0666)) == -1)
    {
		perror("shmget");
		exit(1);
	}

	if((shm_array = (int*)shmat(shm_id, NULL, 0)) == (int *) -1)
    {
		perror("shmat");
		exit(1);
	}

	for(i=0; i<n; i++)
    {
		shm_array[i] = array[i];
	}

	t1 = clock();
	Display(shm_array, n);
	MergeSort(shm_array, 0, n-1);
	Display(shm_array, n);
	t2 = clock();

	double time = ((double)t2-t1)/((double)CLOCKS_PER_SEC);
    printf("Time : %f\n", time);

	if(shmdt(shm_array) == -1)
    {
		perror("shmdt");
		exit(1);
	}

	if(shmctl(shm_id, IPC_RMID, NULL) == -1)
    {
		perror("shmctl");
		exit(1);
	}
}

int Partition(int arr[], int left, int right)
{
	int temp;
	int pivot = arr[right];
	int i = left-1;

	for(int j=left; j<=right-1; j++)
	{
		if(arr[j]<=pivot)
		{
			i++;

			temp = arr[i];
			arr[i] = arr[j];
			arr[j] = temp;
		}
	}

	temp = arr[i+1];
	arr[i+1] = arr[right];
	arr[right] = temp;

	return (i+1);
}

void QuickSort(int arr[], int left, int right)
{
	int indexer, status;

	int lchild = -1;
	int rchild = -1;

    if(left<right)
    {
        indexer = Partition(arr, left, right);

        lchild = fork();

        if(lchild<0)
        {
            perror("fork");
            exit(1);
        }
        if(lchild==0)
        {
            QuickSort(arr, left, indexer-1);
            exit(0);
        }
        else
        {
            rchild = fork();

            if(rchild<0)
            {
                perror("fork");
                exit(1);
            }
            if(rchild==0)
            {
                QuickSort(arr, indexer+1, right);
                exit(0);
            }
        }

        waitpid(lchild, &status, 0);
        waitpid(rchild, &status, 0);
    }
}

void quicksort(int arr[], int low, int high)
{
	if(low<high)
	{
		int pi = Partition(arr, low, high);

		quicksort(arr, low, pi - 1);
		quicksort(arr, pi + 1, high);
		Combine(arr, low, high);
	}

}

void *quickSort(void *pArray)
{
	struct data var = *(struct data *)pArray;
    int *newArray = var.newArray;
	int sPoint = var.sPoint;
	int ePoint = var.ePoint;

    if(sPoint<ePoint)
	{
		int pi = Partition(newArray, sPoint, ePoint);

		quicksort(newArray, sPoint, pi - 1);
		quicksort(newArray, pi + 1, ePoint);
	}
	return 0;
}

void Quick_Sort_Process()
{
    int i, array[10000];
	int n = 0;
	clock_t t1, t2;

	int *shm_array;
	int shm_id;
	key_t key;
	size_t shm_size;

	printf("Enter Size of Array : ");
	scanf("%d", &n);

	srand(time(0));

    for(i=0; i<n; i++)
    {
        array[i] = rand()%100;
    }

	key = IPC_PRIVATE;

	shm_size = n * sizeof(int);

	if((shm_id = shmget(key, shm_size, IPC_CREAT | 0666)) == -1)
    {
		perror("shmget");
		exit(1);
	}

	if((shm_array = (int*)shmat(shm_id, NULL, 0)) == (int *) -1)
    {
		perror("shmat");
		exit(1);
	}

	for(i=0; i<n; i++)
    {
		shm_array[i] = array[i];
	}

	t1 = clock();
	Display(shm_array, n);
	QuickSort(shm_array, 0, n-1);
	Display(shm_array, n);
	t2 = clock();

	double time = ((double)t2-t1)/((double)CLOCKS_PER_SEC);
    printf("Time : %f\n", time);

	if(shmdt(shm_array) == -1)
    {
		perror("shmdt");
		exit(1);
	}

	if(shmctl(shm_id, IPC_RMID, NULL) == -1)
    {
		perror("shmctl");
		exit(1);
	}
}

void Heapify(int arr[], int n, int i)
{
    int largest = i;
    int left = 2*i + 1;
    int right = 2*i + 2;

    if(left<n && arr[left]>arr[largest])
    {
        largest = left;
    }

    if(right<n && arr[right]>arr[largest])
    {
        largest = right;
    }

    if(largest!=i)
    {
        int temp = arr[i];
        arr[i] = arr[largest];
        arr[largest] = temp;

        Heapify(arr, n, largest);
    }
}

void *heapSort(void *pArray)
{
	struct data var = *(struct data *)pArray;
	int *newArray = var.newArray;
	int sPoint = var.sPoint;
	int ePoint = var.ePoint;

	for(int i=ePoint/2-1; i>=sPoint; i--)
    {
        Heapify(newArray, ePoint, i);
    }

    for(int i=ePoint-1; i>=sPoint; i--)
    {
        swap(&newArray[sPoint], &newArray[i]);

        Heapify(newArray, i, sPoint);
    }

    Combine(newArray,sPoint,ePoint);
    return 0;
}

void HeapSort(int arr[], int n)
{
	int i, status;

	int lchild = -1;
	int rchild = -1;

    lchild = fork();

    if(lchild<0)
    {
        perror("fork");
        exit(1);
    }
    if(lchild==0)
    {
        for(i=n/2-1; i>=0; i--)
        {
            Heapify(arr, n, i);
        }
        exit(0);
    }
    else
    {
        rchild = fork();

        if(rchild<0)
        {
            perror("fork");
            exit(1);
        }
        if(rchild==0)
        {
            for(i=n-1; i>=0; i--)
            {
                int temp = arr[0];
                arr[0] = arr[i];
                arr[i] = temp;

                Heapify(arr, i, 0);
            }
            exit(0);
        }
    }

    waitpid(lchild, &status, 0);
    waitpid(rchild, &status, 0);
}

void Heap_Sort_Process()
{
    int i, array[10000];
	int n = 0;
	clock_t t1, t2;

	int *shm_array;
	int shm_id;
	key_t key;
	size_t shm_size;

	printf("Enter Size of Array : ");
	scanf("%d", &n);

	srand(time(0));

    for(i=0; i<n; i++)
    {
        array[i] = rand()%100;
    }

	key = IPC_PRIVATE;

	shm_size = n * sizeof(int);

	if((shm_id = shmget(key, shm_size, IPC_CREAT | 0666)) == -1)
    {
		perror("shmget");
		exit(1);
	}

	if((shm_array = (int*)shmat(shm_id, NULL, 0)) == (int *) -1)
    {
		perror("shmat");
		exit(1);
	}

	for(i=0; i<n; i++)
    {
		shm_array[i] = array[i];
	}

	t1 = clock();
	Display(shm_array, n);
	HeapSort(shm_array, n);
	Display(shm_array, n);
	t2 = clock();

	double time = ((double)t2-t1)/((double)CLOCKS_PER_SEC);
    printf("Time : %f\n", time);

	if(shmdt(shm_array) == -1)
    {
		perror("shmdt");
		exit(1);
	}

	if(shmctl(shm_id, IPC_RMID, NULL) == -1)
    {
		perror("shmctl");
		exit(1);
	}
}

void Process()
{
    int choice;

    printf("\033[1;35m");
    printf("\n\n    1. Bubble Sort");
    printf("    2. Insertion Sort");
    printf("    3. Merge Sort");
    printf("    4. Quick Sort");
    printf("    5. Heap Sort");
    printf("    6. Exit\n\n");

    printf("\033[0m");
    printf("Enter Choice :  ");
    scanf("%d", &choice);

    switch(choice)
    {
        case 1:
            {
                Bubble_Sort_Process();
                break;
            }
        case 2:
            {
                Insertion_Sort_Process();
                break;
            }
        case 3:
            {
                Merge_Sort_Process();
                break;
            }
        case 4:
            {
                Quick_Sort_Process();
                break;
            }
        case 5:
            {
                Heap_Sort_Process();
                break;
            }
        case 6:
            {
                exit(0);
            }
        default:
            {
                printf("\033[1;31m");
                printf("\nWrong Input! Enter Again.\n");
                printf("\033[0m");
            }
    }
}

void Threads()
{
    clock_t start,end;
    int i=0, count=0, choice;

	srand(time(NULL));

	for(i=0; i<10000; i++)
    {
		array[i]=rand()%100;
    }

    struct data x[4];
    int sPoint=0, ePoint=2500;

    for(i=0; i<4; i++)
    {
        x[i].newArray = array;
		x[i].sPoint = sPoint;
		x[i].ePoint = ePoint;
		sPoint += 2500;
		ePoint += 2500;
	}

    pthread_t pid[4];

    printf("\033[1;32m");
    printf("\n\n    1. Bubble Sort");
    printf("    2. Insertion Sort");
    printf("    3. Merge Sort");
    printf("    4. Quick Sort");
    printf("    5. Heap Sort");
    printf("    6. Exit\n\n");

    printf("\033[0m");
    printf("Enter Choice :  ");
    scanf("%d", &choice);

    switch(choice)
    {
        case 1:
            {
                start = clock();

                for(i=0; i<4; i++)
                {
                    pthread_create(&pid[i], NULL, bubblesort, (void *)&x[i]);
                }

                for(i=0; i<4; i++)
                {
                    pthread_join(pid[i], NULL);
                }

                end = clock();
                break;
            }
        case 2:
            {
                start = clock();

                for(i=0; i<4; i++)
                {
                    pthread_create(&pid[i], NULL, insertionSort, (void *)&x[i]);
                }

                for(i=0; i<4; i++)
                {
                    pthread_join(pid[i], NULL);
                }

                end = clock();
                break;
            }
        case 3:
            {
                start = clock();

                for(i=0; i<4; i++)
                {
                    pthread_create(&pid[i], NULL, mergesort, (void *)&x[i]);
                }

                for(i=0; i<4; i++)
                {
                    pthread_join(pid[i], NULL);
                }

                end = clock();
                break;
            }
        case 4:
            {
                start = clock();

                for(i=0; i<4; i++)
                {
                    pthread_create(&pid[i], NULL, quickSort, (void *)&x[i]);
                }

                for(i=0; i<4; i++)
                {
                    pthread_join(pid[i], NULL);
                }

                end = clock();
                break;
            }
        case 5:
            {
                start = clock();

                for(i=0; i<4; i++)
                {
                    pthread_create(&pid[i], NULL, heapSort, (void *)&x[i]);
                }

                for(i=0; i<4; i++)
                {
                    pthread_join(pid[i], NULL);
                }

                end = clock();
                break;
            }
        case 6:
            {
                exit(0);
            }
        default:
            {
                printf("\033[1;31m");
                printf("\nWrong Input! Enter Again.\n");
                printf("\033[0m");
            }
    }

    printf("\n\nAFTER SORTING: \n");

    count=0;
	for(i=0; i<10000; i++)
    {
		printf("%d ",array[i]);
		count++;
		if(count==2500)
        {
			count=0;
			printf("\n");
		}
	}

	double time = ((double)end-start)/CLOCKS_PER_SEC;
    printf("\nTime : %f\n", time);
}

int main()
{
    int choice;

    printf("\033[1;36m");
    printf("\n\tProcess Vs Thread Through Sorting\n");
    printf("\t      ____________________");

    while(1)
    {
        printf("\n\n    \033[1;35m 1. PROCESS                  \033[1;32m 2. THREADS\n");
        printf("    \033[1;33m                 3. Exit\n\n");
        printf("\033[0m");
        printf("Enter Choice :  ");
        scanf("%d", &choice);

        switch(choice)
        {
            case 1:
                {
                    Process();
                    break;
                }
            case 2:
                {
                    Threads();
                    break;
                }
            case 3:
                {
                    exit(0);
                }
            default:
                {
                    printf("\033[1;31m");
                    printf("\nWrong Input! Enter Again.\n");
                    printf("\033[0m");
                }
        }
    }

    return 0;
}