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

These statistics are being sent for every input file (i.e. for every date file) by a worker. When a worker finishes reading its files and has sent all the summary statistics, it will notify the parent process via named pipe that it is ready to receive requests. If a worker receives a SIGINT or SIGQUIT signal, it will print in a file named log_file.xxx (where xxx is the process ID of the worker) the name of the countries (the subdirectories) it is handling, the total number of requests received from the parent process and the total number of requests answered successfully. 

Logfile format:

  ```
  Italy
  China
  Germany
  TOTAL 29150
  SUCCESS 25663
  FAIL 3487
  ```


