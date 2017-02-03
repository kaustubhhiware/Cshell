#include<stdio.h>

int main(){

	int i,n,n2;
	//printf("Enter the length\n");
	scanf("%d",&n);
	int arr[n];
	for(i=0;i<n;i++){
		scanf("%d",&arr[i]);

	}printf("\n%d ",n);
	for(i=0;i<n;i++){
		printf("%d  ",2*arr[i]);
	}
}
