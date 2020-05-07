#!/bin/bash
#
# Usage: ./create_infiles.sh diseasesFile countriesFile input_dir numFilesPerDirectory numRecordsPerFile
#

usage() {
  echo "[Usage]: ./create_infiles.sh diseasesFile countriesFile input_dir numFilesPerDirectory numRecordsPerFile"
}
# 1. Checking the number of arguments provided
ARGC=5
if [ "$#" -ne $ARGC ]; then
  echo "[ERROR]: Wrong number of arguments!"
  usage
	exit 1
fi

#2. Reading files <diseasesFile> and <countriesFile>
DISEASE_FILE=$1
echo $DISEASE_FILE

exit 0
