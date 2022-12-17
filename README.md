# Disease-Aggregator-Application

This application creates a distributed information processing system that accepts, processes, records, and responds to questions about disease outbreaks. Specifically, the ```diseaseAggregator``` application is implemented which creates a series of ```Worker``` tasks that respond to user questions.

## Application Interface

The application will be called ```diseaseAggregator``` and will be used as follows:

```
./diseaseAggregator –w numWorkers -b bufferSize -i input_dir
```

where:
* The ```numWorkers``` parameter is the number of Worker tasks that the application will create.
* The ```bufferSize``` parameter is the size of the buffer for reading from pipes.
* The ```input_dir``` parameter is a directory that contains subdirectories with the files that the Workers will process. Each subdirectory will have the name of a country and will contain files with names that are dates in the form DD-MM-YYYY. For example, the ```input_dir``` could contain subdirectories ```China/```, ```Italy/``` and ```Germany/``` which have the following files:
  ```
  /input_dir/China/21-12-2019
  /input_dir/China/22-12-2019
  /input_dir/China/23-12-2019
  /input_dir/China/31-12-2019
  ...
  /input_dir/Italy/31-01-2020
  /input_dir/Italy/01-02-2020
  ...
  /input_dir/Germany/31-01-2020
  /input_dir/Germany/20-2-2020
  /input_dir/Germany/02-3-2020
  ```
* Each DD-MM-YYYY file contains a series of patient records where each line describes a patient who was admitted/discharged from the hospital that day and includes the recordID, name, illness, and age of the patient. For example, if the contents of the file /input_dir/Germany/02-03-2020 are:
  ```
  recordID, name, illness, age
  1, John, flu, 30
  2, Maria, pneumonia, 45
  3, Michael, COVID-19, 55
  ```
  means that in Germany, on 02-03-2020, two patients (Mary Smith and Larry Jones) with COVID-2019 and SARS-1 infections were admitted to the hospital, while Jon Dupont with H1N1 was discharged from the hospital. For an EXIT record, there must already be an ENTER record with the same recordID and an earlier admission date in the system. If this is not the case, the word ERROR is printed on a blank line (see the description of the Workers below).  Specifically, a record is an ASCII line of text that consists of the following data:
 
  1. ```recordID```: a string (may have only one digit or letter) that uniquely identifies each such record.
  2. The string ```ENTER``` or ```EXIT```: indicates admission to or discharge from the hospital.
  3. ```patientFirstName```: a string consisting of letters without spaces.
  4. ```patientLastName```: a string consisting of letters without spaces.
  5. ```disease```: a string consisting of letters, numbers, and possibly a "-" dash, but no spaces.
  6. ```age```: a positive (>0), integer <= 120.

## Application Functionality 

Starting, the diseaseAggregator application starts ```numWorkers``` Worker child processes and evenly distributes the subdirectories with the countries found in the ```input_dir``` among the Workers. It starts the Workers and updates each Worker through a named pipe about the subdirectories it will handle. The subdirectories are flat, that is, they will only contain files, not subdirectories.

When the application (the parent process) finishes creating the Worker processes, it will wait for user input (commands) from the keyboard (see below for commands).

For every worker process, for every directory assigned to it, it reads all of its files in chronological order based on the file names and fills a data structure queue that it will use to answer questions posed by the parent process. The choice of the number of named pipes and the data structures is design choice. If during file reading, the worker encounters a problematic record (e.g. syntax error or an EXIT record without a preceding ENTER record, etc.), it prints ERROR on a new line. When it finishes reading a file, the worker sends, via named pipe, to the parent process, summary statistics of the file containing the following information: for each infection, in the specific country, it sends the number of cases by age category. For example, for a file /input_dir/China/22-12-2020 containing records for SARS and COVID-19, it will send to the parent process a summary in the following format: 

  ```
  22-12-2020
  China
  SARS
  Age range 0-20 years: 10 cases
  Age range 21-40 years: 23 cases
  Age range 41-60 years: 34 cases
  Age range 60+ years: 45 cases
  
  COVID-19
  Age range 0-20 years: 20 cases
  Age range 21-40 years: 230 cases
  Age range 41-60 years: 340 cases
  Age range 60+ years: 450 cases
  ```

These statistics are being sent for every input file (i.e. for every date file) by a worker. When a worker finishes reading its files and has sent all the summary statistics, it will notify the parent process via named pipe that it is ready to receive requests. 

