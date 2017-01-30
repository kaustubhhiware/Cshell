#include<stdio.h>

int findunique(int a[],int size);
int main(){
	
	int i,n;
	printf("Enter the length\n");
	scanf("%d",&n);
	int arr[n];
	for(i=0;i<n;i++){	
		scanf("%d",&arr[i]);
	
	}printf("\nscanned array is : ");
	for(i=0;i<n;i++){	
		printf("%d  ",arr[i]);
	}
	findunique(arr,n);
	printf("\nThe transformed array is :");
		for(i=0;i<n;i++){
		if(arr[i]!=11111)	
			printf("%d  ",arr[i]);
	}printf("\n");
	
}

int findunique(int a[],int size){
	int i,j;//printf("Ahoy!Welcome aboard!This is findunique!\n");
	for(i=0;i<size;i++){
		for(j=size-1;j>i;j--){
			
			if(a[j]==a[i])
				a[j]=11111;}
		}
		return 0;
	}
	
	

