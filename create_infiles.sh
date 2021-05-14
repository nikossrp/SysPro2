#!/bin/bash

# execute: ./create_infiles.sh inputFile input_dir numFilesPerDirectory

# Using  inputFile from project1 to create directories for every country

Input_parameters=$#

if [ $Input_parameters -ne 3 ]
then
    echo "Failure usage: ./create_infiles.sh inputFile input_dir numFilesPerDirectory"
    exit 1
fi


if [ ! -d $2 ] # create direcotry if doesn't exist
then
    mkdir -p  $2  
else 
    echo "$2 already exist"
    exit 2
fi

creat_file=$2
inputFile=$1
numFilesPerDirectory=$3     #number of files for every subdir


declare -A matrix

num_columns=2
nCountries=0
pCountry=0

counter=0


find_country(){
    all_elements=${#matrix[@]}

    num_rows=$((all_elements / num_columns))

    for ((i = 0; i < $num_rows; i++))
    do
        if [ ${matrix[$i, 0]} = $1 ] 
        then
            counter=${matrix[$i, 1]} 
            result=($i $counter)       # return (country, counter)
            return
        fi
    done

    result=(-1 -1)
    return
}



while read -r line
do
    record=($line)      # list of words
    country=${record[3]}

    find_country $country

    if ((${result[0]} == -1))   #if there isn't the country in array add it
    then
        matrix[$nCountries, 0]=$country
        matrix[$nCountries, 1]=1
        i=1
        pCountry=$nCountries
        ((nCountries++))
    else                        #if there is take the file to write
        pCountry=${result[0]}
        i=${result[1]}
    fi

    
    if (($i > $numFilesPerDirectory))
    then
        matrix[$pCountry, 1]=1
        i=1
    fi




    path=${creat_file}/${country}

    if [ ! -d ${path} ]
    then
        mkdir -p  ${path}   # create folder for country if doesn't exist
    fi

      
    File=${path}/"${country}-$i.txt"

    echo "${record[@]}" >> "$File"

    # next file until $numFilesPerDirectory, 
    ((matrix[$pCountry, 1]++))  


done < $inputFile