If a worker receives a SIGINT or SIGQUIT signal, it will print in a file named log_file.xxx (where xxx is the process ID of the worker) the name of the countries (the subdirectories) it is handling, the total number of requests received from the parent process and the total number of requests answered successfully. 

Logfile format:

  ```
  Italy
  China
  Germany
  TOTAL 29150
  SUCCESS 25663
  FAIL 3487
  ```

If a Worker receives a SIGUSR1 signal, this means that one or more new files have been placed in one of the subdirectories assigned to it. It will check the subdirectories to find the new files, read them, and update the data structures it keeps in memory. For each new file, it will send summary statistics to the parent process.

If a Worker process terminates unexpectedly, the parent process will fork a new Worker process to replace it. 

If the parent process receives SIGINT or SIGQUIT, it will first finish processing the current user command and after responding to the user, it will send a SIGKILL signal to the Workers, wait for them to terminate, and finally print to a file named log_file.xxx where xxx is the process ID of it, the name of all the countries (subdirectories) that "participated" in the application with data, the total number of requests received from the user and responded to successfully, and the total number of requests where some error occurred and/or were not completed.

The user is able to give the following commands (arguments in [] are optional):

* ```/listCountries```

  The application will print each country along with the process ID of the Worker process that handles its files. This command is useful when the user wants to add new files for a country to be processed and needs to know which Worker process to send a notification to via the SIGUSR1 signal.

  Output format: one line per country and process ID. Example:
  ```
  Canada 123
  Italy 5769
  China 5770
  ```
  
* ```/diseaseFrequency virusName date1 date2 [country]```

  If the ```country``` argument is not given, the application will print the number of cases of the disease ```virusName``` recorded in the system within the period ```[date1...date2]```. If the ```country``` argument is given, the application will print the number of cases of the disease ```virusName``` in the ```country``` recorded within the period ```[date1...date2]```. The ```date1``` and ```date2``` arguments will have the format DD-MM-YYYY.

  Output format: one line with the number of cases. Example:
  ```
  153
  ```
  
* ```/topk-AgeRanges k country disease date1 date2```

  The application will print the top k age categories for the ```country``` and ```disease``` that have reported cases of the specific disease in the specific country and the percentage of cases for them. The ```date1``` and ```date2``` arguments will be in the form DD-MM-YYYY. 
  
  Output format: one line for each age category. Example with k=2:
  ```
  20-30: 40% 
  60+: 30%
  ```
  
 * ```/searchPatientRecord recordID```

    The parent process sends the request to all Workers through named pipes and waits for a response from the Worker with the record recordID. When it receives it, it prints it.

    An example of someone who entered the hospital on 23-02-2020 and left on 28-02-2020:
    ```
    776 Larry Jones SARS-1 87 23-2-2020 28-2-2020
    ```

    An example of someone who entered the hospital on 23-02-2020 and hasn't left yet:
    ```
    776 Larry Jones SARS-1 87 23-2-2020 --
    ```
  
* ```/numPatientAdmissions disease date1 date2 [country]```

  If a ```country``` argument is given, the application will print, on a new line, the total number of patients with the ```disease``` who entered the hospital in that country within the period ```[date1 date2]```. If no ```country``` argument is given, the application will print, for each country, the number of patients with the ```disease``` who entered the hospital in the period ```[date1 date2]```. The ```date1 date2``` arguments will be in the DD-MM-YYYY format.
  
  Output format: 
  ```
  Italy 153
  Spain 769
  France 570
  ```
  
 * ```/numPatientDischarges disease date1 date2 [country]```

    If the ```country``` argument is given, the application will print, on a new line, the total number of patients with the ```disease``` who have left the hospital in that country within the period ```[date1 date2]```. If no ```country``` argument is given, the application will print, for each country, the number of patients with the ```disease``` who have left the hospital in the period ```[date1 date2]```. The ```date1 date2``` arguments will be in the DD-MM-YYYY format.

    Output format: 
    ```
    Italy 153
    Spain 769
    France 570
    ```
  
* ```/exit```
  
  Exiting the application. The parent process sends a SIGKILL signal to the Workers, waits for them to terminate, and prints to a file with the name log_file.xxx where xxx is the process ID of the parent process, the names of the subdirectories that "participated" in the application, the total number of requests received from the user and responded to successfully, and the total number of requests where an error occurred and/or were not completed. Before terminating, it will properly release all the reserved memory.
  
  
  
