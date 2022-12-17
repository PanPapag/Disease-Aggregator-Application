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




