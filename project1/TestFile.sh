#!/bin/bash

#execute: ./TestFile.sh virusesFile countriesFile 10000 duplicateAllowed

########  For debug dataset VaccineMonitor ########
#   same citizenID-firstName-...-Age -> 50% when doublicatesAllowed
#   
### inconsistent records ###
#   idia citizenID mono
#   idia citizenID + virusName
#   NO vaccinate + date -> 2% probability
#   [ Date > today date ]


Input_parameters=$#     #number of arguments without progName


# Check args
if [ $Input_parameters -lt 3 -o $Input_parameters -gt 4 ]
then
    echo "Failure usage:./TestFile.sh virusesFile countriesFile numLines [duplicatesAllowed]"

    exit 1

elif [ $Input_parameters == 3 ] 
then
    duplicateAllowed=1		# make duplicates

elif [ $Input_parameters == 4 ]
then
    duplicateAllowed=$4	
    
    if (($duplicateAllowed != 1 && $duplicateAllowed != 0))
    then
        duplicateAllowed=1
    fi
fi


virusesFile=Files/$1      # first argument
countriesFile=Files/$2
numLines=$3
File=Records/citizenRecordsFile

readarray -t virusesArray < $virusesFile        # an array of viruses
if (($? != 0))
then
    echo "File failed to open"
    exit 1
fi
VirusesLines=${#virusesArray[@]}

readarray -t countriesArray < $countriesFile
if (($? != 0))
then
    echo "File failed to open"
    exit 1
fi
CountriesLines=${#countriesArray[@]}



for ((i = 0; i < $numLines; i++))
do
    # citizenID is a number in digits of range (1, 4) 
    citizenID=$((RANDOM%10000))


    # FirstName 3 - 12 
    lengthName=$((RANDOM%13))
    if (( $lengthName < 3 ))
    then
        lengthName=3
    fi   

  
    name=$(</dev/urandom tr -dc a-z | head -c $lengthName)

    # LastName  3 - 12 characters
    lengthLastName=$((RANDOM%13))
    if (( $lengthLastName < 3 ))
    then
        lengthLastName=3
    fi 


    lastName=$(</dev/urandom tr -dc a-z | head -c $lengthLastName)


    # Country

    lineC=$((RANDOM%$CountriesLines))
    randomCountry=${countriesArray[$lineC]}

    # Age
    Age=$((RANDOM%121))
    if (($Age == 0))
    then
        Age=1
    fi

  
    # virus name
    lineV=$((RANDOM%$VirusesLines))
    randomVirus=${virusesArray[$lineV]}


    #vaccinate status
    probability=$((RANDOM%100))

    if [ $probability -lt 50 ]
    then
        vaccinated="YES "
    else
        vaccinated="NO"
    fi


    # date
    if [ $vaccinated == "YES" ]
    then
        d=$(shuf -i 1-30 -n 1)
        m=$(shuf -i 1-12 -n 1)
        y=$(shuf -i 1990-2020 -n 1)

        printf "$citizenID ${name^} ${lastName^} ${randomCountry^} ${Age} ${randomVirus} YES $d-$m-$y" >> $File

    else
        probability=$((RANDOM%100))      

        if [ $probability \< 2 ]       
        then
            d=$(shuf -i 1-30 -n 1)
            m=$(shuf -i 1-12 -n 1)
            y=$(shuf -i 1990-2021 -n 1)

            printf "${citizenID} ${name^} ${lastName^} ${randomCountry^} ${Age} ${randomVirus} NO $d-$m-$y" >> $File
            echo >> $File
            continue;          
        fi

        printf "${citizenID} ${name^} ${lastName^} ${randomCountry^} ${Age} ${randomVirus} NO" >> $File
    fi


    probability=$((RANDOM%100))      

    #   Make a duplicate record
    if [ $duplicateAllowed == 1 -a $probability -lt 60 ]
    then
        echo >> $File
        i=$((i+1))                     


        readarray -t RecordsArray < $File	//get all records in an array
        RecordsLines=${#RecordsArray[@]}
        lineR=$((RANDOM%$RecordsLines))
        rec=${RecordsArray[$lineR]}
        

        rec_array=($rec)
        citizenID=${rec_array[0]}
        name=${rec_array[1]}
        lastName=${rec_array[2]}
        randomCountry=${rec_array[3]}
        Age=${rec_array[4]}


        lineC=$((RANDOM%$VirusesLines))
        randomVirus=${virusesArray[$lineC]}

        probability=$((RANDOM%100))      
        
        #vaccinate
        if [ $probability -lt 50 ]
        then
            vaccinated="YES "
        else
            vaccinated="NO"
        fi

        # date
        if [ $vaccinated == "YES" ]
        then
            d=$(shuf -i 1-30 -n 1)
            m=$(shuf -i 1-12 -n 1)
            y=$(shuf -i 1990-2021 -n 1)


            printf "$citizenID ${name^} ${lastName^} ${randomCountry^} ${Age} ${randomVirus} YES $d-$m-$y" >> $File

        else
            printf "${citizenID} ${name^} ${lastName^} ${randomCountry^} ${Age} ${randomVirus} NO" >> $File
        fi
    fi

    echo >> $File

done
