#!/bin/bash
#
# Usage: ./create_infiles.sh diseasesFile countriesFile input_dir numFilesPerDirectory numRecordsPerFile
#

usage() {
  echo "${YELLOW}[Usage]: ${RESET}./create_infiles.sh diseasesFile countriesFile input_dir numFilesPerDirectory numRecordsPerFile"
}

RED=`tput setaf 1`
GREEN=`tput setaf 2`
YELLOW=`tput setaf 3`
RESET=`tput sgr0`

echo -e "Running on: [$OSTYPE]\n"

# 1. Checking the number of arguments provided
echo "Checking arguments..."

ARGC=5
if [ "$#" -ne $ARGC ]; then
  echo "$RED[ERROR]: ${RESET}Wrong number of arguments!"
  usage
	exit 1
fi

DISEASES_FILE=$1
COUNTRIES_FILE=$2
INPUT_DIR=$3
NUM_FILES_PER_DIR=$4
NUM_RECORDS_PER_FILE=$5

if [ $NUM_FILES_PER_DIR -le 0 ]; then
  echo "${RED}[ERROR]: ${RESET}The number of files per directory should be a positive number!"
  usage
	exit 1
fi

if [ $NUM_RECORDS_PER_FILE -le 0 ]; then
  echo "${RED}[ERROR]: ${RESET}The number of records per file should be a positive number!"
  usage
	exit 1
fi

echo -e "${GREEN}[COMPLETED]: ${RESET}Checking arguments\n"

# 2. Reading files <diseasesFile> and <countriesFile>

# ------------------------------- DISEASES_FILE -------------------------------
if [ -e $DISEASES_FILE ]; then                  # exists file
  if [ -f $DISEASES_FILE ]; then                # is a regular file
    if [ -r $DISEASES_FILE ]; then              # have read rights
      echo "Reading \"$DISEASES_FILE\"...";
      DISEASES=$(cat "$DISEASES_FILE")
    else
      echo "${RED}[ERROR]: ${RESET}File \"$DISEASES_FILE\" has no reading permissions!"
      exit 1
    fi
  else
    echo "${RED}[ERROR]: ${RESET}File \"$DISEASES_FILE\" is not a regular file!"
    exit 1
  fi
else
  echo "${RED}[ERROR]: ${RESET}File \"$DISEASES_FILE\" does not exists!"
  exit 1
fi

echo "Printing diseases..."
for disease in ${DISEASES}; do
	echo -e "\t${disease}";
done

echo -e "${GREEN}[COMPLETED]: ${RESET}Reading \"$DISEASES_FILE\"\n"

# ------------------------------- COUNTRIES_FILE -------------------------------
if [ -e $COUNTRIES_FILE ]; then                  # exists file
  if [ -f $COUNTRIES_FILE ]; then                # is a regular file
    if [ -r $DISEASES_FILE ]; then              # have read rights
      echo "Reading \"$COUNTRIES_FILE\"...";
      COUNTRIES=$(cat "$COUNTRIES_FILE")
    else
      echo "${RED}[ERROR]: ${RESET}File \"$COUNTRIES_FILE\" has no reading permissions!"
      exit 1
    fi
  else
    echo "${RED}[ERROR]: ${RESET}File \"$COUNTRIES_FILE\" is not a regular file!"
    exit 1
  fi
else
  echo "${RED}[ERROR]: ${RESET}File \"$COUNTRIES_FILE\" does not exists!"
  exit 1
fi

echo "Printing countries..."
for country in ${COUNTRIES}; do
	echo -e "\t${country}";
done

echo -e "${GREEN}[COMPLETED]: ${RESET}Reading \"$COUNTRIES_FILE\"\n"

# 3. Creating input directory

echo "Creating input directory..."

if [ ! -d ${INPUT_DIR} ]; then
    mkdir ${INPUT_DIR}
    echo -e "${GREEN}[COMPLETED]: ${RESET}Creating input directory \"$INPUT_DIR\"\n"
else
  echo -e "${YELLOW}[EXISTS]: ${RESET}Directory \"${INPUT_DIR}\" already exits\n"
fi

# 4. Creating into input directory one sub-directory for each country
echo "Creating subdirectories for each country inside the file \"$COUNTRIES_FILE\"..."

INPUT_DIR_PATH="${INPUT_DIR}/"
for country in ${COUNTRIES}; do
  SUB_DIR="${INPUT_DIR_PATH}${country}"
  if [ ! -d ${SUB_DIR} ]; then
      mkdir ${SUB_DIR}
      echo "${GREEN}[COMPLETED]: ${RESET}Creating subdirectory \"${SUB_DIR}\""
  else
    echo "${YELLOW}[EXISTS]: ${RESET}Directory \"${SUB_DIR}\" already exits"
  fi
done

echo

# 5. Creating files in each subdirectory
echo "Creating files in each country subdirectory..."
for country in ${COUNTRIES}; do
  START_DATE=2020-01-01
  for ((i=1; i <= ${NUM_FILES_PER_DIR}; i++)); do
    if [[ "$OSTYPE" == "darwin"* ]]; then
      FORMATED_DATE=$(date -j -f %Y-%m-%d ${START_DATE} +%d-%m-%Y)
      START_DATE=$(date -j -v+1d -f %Y-%m-%d ${START_DATE} +%Y-%m-%d)
    elif [[ "$OSTYPE" == "linux" ]]; then
      FORMATED_DATE=$(date --date=${START_DATE} "+%d-%m-%Y")
      START_DATE=$(date -I --date=${START_DATE}"+1 day")
    fi
    FILE_PATH="${INPUT_DIR_PATH}${country}/${FORMATED_DATE}"
    if [ ! -e ${FILE_PATH} ]; then
      touch ${FILE_PATH}
      echo "${GREEN}[COMPLETED]: ${RESET}Creating file \"${FILE_PATH}\""
    else
      echo "${YELLOW}[EXISTS]: ${RESET}File \"${FILE_PATH}}\" already exits"
    fi
  done
done

exit 0
