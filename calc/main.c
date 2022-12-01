#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#define cls printf("\e[1;1H\e[2J")
#define nl  printf("\n")
#define cl  printf("\33[2K\r")

double BaseSal = 0.0;
double FederalTax(double BaseSal);
double StateTax(double BaseSal);
double FICATax(double BaseSal);
double NetTax(double BaseSal);

double HourlyToBaseSal(double perHour);
double BaseSalToHourly(double BaseSal);
double StringToDouble(char* str);


int main(int argc, char* argv[]){
    cls;
    if(argc < 2) { perror("\nPlease put hourly rate or Base Salary in argument.\n"); exit(-1);}
    char* input = strdup(argv[1]);
    BaseSal = StringToDouble(input);
    printf("Annual Gross Salary:\t\t$%.2f",BaseSal);nl;
    printf("Gross Hourly Rate:\t\t$%.2f",BaseSalToHourly(BaseSal));nl;
    printf("Total Taxes Due:\t\t$%.2f",NetTax(BaseSal));nl;
    printf("Take Home Annual Salary:\t$%.2f",BaseSal-NetTax(BaseSal));nl;
    printf("Take Home Monthly Salary:\t$%.2f",(BaseSal-NetTax(BaseSal))/12.0);nl;
    printf("Take Home Weekly Salary:\t$%.2f",(BaseSal-NetTax(BaseSal))/52.0);nl;
    printf("Take Home Hourly rate:\t\t$%.2f/Hr",BaseSalToHourly(BaseSal-NetTax(BaseSal)));nl;

    return 0;
}


double FederalTax(double BaseSal){
    double cur = BaseSal; //amount need to be taxed
    double taxAmt = 0.0;
    double StandardDeduction = 12550.0;
    
    double FedTaxBracket[7][2] = {
                          /*tax rate , bracket start  */
                                {0.10 , 0.0    },                  
                                {0.12 , 10275.0 },
                                {0.22 , 41775.0 },
                                {0.24 , 89075.0 },
                                {0.32 , 170050.0},
                                {0.35 , 215950.0},
                                {0.37 , 539900.0}
    
    };                          
    
    cur -= StandardDeduction; //sub std deductions and sub the tax amt bracket by bracket
    if(cur > FedTaxBracket[6][1]){ taxAmt += (cur-FedTaxBracket[6][1])*FedTaxBracket[6][0]; cur = FedTaxBracket[6][1];}
    if(cur > FedTaxBracket[5][1]){ taxAmt += (cur-FedTaxBracket[5][1])*FedTaxBracket[5][0]; cur = FedTaxBracket[5][1];}
    if(cur > FedTaxBracket[4][1]){ taxAmt += (cur-FedTaxBracket[4][1])*FedTaxBracket[4][0]; cur = FedTaxBracket[4][1];}
    if(cur > FedTaxBracket[3][1]){ taxAmt += (cur-FedTaxBracket[3][1])*FedTaxBracket[3][0]; cur = FedTaxBracket[3][1];}
    if(cur > FedTaxBracket[2][1]){ taxAmt += (cur-FedTaxBracket[2][1])*FedTaxBracket[2][0]; cur = FedTaxBracket[2][1];}
    if(cur > FedTaxBracket[1][1]){ taxAmt += (cur-FedTaxBracket[1][1])*FedTaxBracket[1][0]; cur = FedTaxBracket[1][1];}
    if(cur > FedTaxBracket[0][1]){ taxAmt += (cur-FedTaxBracket[0][1])*FedTaxBracket[0][0]; cur = FedTaxBracket[0][1];}

    return taxAmt;
}

double StateTax(double BaseSal){
    return 0.05*BaseSal;
}

double FICATax(double BaseSal){
    return 0.0765*BaseSal;
}

double NetTax(double BaseSal){
    return (FederalTax(BaseSal)+FICATax(BaseSal)+StateTax(BaseSal));
}

double HourlyToBaseSal(double perHour){
    return perHour*40*52;
}

double BaseSalToHourly(double BaseSal){
    return BaseSal/(40*52);
}

double StringToDouble(char * str){
    int l = strlen(str);
    double mul;
    double val;
   
    for(int i = 0; i< l; i++){
            mul = 1;
            for(int j = l; j >i+1 ;j--) mul = mul*10;
            double cur = ((unsigned char)str[i]-(unsigned char)'0')*mul;    
        val += cur;
    }
    return val;
}