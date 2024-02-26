#include "rcp_include.h"



static void opendog(void* pram)
{
	double* P = (double*)pram;
	printf("\nopendog:%lf\n", *P);

}




double lo[4] = { 1.2,3.4,54.5,22.3 }; double yu = 88;


int main(void)
{
	
	rcp_init();

	
	rcp_Category_Data_Register_Data("longitude","double",&lo,4);
	rcp_Category_Data_Register_Data("sdadh", "double", &yu, 1);
	rcp_Category_Signal_Register_Signal("opendog", "double", &yu, opendog);


	
	rcp_Category_Data_Query_Tag();
	rcp_Category_Data_Query_Data();
	rcp_Query_Category();
	
	

	return 0;
}

