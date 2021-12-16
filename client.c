#include <stdio.h>
#include <stdlib.h> 
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
struct message 
{
	long type;
    char command[64];
	char text[1024];
    char username[64];
    int success;
    char receiver[64];
    int pid;
    
}; 

int main()
{ 
    printf("Welcome to IPC Short Message Service\n");
    printf("MENU\nObsługa konta użytkownika:\n\tlogin\n\tlogout\n\tactive users\n\tview group members\nObsluga grup tematycznych:\n\tjoin\n\tleave\n\tview groups\nWiadomosci:\n\tsend message\n\treceive messages\nhelp\n\n");
    struct message message;
    struct message rec;
    key_t klucz = 1;
    int ipcq = msgget(klucz, IPC_CREAT | 0666);
    message.type = 2; 
    message.pid = getpid();
    rec.type = 1;
    int logged = 0;
    

    do
    {   
        if (logged == 0) {printf("Enter a command. You're not logged in\n");}
        else {printf("Enter a command. You're logged as %s\n", message.username); }

        scanf(" %[^\n]s", message.command);
        
        
        if (strcmp(message.command, "login")==0)
        {
            if (logged == 1)
            {
                printf("you have to logout from the current user first\n");
            }
            else 
            {
                printf("enter a username\n");
                scanf(" %[^\n]s",message.text);
                int wyslanie = msgsnd(ipcq, &message, sizeof(message), 0);
                if (wyslanie == -1) {printf("blad, nie wyslano\n");}
                else 
                {
                    //printf("wyslano\n");
                    
                    msgrcv(ipcq, &rec, sizeof(rec), getpid(), 0);
                    //printf("%d\n", odbior);
                    printf("%s", rec.text);
                    strcpy(message.username, message.text);
                    if (rec.success == 1) {logged = 1;}
                }
            }
        }
  
        
        else if (strcmp(message.command, "logout")==0)
        {
            if (logged == 0) {printf("you are not logged in!\n");}
            else
            {
                int wyslanie = msgsnd(ipcq, &message, sizeof(message), 0);
                if (wyslanie == -1) {printf("error, command was not sent\n");}
                else 
                {              
                    msgrcv(ipcq, &rec, sizeof(rec), getpid(), 0);
                    if (rec.success == 1) {logged = 0; printf("succesful logout\n");}
                    else {printf("unsuccesful logout\n");} 
                }
            }

        }
        else if (strcmp(message.command, "active users")==0)
        {
            int wyslanie = msgsnd(ipcq, &message, sizeof(message), 0);
            if (wyslanie == -1) {printf("error, command was not sent\n");}
                else 
                {
                    msgrcv(ipcq, &rec, sizeof(rec), getpid(), 0);
                    printf("%s\n", rec.text);
                }
        }
        else if (strcmp(message.command, "view group members")==0)
        {
            printf("enter the name of a group\n");
            scanf(" %[^\n]s",message.text);
            int wyslanie = msgsnd(ipcq, &message, sizeof(message), 0);
            if (wyslanie == -1) {printf("error, command was not sent\n");}
                else 
                {
                    msgrcv(ipcq, &rec, sizeof(rec), getpid(), 0);
                    printf("%s\n", rec.text);
                }
        }
        else if (strcmp(message.command, "join")==0)
        {
            if (logged == 0) {printf("It is required to be logged in in order to join a group!\n");}
            else 
            {
                printf("enter the name of a group\n");
                scanf(" %[^\n]s",message.text);
                int wyslanie = msgsnd(ipcq, &message, sizeof(message), 0);
                if (wyslanie == -1) {printf("error, command was not sent\n");}
                else 
                {
                    msgrcv(ipcq, &rec, sizeof(rec), getpid(), 0);
                    printf("%s\n", rec.text);
                }
            }
        }
        else if (strcmp(message.command, "leave")==0)
        {
            if (logged == 0) {printf("It is required to be logged in in order to join a group!\n");}
            else
            {
                printf("enter the name of a group\n");
                scanf(" %[^\n]s",message.text);
                int wyslanie = msgsnd(ipcq, &message, sizeof(message), 0);
                if (wyslanie == -1) {printf("error, command was not sent\n");}
                else 
                {
                    msgrcv(ipcq, &rec, sizeof(rec), getpid(), 0);
                    printf("%s\n", rec.text);
                }
            }
        }
        else if (strcmp(message.command, "view groups")==0)
        {
            if (logged == 0) {printf("Keep in mind joining groups requires to be logged in\n");}
            int wyslanie = msgsnd(ipcq, &message, sizeof(message), 0);
            if (wyslanie == -1) {printf("error, command was not sent\n");}
                else 
                {
                    msgrcv(ipcq, &rec, sizeof(rec), getpid(), 0);
                    printf("%s\n", rec.text);
                }
        }
        else if (strcmp(message.command, "send message")==0)
        {
            if (logged == 0) {printf("It is required to be logged in in order to send a message!\n");}
            else
            {   
                printf("enter the message \n");
                scanf(" %[^\n]s",message.text);
                printf("enter the username or the groupname you want to send a message to \n");
                scanf(" %[^\n]s",message.receiver);
                //printf("wysyla sie do %s", message.receiver);
                int wyslanie = msgsnd(ipcq, &message, sizeof(message), 0);
                if (wyslanie == -1) {printf("error, command was not sent\n");}
                else 
                {
                    msgrcv(ipcq, &rec, sizeof(rec), getpid(), 0);
                    printf("%s\n", rec.text);
                }
                
            }
        }
        else if (strcmp(message.command, "receive messages")==0)
        {
            if (logged == 0) {printf("It is required to be logged in in order to receive messages!\n");}
            else
            {
                int wyslanie = msgsnd(ipcq, &message, sizeof(message), 0);
                if (wyslanie == -1) {printf("error, command was not sent\n");}
                else 
                {
                    msgrcv(ipcq, &rec, sizeof(rec), getpid(), 0);
                    printf("%s\n", rec.text);
                }
            }
        }
        else if (strcmp(message.command, "help")==0)
        {
            printf("MENU\nObsługa konta użytkownika:\n\tlogin - zalogowanie\n\tlogout - wylogowanie\n\tactive users - podglad listy zalogowanych uzytkowników\n\tview group members - podglad listy czlonkow grupy tematycznej\nObsluga grup tematycznych:\n\tjoin group - dolaczenie do jednej z grup tematycznych\n\tleave group - opuszczenie grupy tematycznej\n\tview groups - podglad listy dostepnych grup\nWiadomosci:\n\tsend message - wyslanie wiadomosci do grupy lub uzytkownika\n\treceive messages - skrzynka odbiorcza, wyswietlenie nowych wiadomosci\nhelp\n\nWpisz polecenie odpowiadające opcji z menu\n");

        }

        else { printf("Command was not found, did you enter it correctly? Enter help for the list of commands\n") ;}
    }while(message.command);
}
    
