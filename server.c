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
struct user
{   
    int logstatus;
    char username[64];
    int memberstatus;
    int userID;
    int mq;
};
struct group
{
    char name[64];
    struct user users[9];
};
struct pm
{
    long type;
    char text[1024];
    char sender[64];
    char receiver[64];
};
int isingroup(char username[64], struct group gr)
{
    int is = 0;
    for (int u = 0; u < 9; ++u)
    {
        if (strcmp(username, gr.users[u].username) && gr.users[u].memberstatus == 1)
        {
            is = 1;
            break;
        }
    }
    return is;
}
    
int main()
{ 
    struct user users[9];
    
    struct group groups[3];

    
    printf("IPC Short Message Service - serwer begins working\n");

    //int read = 0;
    FILE *confile = fopen("configuration.txt", "r");
    char line[256];
    int u = 0;
    for (int i = 0; i < 3; ++i)
    {
        fgets(line, 256, confile);
        strtok(line, "\n");
        if (feof(confile)) {break;}
        strcpy(groups[i].name, line);    
    }
    while(!feof(confile))
    {   
        fgets(line, 256, confile);
        strtok(line, "\n");
        if (feof(confile)) {break;}     
        strcpy(users[u].username, line);
        ++u;
    }
    for (int u = 0; u < 9; ++u)
    {
        users[u].logstatus = 0;
        users[u].userID = u;
        users[u].mq = msgget(u, IPC_CREAT | 0644);
        for (int i = 0; i < 3; ++i)
        {
            groups[i].users[u] = users[u];
            groups[i].users[u].memberstatus = 0;
        }
    }

    struct message message;
    struct message send;
    message.type = 1;
    send.type = 1;

    key_t klucz = 1;
    int ipcq = msgget(klucz, IPC_CREAT | 0666);
        while(1)
    {
    
    if (msgrcv(ipcq, &message, sizeof(message), 2, 0) > 0 )
    {   
        send.type = message.pid;
        if (strcmp(message.command, "login")==0)
        {   
            int found = 0;
            for (int u = 0; u < 9; ++u)
            {   
                //printf(" %s \n", users[u].username);
                if(strcmp(message.text, users[u].username)==0)
                {   
                    if (users[u].logstatus == 0) 
                    {
                        users[u].logstatus = 1;

                        send.success = 1;
                        strcpy(send.text, "Successfully logged as ");
                        strcat(send.text, users[u].username);
                        strcat(send.text, "\n");
                        printf("%s has just logged in\n", users[u].username);
                        

                        
                    }
                    else
                    {
                        send.success = 0;
                        strcpy(send.text, "Error: ");
                        strcat(send.text, users[u].username);
                        strcat(send.text, " is currently logged in somewhere else\n");
                        printf("Error: %s is currently logged in somewhere else\n", users[u].username);
                        
                    }
                    ++found;
                }
                
            }
            if (found==0) 
            {
                send.success=0;
                strcpy(send.text, "Wrong username \n");
                printf("Wrong username \n");
            }
            
        }
        else if (strcmp(message.command, "logout")==0)
        {
            send.success = 0;
            for (int u = 0; u < 9; ++u)
            {
                if(strcmp(message.username, users[u].username)==0)
                {
                    users[u].logstatus = 0;
                    printf("%s has just logged out\n", users[u].username);
                    send.success = 1;
                    break;
                }
            }
            

        }
        else if (strcmp(message.command, "active users")==0)
        {   
            //printf("zalogowani użytkownicy\n");
            strcpy(send.text, "currenly active users: ");
            int found = 0;
            for (int u = 0; u < 9; ++u)
            {
                if(users[u].logstatus == 1)
                {
                    //printf("%s \n", users[u].username);
                    strcat(send.text, users[u].username);
                    strcat(send.text, ", ");
                    ++found;
                    
                }
            }
            if (found==0) 
            {
                strcat(send.text, "(No active users at the moment)\n");
                
            }
            

        }
        else if (strcmp(message.command, "view group members")==0)
        {   
            int foundg = 0;
            for (int i = 0; i < 3; ++i)
            {
                
                if (strcmp(message.text, groups[i].name)==0)
                {
                    ++foundg;
                    int foundm = 0;
                    strcpy(send.text, "users in ");
                    strcat(send.text, groups[i].name);
                    strcat(send.text, ": ");
                    for (int u = 0; u < 9; ++u)
                    {
                        if (groups[i].users[u].memberstatus == 1)
                        { 
                            strcat(send.text, groups[i].users[u].username);
                            strcat(send.text, ", ");
                            ++foundm;
                        }
                        

                    }
                    if (foundm==0) 
                    {
                        strcat(send.text, "(This groups currently has no members)\n");
                
                    }
                    break;
                }
            }
            if (foundg==0) 
            {
                strcat(send.text, "Error: this group does not exist\n");
            }
            

        }
        else if (strcmp(message.command, "join")==0)
        {
            send.success = 0;
            for (int i = 0; i < 3; ++i)
            {
                if (strcmp(groups[i].name, message.text)==0)
                {
                    for (int u = 0; u < 9; ++u) 
                    {
                        if (strcmp(message.username, groups[i].users[u].username)==0)
                        {
                            if (groups[i].users[u].memberstatus == 0)
                            {
                                groups[i].users[u].memberstatus = 1;
                                strcpy(send.text, "successful join, you're now a member of ");
                                strcat(send.text, groups[i].name);
                                
                            }
                            else 
                            {
                                strcpy(send.text, "you are already a member of this group!");
                            }
                            send.success = 1;
                            break;
                        }
                    }
                }
                
            }
            if (send.success == 0)
            {
                strcpy(send.text, "unsuccessful join - no such group");
            }
            
        }
        else if (strcmp(message.command, "leave")==0)
        {
            send.success = 0;
            for (int i = 0; i < 3; ++i)
            {
                if (strcmp(groups[i].name, message.text)==0)
                {
                    for (int u = 0; u < 9; ++u) 
                    {
                        if (strcmp(message.username, groups[i].users[u].username)==0)
                        {
                            if (groups[i].users[u].memberstatus == 1)
                            {
                                groups[i].users[u].memberstatus = 0;
                                strcpy(send.text, "successful leave, you're no longer a member of ");
                                strcat(send.text, groups[i].name);
                                
                            }
                            else 
                            {
                                strcpy(send.text, "you haven't been a member of this group!");
                            }
                            send.success = 1;
                            break;
                        }
                    }
                }
            }
            if (send.success == 0)
            {
                strcpy(send.text, "leave was unsuccessful - no such group");
            }
        }
        else if (strcmp(message.command, "view groups")==0)
        {
            strcpy(send.text, "Available groups to join: ");
            for (int i = 0; i < 3; ++i)
            {
                
                for (int u = 0; u < 9; ++u) 
                    {
                        if (strcmp(message.username, groups[i].users[u].username)==0)
                        {
                            if (groups[i].users[u].memberstatus == 0)
                            {
                                strcat(send.text, groups[i].name);  
                                strcat(send.text, ", "); 
                            }
                        
                            break;
                        }
                    }
                
            }
                        
        }
        else if (strcmp(message.command, "send message")==0)
        {
            struct pm msg;
            int found = 0;
            for (int i = 0; i < 3; ++i)
            {
                if (strcmp(groups[i].name, message.receiver)==0)
                {
                    
                    strcpy(msg.text, message.text);
                    strcpy(msg.sender, message.username);
                    strcpy(msg.receiver, message.receiver);
                    for (int u = 0; u < 9; ++u) 
                    {
                        if (groups[i].users[u].memberstatus == 1)
                        {
                            msg.type = u+10;
                            int wyslaniepm = msgsnd(users[u].mq, &msg, sizeof(msg), 0);
                            if (wyslaniepm < 0) {printf("pm was not sent\n");}
                            else {printf("pm was sent\n");}
                        }
                    }
                    found = 1;
                    break;
                    
                }
            }
            if (found == 0)
            {
                for (int u = 0; u < 9; ++u) 
                {
                    if (strcmp(users[u].username, message.receiver)==0)
                    {
                        msg.type = u+10;
                        strcpy(msg.text, message.text);
                        strcpy(msg.sender, message.username);
                        strcpy(msg.receiver, message.receiver);
                        int wyslaniepm = msgsnd(users[u].mq, &msg, sizeof(msg), 0);
                        if (wyslaniepm < 0) {printf("pm was not sent\n");}
                        else {printf("pm was sent\n");}
                        found = 1;
                        //printf("found = 1\n");
                        break;
                    }
                   
                }
            }
            if (found == 0)
            {
                message.success = 0;
                strcpy(send.text, "receiver not found\n");
            }
            else
            {
                message.success = 1;
                strcpy(send.text, "message was sent successfully\n");
            }
            
        
        }
        else if (strcmp(message.command, "receive messages")==0)
        {
            struct pm msg;
            for (int u = 0; u < 9; ++u) 
            {
                if (strcmp(users[u].username, message.username)==0)
                {
                    //printf("odbieranie\n");
                    //while(1){
                    
                    if (msgrcv(users[u].mq, &msg, sizeof(msg), u+10, IPC_NOWAIT) > 0)
                    { 
                        strcpy(send.text, "New message from ");
                        strcat(send.text, msg.sender); 
                        strcat(send.text, ": ");
                        strcat(send.text, msg.text); 
                        strcat(send.text, "\n");

                        while(msgrcv(users[u].mq, &msg, sizeof(msg), u+10, IPC_NOWAIT) > 0)
                        {
                            strcat(send.text, "New message from ");
                            strcat(send.text, msg.sender); 
                            strcat(send.text, ": ");
                            strcat(send.text, msg.text); 
                            strcat(send.text, "\n");
                        }

                    
                    }
                    else {strcpy(send.text, "no new messages");}
                    //}
                }
            }

        }
        int wyslanie = msgsnd(ipcq, &send, sizeof(send), 0);
        if (wyslanie < 0) {printf("not sent\n");}
        else {printf("sent\n");}
    }
    
    }
  
}
