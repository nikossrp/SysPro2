# SysPro2

<p><h3>Compile</h3></p>
make

<h3><p>Usage</h3></p>
./travelMonitor –m numMonitors -b bufferSize -s sizeOfBloom -i input_dir

<h3><p>Description</h3></p>
 TravelMonitor will generate numMonitors processes. Each process is called monitorX, monitor is executing the application VaccineMonitor (see project SysPro1-VaccinateMonitor). 
 For the commands /travelRequest, /searchVaccinationStatus, /addVaccinationRecords, travelMonitor will send some request via named pipes to Monitors.
 Monitor will extract the answer. The answer will be sent back to travelMonitor via named pipe, at the end travelMonitor will print the answer to console.
 
 
<h3><p>Commands</h3></p>

<p>/travelRequest citizenID date countryFrom countryTo virusName <br/>
the answer would be one of the following </br>
REQUEST REJECTED – YOU ARE NOT VACCINATED </br>
REQUEST REJECTED – YOU WILL NEED ANOTHER VACCINATION BEFORE TRAVEL DATE </br>
REQUEST ACCEPTED – HAPPY TRAVELS </br>
</p>

/travelStats virusName date1 date2 country  </br>
print the number of travelRequests (accepted/rejected/total) for the country (country = countryTo at the previous command) on [date1, date2] </br>
e.g </br>
TOTAL REQUESTS 29150 </br>
ACCEPTED 25663 </br>
REJECTED 3487 </br>

/travelStats virusName date1 date2 </br>
print the number of travelRequests (accepted/rejected/total) on [date1, date2]
e.g </br>
TOTAL REQUESTS 29150 </br>
ACCEPTED 25663 </br>
REJECTED 3487 </br>


/addVaccinationRecords country </br>
Steps to execute the command:</br>
1.Insert a new file in directory with name country</br>
2.Execute the command /addVaccinationRecords</br>
Now, the application has updated the dataset. </br>


/searchVaccinationStatus citizenID </br>
print all informations about citizen (vaccinations, full name, age, country)


/exit </br>
Exit from application </br>
kill all children with SIGKILL </br>
close named pipes </br>
free memory

