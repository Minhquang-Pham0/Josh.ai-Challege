// Josh.AI Challenge Submission
// Minh Quang Pham

// Build Instructions

1. **Using Makefile:**
   - Navigate to the project directory in the console.
   - Run the following command:
     ```
     make
     ```
   NOTE: This method was tested with MSYS2 with G++.

2. **Using Visual Studio Code:**
   - Open the project folder in Visual Studio Code.
   - Use the included 'tasks.json' file to build the project.
   - Select "Tasks: Run Build Task" from the Command Palette or press 'Ctrl+Shift+B'.

// Running Instructions

To run the project, execute the following command:
    ```
    main.exe -t <target url> -p <target port>
    ```
Both arguments are optional and will default to `localhost` and `8080` respectively.

// Functionality Documention

After getting the initial state of the system, the program will continue to poll the server every 5 seconds.
The program will then compare the saved current state of the system with new polled state and update and output
any changes it detects.
    Specifically, the program will look for the following changes:
        Added lights
        Removed lights
        Changed brightness
        Changed toggle status
In addition, the program also converts the brightness value from the byte value to a percentage value.

There are 3 specific error checking cases currently in the program:
    Network Error: In this case, the program is unable to establish a connection to the server.
    Response Error: In this case, the program is able to communicate with the server; however, the response code was not expected.
    JSON Parse Error: In this case, the received data from the server was not able to be parsed.
In all 3 scenarios, the program will wait 5 seconds and then attempt to contact the server and parse the data again. The program
will attempt to do so 3 times before exiting.

// Possible Improvements
The following are possible changes that could improve the program; however, the features and benefits were not included in the
initial scope of the challenge.

Multithreading:
    In order to improve the user experience, the program could be multithreaded. Specifically, most of the current main thread
    could be moved to a seperate thread and feeding information back to a main thread. This would allow for the main thread or
    a seperate thread to focus on user input, allowing the user to make changes to the system directly with the program as well
    as formally exiting the program, rather than closing the program.

Saving/Loading:
    The program could also be further be improved by outputting the current state of the system as a json. This would allow for
    the program to load up the previous known state of the system and display all the changes of the system since the last run
    of the program.