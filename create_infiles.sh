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

# 1. Checking the number of arguments provided
echo "Checking arguments.."

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

echo "${GREEN}[COMPLETED]: ${RESET}Checking arguments."
echo

# 2. Reading files <diseasesFile> and <countriesFile>

# ------------------------------- DISEASES_FILE -------------------------------
if [ -e $DISEASES_FILE ]; then                  # exists file
  if [ -f $DISEASES_FILE ]; then                # is a regular file
    if [ -r $DISEASES_FILE ]; then              # have read rights
      echo "Reading \"$DISEASES_FILE\"...";
      DISEASES=$(cat "$DISEASES_FILE")
    else
      echo "${RED}[ERROR]: ${RESET}File \"$DISEASES_FILE\" has no reading permissions!"
    fi
  else
    echo "${RED}[ERROR]: ${RESET}File \"$DISEASES_FILE\" is not a regular file!"
  fi
else
  echo "${RED}[ERROR]: ${RESET}File \"$DISEASES_FILE\" does not exists!"
fi

echo "Printing diseases..."
for disease in ${DISEASES}; do
	echo "\t${disease}";
done

echo "${GREEN}[COMPLETED]: ${RESET}Reading \"$DISEASES_FILE\"."
echo

# ------------------------------- COUNTRIES_FILE -------------------------------
if [ -e $COUNTRIES_FILE ]; then                  # exists file
  if [ -f $COUNTRIES_FILE ]; then                # is a regular file
    if [ -r $DISEASES_FILE ]; then              # have read rights
      echo "Reading \"$COUNTRIES_FILE\"...";
      COUNTRIES=$(cat "$COUNTRIES_FILE")
    else
      echo "${RED}[ERROR]: ${RESET}File \"$COUNTRIES_FILE\" has no reading permissions!"
    fi
  else
    echo "${RED}[ERROR]: ${RESET}File \"$COUNTRIES_FILE\" is not a regular file!"
  fi
else
  echo "${RED}[ERROR]: ${RESET}File \"$COUNTRIES_FILE\" does not exists!"
fi

echo "Printing countries..."
for country in ${COUNTRIES}; do
	echo "\t${country}";
done

echo "${GREEN}[COMPLETED]: ${RESET}Reading \"$COUNTRIES_FILE\"."
echo

# 3. Creating input directory

echo "Creating input directory.."

if [[ ! -d ${INPUT_DIR} ]]; then
    mkdir ${INPUT_DIR}
    echo "${GREEN}[COMPLETED]: ${RESET}Creating input directory \"$INPUT_DIR\"."
else
  echo "${YELLOW}[EXISTS]: ${RESET}Directory \"${INPUT_DIR}\" already exits."
fi



exit 0
