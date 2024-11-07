#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <semaphore.h>
#include <pthread.h>

typedef struct {
    double bal;
    sem_t access_sem;
    sem_t withdraw_sem;

} BankAccount;

BankAccount* init_bank_account(int intial_balance){
    BankAccount* account = malloc(sizeof(BankAccount));
    account->bal = intial_balance;
    sem_init(&account->access_sem,0,2);
    sem_init(&account->withdraw_sem,0,1);
    return account;
}

void deposit(BankAccount* account, double amount){
    sem_wait(&account->access_sem);
    account->bal += amount;
    sem_post(&account->access_sem);
}
double readBal(BankAccount* account){
    sem_wait(&account->access_sem);
    double bal = account->bal;
    printf("the current balence if %f\n", bal);
    sem_post(&account->access_sem);
    return bal;
}
void withdraw(BankAccount* account, double amount){
    sem_wait(&account->access_sem);
    sem_wait(&account->access_sem);
    sem_wait(&account->withdraw_sem); //block acess to all other threads
    if(account->bal >= amount){
        account->bal -= amount;
        printf("you have withdrawn $%f\n", amount);

    }
    else{
        printf("you have insuffient funds in your account to withdraw $%f\n", amount);
    }
    sem_post(&account->access_sem);
    sem_post(&account->access_sem);
    sem_post(&account->withdraw_sem);
}
void user1(BankAccount* account){
    int count1 = 0;
    while(count1<20){
        sleep(1);
        printf("user1 is about to remove $100 curent bal is %f\n", account->bal);
        withdraw(account, 100);
        double curBal = readBal(account);
        printf("user 1 has removed $100, new balance is %f\n", curBal);
        count1++;
    }
}
void user2(BankAccount* account){
    int count2 = 0;
    while(count2 < 20){
        sleep(1);
        printf("user 2 is going to check bal then add 500$\n");
        deposit(account, 500);
        printf("new balence is %f\n", readBal(account));
        count2++;
    }
}
void user3(BankAccount* account){
    int count = 0;
    while(count < 20){
        sleep(1);
        printf("user3 is about to remove $400 curent bal is %f\n", account->bal);
        withdraw(account, 400);
        double curBal = readBal(account);
        printf("user3 has removed $400, new balance is %f\n", curBal);
        count++;
    }
}

int main(){
    BankAccount* account = init_bank_account(10000);

    pthread_t thread1, thread2, thread3;
    printf("setting up threads now");
    pthread_create(&thread1, NULL,(void*)user1, account);
    pthread_create(&thread2, NULL,(void*)user2, account);
    pthread_create(&thread3, NULL,(void*)user3, account);

    printf("thread set up, waiting for completiong to join back\n");

    pthread_join(thread1,NULL);
    pthread_join(thread2,NULL);
    pthread_join(thread2,NULL);
    printf("threadds joined back, run ccomplete\n");

    return 0;
}